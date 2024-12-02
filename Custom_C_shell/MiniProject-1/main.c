#ifndef MAIN_H
#define MAIN_H
#include "display.h"
#include "command.h"
#include "comm_log.h"
#include "alias.h"
#include "excess.h"


int main()
{
    // Set up the signal handler
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    struct sigaction sa2;
    sa2.sa_handler = handle_sigint;
    sa2.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa2.sa_mask);
    sigaction(SIGINT, &sa2, NULL);

    struct sigaction sa3;
    sa3.sa_handler = handle_sigtstp;
    sa3.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa3.sa_mask);
    sigaction(SIGTSTP, &sa3, NULL); 

    load_log(c_log, &log_count);

    parse_myshrc("myshrc");

    // Setup
    setup();

    // Get home directory buffer = 1024
    char root[1024];
    getcwd(root, 1024);

    // Initializations
    char cwd[1024];

    while (1)
    {
        // Get prompt
        getcwd(cwd, 1024);
        if (cwd == NULL)
        {
            printf("" COLOR_RED "Error in getting current working directory\n" COLOR_RESET);
            return 1;
        }
        prompt = get_prompt(sysname, username, root, cwd, command_store, time_str, count);
        printf("\033[1;36m%s\033[0m ", prompt);
        // Take input command from the user
        char command[1024];
        if (fgets(command, sizeof(command), stdin) == NULL) {
            if (feof(stdin)) {
                handle_exit(); 
            } else {
                printf("" COLOR_RED "Error in reading command\n" COLOR_RESET);
                continue;
            }
        }
        fflush(stdin);
        command[strcspn(command, "\n")] = 0;
        char original_command[1024];
        strncpy(original_command, command, sizeof(original_command));
        command_trim(command);
        token_count = 0;
        // Tokenize the command and execute it
        char **tokens = tokenize_string(command,original_command);
        execution(tokens, root, token_count);
    }
    free(last_dir);
    return 0;
}

#endif