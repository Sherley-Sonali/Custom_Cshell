#ifndef COMMAND_H
#define COMMAND_H
#include "command.h"
#include "comm_log.h"
#include "display.h"
#include "procl.h"
#include "seek_c.h"
#include "reveal.h"
#include "alias.h"
#include "activity.h"
#include "neonate.h"
#include "iman.h"

int var1 = 1;
int var2 = 1;
int do_pid = 0;
int univeral = 0;
int univeral2 = 0;
int do_pid2 = 0;
int do_pid2_d = 0;
bool status_bg = false;
char *last_dir = NULL;
char resolved_path[1024];
char *token;
char *tokens[1024];
int token_count = 0;
int job_count = 0;
Job jobs[1024]; // Definition of jobs array
int fg_job_count = 0;
Job fg_jobs[1024];
int ctrlc = 0;
char *root2 = NULL;

// Command broken to tokens
char **tokenize_string(char *str, char *original_str)
{
    //printf("Original: %s\n", original_str);
    if (strstr(original_str, "log") == NULL)
    {
        //printf("%s\n", str);
        add_command_to_log(c_log, &log_count, original_str);
        save_log(c_log, log_count);
    }
    char *token;
    token = strtok(str, ";");
    while (token != NULL)
    {
        char *start = token;
        while (*start == ' ')
            start++;

        char *end = start + strlen(start) - 1;
        while (end > start && *end == ' ')
            end--;
        end[1] = '\0';

        if (*start)
        {
            // Allocate memory and store the token in the array
            tokens[token_count] = strdup(start);
            token_count++;
        }
        token = strtok(NULL, ";");
    }
    return tokens;
}


void execution(char **tokens, char *root, int token_count)
{
    root2 = root;
    int command_count = 0;
    char *commands[1024];
    // printf("%d\n", token_count);
    for (int i = 0; i < token_count; i++)
    {
        char *token;
        command_count = 0;
        int flag_end = 0;
        // char *dup = strdup(tokens[i]);
        if (tokens[i][strlen(tokens[i]) - 1] == '&')
        {
            flag_end = 1;
        }
        token = strtok(tokens[i], "&");

        // printf("%s \n", token);
        while (token != NULL)
        {
            commands[command_count++] = strdup(token); // Store each command
            token = strtok(NULL, "&");
        }
        for (int i = 0; i < command_count; i++)
        {
            char *command = commands[i];

            // Trim leading spaces
            while (*command == ' ')
                command++;
            char *end = command + strlen(command) - 1;
            while (end > command && *end == ' ')
                end--;
            end[1] = '\0';
            int is_last_command = (i == command_count - 1);

            if (is_last_command && flag_end)
            {
                exec_pipe_bg(command, root);
            }
            else if (is_last_command)
            {
                exec_pipe_fg(command, root);
            }
            else
            {
                exec_pipe_bg(command, root);
            }
        }

        // free(commands[i]);
    }
    token_count = 0;
}

void fg_run(char *command, char *root)
{
    char *doop_command = strdup(command); // printf("Command: --%s--\n", command);
    int func_flag = 0;
    int alias_flag = 0;
    int exec_count = 1;
    char *func_command[3] = {NULL, NULL, NULL};

    command = strtok(command, " ");
    char *parameter = strtok(NULL, " ");
    char *alias_command = find_alias(command);
    if (alias_command != NULL)
    {
        command = alias_command;
        command = command + 1;
        command[strlen(command) - 1] = '\0';
        // printf("Command: %s\n", command);
        alias_flag = 1;
    }
    else
    {
        // printf("Command: %s\n", command);
        char *m = find_function(command);
        if (m != NULL)
        {
            command = m;
            // printf("--%s--\n", command);
            func_flag = 1;
        }
        if (func_flag)
        {
            tokenize_and_store(command, func_command);
            // printf("%s---\n", func_command[0]);
            // printf("%s---\n", func_command[1]);
            command_trim(func_command[0]);
            command_trim(func_command[1]);
            exec_count = 2;
            func_command[2] = parameter;
            // printf("-----%s\n", func_command[2]);
        }
    }

    // printf("Command: %s\n", command);
    if (!func_flag)
    {
        exec_count = 1;
        if (alias_flag)
        {
            command = alias_command;
            command = command + 1;
            strcat(command, " ");
            if (parameter != NULL)
                strcat(command, parameter);
            // printf("Command: %s\n", command);
        }
        else
            command = doop_command;
    }
    for (int exec = 0; exec < exec_count; exec++)
    {
        if (func_flag)
        {
            char *t = strdup(func_command[exec]);
            // printf("%s---\n", func_command[exec]);
            // printf("%s---\n", func_command[2]);
            strcat(t, " ");
            strcat(t, func_command[2]);
            command = t;
        }
        //printf("Command: %s\n", command);
        exec_comm_fg(command, root);
    }
}

int exec_comm_fg(char *command, char *root)
{
    // printf("Command: %s\n", command);
    time_t start, end;
    time_t start2, end2;
    time(&start);
    time(&start2);
    int time_taken, time_taken2;
    char command_copy[1024];
    strncpy(command_copy, command, sizeof(command_copy));
    command_copy[sizeof(command_copy) - 1] = '\0';

    // Log commands
    if (strncmp(command_copy, "log", 3) == 0)
    {
        if (strlen(command_copy) == 3)
        {
            log_count--;
            display_log(c_log, log_count);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        if (strncmp(command_copy, "log purge", 9) == 0)
        {
            log_count = 0;
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        if (strncmp(command, "log execute ", 12) == 0)
        {
            log_count--;
            int index = atoi(command_copy + 12);
            log_execute_command(c_log, &log_count, index, root);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
    }
    if (strncmp(command_copy, "iMan", 4) == 0)
    {
        char *token = strtok(command_copy, " ");
        if (token == NULL)
        {
            printf("" COLOR_ORANGE "Usage: iMan <command>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        char *man_command = strtok(NULL, " ");
        if (man_command == NULL)
        {
            printf("" COLOR_ORANGE "Usage: iMan <command>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        // printf("Command:%s", man_command);
        fetch_man_page(man_command);
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    if (strncmp(command_copy, "ping", 4) == 0)
    {
        char *token = strtok(command_copy, " ");
        if (token == NULL)
        {
            printf("" COLOR_ORANGE "Usage: ping <pid> <sig>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        char *p = strtok(NULL, " ");
        if (p == NULL)
        {
            printf("" COLOR_ORANGE "Usage: ping <pid> <sig>\n" COLOR_RESET "");
            return 1;
        }
        int pid = atoi(p);
        char *sign = strtok(NULL, " ");
        if (sign == NULL)
        {
            printf("" COLOR_ORANGE "Usage: ping <pid> <sig>\n" COLOR_RESET "");
            return 1;
        }
        int signal_num = atoi(sign);
        int mod_signal_num = signal_num % 32;
        int z = kill(pid, mod_signal_num);

        // Send the signal to the process
        if (z == -1)
        {
            // Error handling: if process doesn't exist
            if (errno == ESRCH)
            {
                printf("" COLOR_RED "No such process found\n" COLOR_RESET "");
            }
            else
            {
                printf("" COLOR_RED "Error sending signal\n" COLOR_RESET "");
            }
        }
        if (mod_signal_num == 19 || mod_signal_num == 20)
        {
            update_process_state(pid, "Stopped");
        }
        time(&end);
        time(&end2);
        time_taken2 = (int)difftime(end2, start2);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    if (strncmp(command_copy, "neonate", 7) == 0)
    {
        char *t2 = strtok(command_copy, " ");
        if (t2 == NULL)
        {
            printf("" COLOR_ORANGE "Usage: neonate -n <time>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        char *flag_n = strtok(NULL, " ");
        if (flag_n == NULL)
        {
            printf("%d\n", get_most_recent_pid());
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        else if (strncmp(flag_n, "-n", 2) != 0)
        {
            printf("" COLOR_ORANGE "Usage: neonate -n <time>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        char *time_arg = strtok(NULL, " ");
        if (time_arg == NULL)
        {
            printf("" COLOR_ORANGE "Usage: neonate -n <time>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        int time_argument = atoi(time_arg);
        while (1)
        {
            pid_t latest_pid = get_most_recent_pid();
            printf("%d\n", latest_pid);
            fflush(stdout); // Flush the output to ensure it is printed immediately

            // Sleep for the specified time
            sleep(time_argument);

            // Check if 'x' is pressed
            if (kbhit() && getchar() == 'x')
            {
                break;
            }
        }

        // Log the command execution time
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }

    if (strncmp(command_copy, "activities", 10) == 0)
    {
        print_activities();
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    // Reveal commands
    if (strncmp(command_copy, "reveal", 6) == 0)
    {

        if (strlen(command_copy) == 6)
        {
            reveal_command("~", root, last_dir);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        reveal_command(command_copy, root, last_dir);
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    if (strncmp(command_copy, "exit", 4) == 0)
    {
        char *s = "Exiting...\n";
        printf(" " COLOR_WHITE COLOR_BOLD "%s" COLOR_RESET "", s);
        exit(0);
    }
    if (strncmp(command_copy, "fg", 2) == 0)
    {
        char *t2 = strtok(command_copy, " ");
        if (t2 == NULL)
        {
            printf("" COLOR_ORANGE "Usage: fg <pid>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        char *p = strtok(NULL, " ");
        int pid_for_fg = atoi(p);
        char *fg_for_exec = (char *)malloc(1024);
        fg_for_exec = check_pid(pid_for_fg);
        if (fg_for_exec == NULL)
        {
            printf("" COLOR_RED "No such process found\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 0;
        }

        univeral = 1;
        exec_pipe_fg(fg_for_exec, root);
        // printf("--%d\n", do_pid);
        // printf("%d\n", do_pid);
        update_process_state(pid_for_fg, "Exited");
        remove_process(do_pid);
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        // printf("%d %d\n", time_taken, time_taken2);
        int k = time_taken - time_taken2 - 6;
        append_command_time(t, k);
        return 1;
    }
    if (strncmp(command_copy, "bg", 2) == 0)
    {
        char *t2 = strtok(command_copy, " ");
        if (t2 == NULL)
        {
            printf("" COLOR_ORANGE "Usage: bg <pid>\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        char *p = strtok(NULL, " ");
        int pid_for_bg = atoi(p);
        char *bg_for_exec = (char *)malloc(1024);
        bg_for_exec = check_pid(pid_for_bg);
        if (bg_for_exec == NULL)
        {
            printf("" COLOR_RED "No such process found\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 0;
        }
        // printf("%d\n", pid_for_bg);
        // kill(pid_for_bg, SIGCONT);

        univeral2 = 1;
        //do_pid2_d = pid_for_bg;
        printf("%d\n", pid_for_bg);
        exec_pipe_fg(bg_for_exec, root);
        char* bg_star = strtok(bg_for_exec, " ");
        printf("%s exited normally (%d)\n", bg_star, pid_for_bg);
        do_pid2 = jobs[job_count - 1].pid;
        remove_process(do_pid2);
        // printf("%d\n", do_pid2);
        do_pid2 = 0;
        update_process_state(pid_for_bg, "Exited");

        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken - 8);
        return 1;
    }
    // Proclore commands
    if (strncmp(command_copy, "proclore", 8) == 0)
    {

        char *token = strtok(command_copy + 8, " ");
        if (token == NULL)
        {
            proc_comm(getpid(), root);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        int pid = atoi(token);
        proc_comm(pid, root);
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    if (strncmp(command_copy, "seek", 4) == 0)
    {
        char *token = strtok(command_copy, " ");
        char *args[5] = {NULL};
        int argc = 0;
        while (token != NULL && argc < 5)
        {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        int d_flag = 0, f_flag = 0, e_flag = 0;
        char *target = NULL;
        char *start_dir = ".";
        int c = 0;
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(args[i], "-d") == 0)
            {
                d_flag = 1;
                c++;
            }
            else if (strcmp(args[i], "-f") == 0)
            {
                f_flag = 1;
                c++;
            }
            else if (strcmp(args[i], "-e") == 0)
            {
                c++;
                e_flag = 1;
            }
            else if (argc == c + 2)
            {
                target = args[i];
                break;
            }
            else if (argc == c + 3)
            {
                target = args[argc - 2];
                start_dir = args[argc - 1];
            }
        }
        if (strncmp(start_dir, "~", 1) == 0)
        {
            strcpy(start_dir, strcat(root, start_dir + 1));
        }
        if (target == NULL)
        {
            printf("" COLOR_ORANGE "Usage: seek [-d|-f] [-e] <target> [directory]\n" COLOR_RESET "");
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        else if (d_flag == 1 && f_flag == 1)
        {
            char *s = "Invalid flags!\n";
            printf(" " COLOR_RED "%s" COLOR_RESET "", s);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }

        else
        {
            char resolved_dir[MAX_PATH];
            arr[0] = 0;
            arr[1] = 0;
            found[0] = NULL;
            found[1] = NULL;
            buff[0] = '\0';
            no_match_flag = 1;
            if (strncmp(start_dir, "~", 1) == 0)
            {
                strcpy(start_dir, strcat(root, start_dir + 1));
            }
            realpath(start_dir, resolved_dir);                                          // Resolve the directory path
            seek_directory(resolved_dir, target, d_flag, f_flag, e_flag, resolved_dir); // Call the seek_directory function
            if (e_flag)
                check_e_flag(arr[0], arr[1], found[0], buff, d_flag, f_flag);
            // printf("no match flag %d\n", no_match_flag);
            check_if_match_found(no_match_flag);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        return 1;
    }

    // Hop commands
    if (strncmp(command_copy, "hop", 3) == 0)
    {

        char *token = strtok(command_copy, " ");
        if (token == NULL)
        {
            hop_command("~", root, &last_dir);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        while (token != NULL)
        {
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                hop_command(token, root, &last_dir);
            }
        }
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    else
    {
        // Attempt to execute as a system command
        pid_t pid = fork();
        if (univeral)
        {
            do_pid = pid;
            univeral = 0;
        }
        add_process(pid, command);

        fg_jobs[fg_job_count].pid = pid;
        strcpy(fg_jobs[fg_job_count].command, command);
        fg_jobs[fg_job_count].job_id = fg_job_count + 1;
        fg_job_count++;
        status_bg = true;
        if (pid == 0)
        {
            // Child process
            char *argv[1024];
            int argc = 0;

            // Tokenize the command to pass to execlp
            char *token = strtok(command_copy, " ");
            while (token != NULL)
            {
                argv[argc++] = token;
                token = strtok(NULL, " ");
            }
            argv[argc] = NULL;
            int z = execvp(argv[0], argv);
            // Execute the command using execlp
            if (z == -1)
            {
                printf("\033[31mERROR : '%s' is not a valid command\033[0m\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (pid > 0)
        {

            // Parent process: Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status))
            {
                int exit_code = WEXITSTATUS(status);
                if (exit_code == 0)
                {

                    time(&end);
                    time_taken = (int)difftime(end, start);
                    char command_copy_2[1024];
                    strncpy(command_copy_2, command, sizeof(command_copy_2));
                    char *t = strtok(command_copy_2, " ");
                    append_command_time(t, time_taken);
                }
            }
        }

        else
        {
            // Fork failed
            char *s = "ERROR : fork";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            return -1;
        }
    }

    return 1;
}
void exec_comm_bg(char *command, int background, int original_stdout, int fd_out)
{
    pid_t pid = fork();
    if (univeral2)
    {
        do_pid2 = pid;
        univeral2 = 0;
    }
    add_process(pid, command);

    char *argv[1024];

    if (pid == 0)
    {
        // Child process
        int argc = 0;
        char *token = strtok(command, " ");
        while (token != NULL)
        {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;
        if (strcmp(argv[0], "exit") == 0)
        {
            char *s = "Exiting shell...";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "\n", s);
            exit(0);
        }
        // Execute the command
        if (execvp(argv[0], argv) == -1)
        {
            char *s = "ERROR : execvp failed\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
        // Parent process
        if (background)
        {
            // Store the job
            jobs[job_count].pid = pid;
            strcpy(jobs[job_count].command, command);
            jobs[job_count].job_id = job_count + 1;
            job_count++;

            dup2(original_stdout, STDOUT_FILENO);
            printf("[%d] %d\n", job_count, pid);
            dup2(fd_out, STDOUT_FILENO);
            // Return immediately; background process is being handled asynchronously
            return;
        }
        else
        {
            // Blocking wait for foreground process
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        // Fork failed
        char *s = "ERROR : fork";
        printf(" " COLOR_RED COLOR_BOLD "%s\n" COLOR_RESET "", s);
        return;
    }
}

int hop_command(char *path, char *home, char **last_dir)
{
    char cwd[1024];

    // Resolve special cases for path
    if (strncmp(path, "~/", 2) == 0)
    {
        path = path + 1;
        strcpy(resolved_path, home);
        strcat(resolved_path, path);
        path = resolved_path;
    }
    else if (strcmp(path, "~") == 0)
    {
        path = home;
    }
    else if (strcmp(path, "-") == 0)
    {
        if (*last_dir != NULL)
        {
            path = *last_dir;
            if (chdir(path) == -1)
            {
                char *s = "ERROR : chdir - No such file or directory\n";
                printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            }
            return 1;
        }
        else
        {
            char *s = "OLDPWD not set\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);

            return 1;
        }
    }
    else if (strcmp(path, ".") == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            char *s = "ERROR : Unable to get cwd - No such file or directory\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            return 1;
        }
        printf("%s\n", cwd);
        return 1; // Current directory, do nothing
    }
    else if (strcmp(path, "..") == 0)
    {
        chdir("..");
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            char *s = "ERROR : getcwd - No such file or directory\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        }
        else
        {
            printf("%s\n", cwd);
        }
        return 1;
    }

    // Change directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        char *s = "ERROR : getcwd - No such file or directory\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        return 1;
    }

    // Save current directory as last directory before changing
    if (*last_dir)
    {
        free(*last_dir);
    }
    *last_dir = strdup(cwd);

    // Change directory to the specified path
    if (chdir(path) == -1)
    {
        char *s = "ERROR : chdir - No such file or directory\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        // perror("Error - chdir");
    }
    else
    {
        // Print the new working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            char *s = "ERROR : getcwd - No such file or directory\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            // perror("Error - getcwd");
        }
        else
        {
            printf("%s\n", cwd);
        }
    }
    return 1;
}
void append_command_time(const char *command, int time_taken)
{
    if (count < MAX_COMMANDS)
    {
        strncpy(command_store[count], command, sizeof(command_store[count]) - 1);
        command_store[count][sizeof(command_store[count]) - 1] = '\0'; // Ensure null-termination
        time_str[count] = time_taken;
        count++;
    }
    else
    {

        char *s = "Command history limit reached. Cannot store more commands.\n";
        printf(" " COLOR_ORANGE COLOR_BOLD "%s" COLOR_RESET "", s);
    }
}

void handle_sigint(int sig)
{
    // Handle Ctrl-C (SIGINT) for the foreground process
    if (fg_job_count > 0)
    {
        // Send SIGINT to the most recent foreground job
        kill(fg_jobs[fg_job_count - 1].pid, SIGINT);
    }
    else
    {
        // No foreground process running, do nothing
        ctrlc = 1;
        return;
    }
}

void handle_sigtstp(int sig)
{
    // Handle Ctrl-Z (SIGTSTP)
    if (fg_job_count > 0)
    {
        // Send SIGTSTP to the foreground job
        kill(fg_jobs[fg_job_count - 1].pid, SIGTSTP);
        update_process_state(fg_jobs[fg_job_count - 1].pid, "Stopped");

        printf("[%d] %d\n", job_count, fg_jobs[fg_job_count - 1].pid);
        exec_pipe_bg(fg_jobs[fg_job_count - 1].command, root2);
        fg_job_count--;
    }
    else
    {
        return;
    }
}

void kill_all_jobs()
{
    // Kill foreground jobs
    for (int i = 0; i < fg_job_count; ++i)
    {
        kill(fg_jobs[i].pid, SIGKILL);
    }

    // Kill background jobs
    for (int i = 0; i < job_count; ++i)
    {
        kill(jobs[i].pid, SIGKILL);
    }
}
void handle_exit()
{
    kill_all_jobs();
    free(last_dir); // Free any allocated resources
    exit(0);
}
void handle_sigchld(int sig)
{
    (void)sig; // Suppress unused variable warning

    int status;
    pid_t pid;
    if (status_bg)
    {
        for (int i = 0; i < fg_job_count; i++)
        {
            pid = fg_jobs[i].pid;
            if (WIFEXITED(status))
            {
                update_process_state(pid, "Exited");
            }
            else if (WIFSTOPPED(status))
            {
                update_process_state(pid, "Stopped");
            }
            else if (WIFSIGNALED(status))
            {
                int term_signal = WTERMSIG(status);
                if (term_signal == 9)
                {
                    update_process_state(pid, "Stopped");
                }
                else
                {
                    update_process_state(pid, "Killed");
                    remove_process(pid);
                }
            }

            else if (WIFCONTINUED(status))
            {
                update_process_state(pid, "Running");
            }
        }
        status_bg = false;
    }
    // Reap all finished child processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // Find the job associated with this PID
        for (int i = 0; i < job_count; i++)
        {
            if (jobs[i].pid == pid)
            {
                // Print process exit information
                if (WIFEXITED(status) && !ctrlc)
                {
                    char *duplicate = strdup(jobs[i].command);
                    capitalize_first_letter(duplicate);
                    char *token = strtok(duplicate, " ");
                    dup2(var1, STDOUT_FILENO);
                    if (!do_pid2)
                        printf("%s exited normally (%d)\n", token, pid);
                    else
                        printf("%s exited normally (%d)\n", token, do_pid2_d);
                    dup2(var2, STDOUT_FILENO);
                    update_process_state(pid, "Exited");
                }
                else if (WIFSTOPPED(status) && !ctrlc)
                {
                    char *duplicate = strdup(jobs[i].command);
                    capitalize_first_letter(duplicate);
                    char *token = strtok(duplicate, " ");
                    dup2(var1, STDOUT_FILENO);
                    printf("%s exited abnormally (%d)\n", token, pid);
                    dup2(var2, STDOUT_FILENO);
                    update_process_state(pid, "Stopped");
                }
                else if (WIFSIGNALED(status) && !ctrlc)
                {
                    char *duplicate = strdup(jobs[i].command);
                    capitalize_first_letter(duplicate);
                    char *token = strtok(duplicate, " ");
                    dup2(var1, STDOUT_FILENO);
                    printf("%s exited abnormally (%d)\n", token, pid);
                    dup2(var2, STDOUT_FILENO);
                    int term_signal = WTERMSIG(status);
                    // printf("Received termination signal: %d\n", term_signal);
                    if (term_signal == 9 || term_signal == 15)
                    {
                        update_process_state(pid, "Killed");
                        remove_process(pid);
                    }
                    else
                    {
                        update_process_state(pid, "Stopped");
                    }
                }

                else if (WIFCONTINUED(status) && !ctrlc)
                {
                    update_process_state(pid, "Running");
                }

                // Remove the job from the list
                for (int j = i; j < job_count - 1; j++)
                {
                    jobs[j] = jobs[j + 1];
                }
                job_count--;
                break;
            }
        }
    }
}


void io_direction_bg(char *cmd)
{
    // printf("Command: %s--\n", cmd);
    char command_io[1024] = "";
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0; // 0 for '>', 1 for '>>'
    int i = 0;

    // Save the original stdin and stdout file descriptors
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);

    // Process the command string to handle redirection
    while (cmd[i] != '\0')
    {
        if (cmd[i] == '<')
        {
            // Handle input redirection
            cmd[i] = '\0'; // Terminate the command before '<'
            i++;
            while (cmd[i] == ' ')
                i++;              // Skip any spaces
            input_file = &cmd[i]; // Set input file
            while (cmd[i] != ' ' && cmd[i] != '\0' && cmd[i] != '>')
                i++; // Find the end of the filename
            if (cmd[i] != '\0' && cmd[i] != '>')
            {
                cmd[i] = '\0'; // Terminate the filename
                i++;
            }
        }
        else if (cmd[i] == '>')
        {
            if (cmd[i + 1] == '>')
            {
                append = 1;
                cmd[i] = '\0'; // Terminate the command before '>>'
                i += 2;        // Skip '>>'
            }
            else
            {
                // Handle overwrite output redirection
                cmd[i] = '\0'; // Terminate the command before '>'
                i++;
            }
            while (cmd[i] == ' ')
                i++;               // Skip any spaces
            output_file = &cmd[i]; // Set output file
            while (cmd[i] != ' ' && cmd[i] != '\0' && cmd[i] != '<')
                i++; // Find the end of the filename
            if (cmd[i] != '\0' && cmd[i] != '<')
            {
                cmd[i] = '\0'; // Terminate the filename
                i++;
            }
        }
        else
        {
            i++;
        }
    }
    strncpy(command_io, cmd, sizeof(command_io) - 1);
    command_io[sizeof(command_io) - 1] = '\0'; // Ensure null-termination

    if (input_file)
    {
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in < 0)
        {
            printf("" COLOR_RED "No such input file found!\n" COLOR_RESET "");
            return;
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
    int fd = original_stdout;
    // Handle Output Redirection
    if (output_file)
    {
        int fd_out;
        if (append)
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (fd_out < 0)
        {
            printf("" COLOR_RED "Error opening output file\n" COLOR_RESET "");
            return;
        }
        fd = fd_out;
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
    // Execute the command with the modified input/output
    exec_comm_bg(command_io, 1, original_stdout, fd);
    var1 = var2 = 1;

    // Restore original stdin and stdout
    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);

    // Close the duplicated file descriptors
    close(original_stdin);
    close(original_stdout);
}
void exec_pipe_fg(char *cmd, char *root)
{
    char *dup_pipe = strdup(cmd);
    char *pipe_commands[256];
    int num_commands = 0;
    int pipe_fds[2];
    pid_t pid;
    int fd_in = 0; // Input for the first command is STDIN
    int og_o = dup(STDOUT_FILENO);
    int og_i = dup(STDIN_FILENO);

    for (int i = 0; i < strlen(cmd); i++)
    {
        if (cmd[i] == '|')
        {
            break;
        }
        if (i == strlen(cmd) - 1)
        {
            io_direction_fg(dup_pipe, root);
            return;
        }
    }
    if (check_pipe_validity(cmd) == -1)
    {
        return;
    }
    // Split the command by '|'
    char *token = strtok(cmd, "|");

    while (token != NULL)
    {
        pipe_commands[num_commands++] = token;

        command_trim(token); // Ensure to trim the command of leading/trailing spaces
        token = strtok(NULL, "|");
    }

    for (int i = 0; i < num_commands; i++)
    {
        if (pipe(pipe_fds) == -1)
        {
            printf("" COLOR_RED "Error creating pipe\n" COLOR_RESET "");
            return;
        }

        if ((pid = fork()) == -1)
        {
            printf("" COLOR_RED "Error forking process\n" COLOR_RESET "");
            return;
        }
        else if (pid == 0)
        { 
            add_process(pid, pipe_commands[i]);
            fg_jobs[fg_job_count].pid = pid;
            strcpy(fg_jobs[fg_job_count].command, pipe_commands[i]);
            fg_jobs[fg_job_count].job_id = fg_job_count + 1;
            fg_job_count++;
            status_bg = true;
            if (fd_in != 0)
            {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            // Set up output redirection
            if (i < num_commands - 1)
            {
                dup2(pipe_fds[1], STDOUT_FILENO);
            }
            close(pipe_fds[0]);
            close(pipe_fds[1]);

            // Execute the command (without further redirection since it's handled by the pipes)
            io_direction_fg(pipe_commands[i], root);
            exit(EXIT_SUCCESS);
        }
        else
        {                        // Parent process
            wait(NULL);          // Wait for the child process to finish
            close(pipe_fds[1]);  // Close the write end of the pipe in the parent
            fd_in = pipe_fds[0]; // Save the read end of the pipe for the next command
        }
    }

    // Restore original stdin and stdout
    dup2(og_o, STDOUT_FILENO);
    dup2(og_i, STDIN_FILENO);
    close(og_o);
    close(og_i);
}

void exec_pipe_bg(char *cmd, char *root)
{
    char dup2_bg[1024] = "";
    int last_fd;
    char buffer[1024];
    char *dup_pipe = strdup(cmd);
    char *pipe_commands[256];
    int num_commands = 0;
    int pipe_fds[2];
    pid_t pid;
    int fd_in = 0; // Input for the first command is STDIN
    int og_o = dup(STDOUT_FILENO);
    int og_i = dup(STDIN_FILENO);
    for (int i = 0; i < strlen(cmd); i++)
    {
        if (cmd[i] == '|')
        {
            break;
        }
        if (i == strlen(cmd) - 1)
        {
            io_direction_bg(dup_pipe);
            return;
        }
    }
    if (check_pipe_validity(cmd) == -1)
    {
        return;
    }
    // Split the command by '|'
    char *token = strtok(cmd, "|");

    while (token != NULL)
    {
        pipe_commands[num_commands++] = token;

        command_trim(token); // Ensure to trim the command of leading/trailing spaces
        token = strtok(NULL, "|");
    }
    for (int i = 0; i < num_commands; i++)
    {
        if (pipe(pipe_fds) == -1)
        {
            printf("" COLOR_RED "Error creating pipe\n" COLOR_RESET "");
            return;
        }

        if ((pid = fork()) == -1)
        {
            printf("" COLOR_RED "Error forking process\n" COLOR_RESET "");
            return;
        }
        else if (pid == 0)
        { // Child process
            add_process(pid, pipe_commands[i]);
            fg_jobs[fg_job_count].pid = pid;
            strcpy(fg_jobs[fg_job_count].command, pipe_commands[i]);
            fg_jobs[fg_job_count].job_id = fg_job_count + 1;
            fg_job_count++;
            status_bg = true;
            if (fd_in != 0)
            {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (i < num_commands - 1)
            {
                dup2(pipe_fds[1], STDOUT_FILENO);
            }
            close(pipe_fds[0]);
            close(pipe_fds[1]);

            if (i < num_commands - 1)
            {
                io_direction_fg(pipe_commands[i], root);
            }
            else
            {
            }
            exit(EXIT_SUCCESS);
        }
        else
        {                       // Parent process
            wait(NULL);         // Wait for the child process to finish
            close(pipe_fds[1]); // Close the write end of the pipe in the parent
            if (i == num_commands - 1)
            {
            }
            else
            {
                fd_in = pipe_fds[0]; // Save the read end of the pipe for the next command
            }
        }
    }
    last_fd = fd_in;
    // printf("Last FD: %d\n", last_fd);
    FILE *f = fdopen(last_fd, "r");
    if (f == NULL)
    {
        printf("" COLOR_RED "Error opening file\n" COLOR_RESET "");
        return;
    }
    while (fgets(buffer, sizeof(buffer), f))
    {
        FILE *f2 = fopen("tempo.txt", "w");
        if (f2 == NULL)
        {
            printf("" COLOR_RED "Error opening file\n" COLOR_RESET "");
            return;
        }
        fprintf(f2, "%s", buffer);
        strcat(dup2_bg, pipe_commands[num_commands - 1]);
        strcat(dup2_bg, "<tempo.txt");
        // printf("Command: %s\n", dup2_bg);
        io_direction_bg(dup2_bg);

        fclose(f2);
        fd_in = pipe_fds[0];
    }

    // Restore original stdin and stdout
    dup2(og_o, STDOUT_FILENO);
    dup2(og_i, STDIN_FILENO);
    close(og_o);
    close(og_i);
}

void io_direction_fg(char *cmd, char *root)
{
    char command_io[1024] = "";
    char *input_file = NULL;
    char *output_file = NULL;
    
    int append = 0; // 0 for '>', 1 for '>>'
    int i = 0;

    // Save the original stdin and stdout file descriptors
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);

    // Process the command string to handle redirection
    while (cmd[i] != '\0')
    {
        if (cmd[i] == '<')
        {
            // Handle input redirection
            cmd[i] = '\0'; // Terminate the command before '<'
            i++;
            while (cmd[i] == ' ')
                i++;              // Skip any spaces
            input_file = &cmd[i]; // Set input file
            while (cmd[i] != ' ' && cmd[i] != '\0' && cmd[i] != '>')
                i++; // Find the end of the filename
            if (cmd[i] != '\0' && cmd[i] != '>')
            {
                cmd[i] = '\0';
                i++;
            }
        }
        else if (cmd[i] == '>')
        {
            if (cmd[i + 1] == '>')
            {
                // Handle append output redirection
                append = 1;
                cmd[i] = '\0'; // Terminate the command before '>>'
                i += 2;        // Skip '>>'
            }
            else
            {
                // Handle overwrite output redirection
                cmd[i] = '\0'; // Terminate the command before '>'
                i++;
            }
            while (cmd[i] == ' ')
                i++;               // Skip any spaces
            output_file = &cmd[i]; // Set output file
            while (cmd[i] != ' ' && cmd[i] != '\0' && cmd[i] != '<')
                i++; // Find the end of the filename
            if (cmd[i] != '\0' && cmd[i] != '<')
            {
                cmd[i] = '\0'; // Terminate the filename
                i++;
            }
        }
        else
        {
            i++;
        }
    }
    strncpy(command_io, cmd, sizeof(command_io) - 1);
    command_io[sizeof(command_io) - 1] = '\0'; // Ensure null-termination
    // printf("Command: %s--\n", command_io);

    // Handle Input Redirection
    if (input_file)
    {
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in < 0)
        {
            printf("" COLOR_RED "No such input file found!\n" COLOR_RESET "");
            return;
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }

    // Handle Output Redirection
    if (output_file)
    {
        int fd_out;
        if (append)
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (fd_out < 0)
        {
            printf("" COLOR_RED "Error opening output file\n" COLOR_RESET "");
            return;
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }

    // Execute the command with the modified input/output
    fg_run(command_io, root);

    // Restore original stdin and stdout
    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);

    // Close the duplicated file descriptors
    close(original_stdin);
    close(original_stdout);
}


void capitalize_first_letter(char *str)
{
    if (str == NULL || str[0] == '\0')
    {
        // If the string is empty or null, do nothing
        return;
    }

    // Capitalize the first character if it's a letter
    str[0] = toupper(str[0]);
}
void command_trim(char *str)
{
    if (str == NULL)
        return;

    size_t len = strlen(str);
    size_t i = 0, j = 0;

    while (i < len && isspace((unsigned char)str[i]))
    {
        i++;
    }

    if (i == len)
    {
        str[0] = ' ';
        str[1] = '\0';
        return;
    }

    while (i < len)
    {
        if (isspace((unsigned char)str[i]))
        {
            if (j > 0 && str[j - 1] != ' ')
            {
                str[j++] = ' ';
            }
        }
        else
        {
            str[j++] = str[i];
        }
        i++;
    }

    str[j] = '\0';
    size_t end = j;
    while (end > 0 && isspace((unsigned char)str[end - 1]))
    {
        end--;
    }
    str[end] = '\0';
}

int check_pipe_validity(char *input)
{
    // Find the position of the pipe character '|'
    const char *pipe_pos = strchr(input, '|');

    // If no pipe is found, it's invalid
    if (pipe_pos == NULL)
    {
        printf("" COLOR_RED "Invalid\n" COLOR_RESET "");
        return -1;
    }

    // Extract the parts before and after the pipe
    int before_pipe_len = pipe_pos - input;
    int after_pipe_len = strlen(pipe_pos + 1); // +1 to skip the '|'

    // Check if both sides have non-empty strings
    if (before_pipe_len > 0 && after_pipe_len > 0)
    {
        // if(input[before_pipe_len - 1] == ' ' || input[before_pipe_len + 1] == ' ') {
        //     printf("Invalid use of pipe\n");
        //     return -1;
        // }
    }
    else
    {
        printf("" COLOR_RED "Invalid use of pipe\n" COLOR_RESET "");
        return -1;
    }
    return 1;
}


#endif