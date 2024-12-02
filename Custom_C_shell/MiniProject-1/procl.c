#include "procl.h"

void print_path_relative_to_cwd( char *path,char *cwd) {
    char relative_path[MAX_PATH];
    
    // Check if path starts with the current working directory
    if (strncmp(path, cwd, strlen(cwd)) == 0) {
        // Path is relative to the current working directory
        snprintf(relative_path, sizeof(relative_path), "~%s", path + strlen(cwd));
    } 
     else {
        // Path is neither relative to current working directory 
        snprintf(relative_path, sizeof(relative_path), "%s", path);
    }

    printf("executable path : %s\n", relative_path);
}

int proc_comm(int pid, char* root)
{
    char path[256];
    char status;
    //char buffer[1024];
    char exec_path[256];
    int vm_size;
    int process_group;

    if (kill(pid, 0) == -1 && errno == ESRCH)
    {
        printf("" COLOR_RED COLOR_BOLD "ERROR : " COLOR_RESET "");
        printf("Process %d does not exist.\n", pid);        
        return -1;
    }

    // Open /proc/[pid]/stat to get status, process group
    sprintf(path, "/proc/%d/stat", pid);
    FILE *stat_file = fopen(path, "r");
    if (!stat_file)
    {
        char *s = "ERROR : opening stat file\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        //perror("Error opening stat file");
        return -1;
    }

    fscanf(stat_file, "%*d %*s %c %d", &status, &process_group);
    fclose(stat_file);

    char proc_status[3] = {status, '\0', '\0'};
    if (tcgetpgrp(STDIN_FILENO) != process_group)
    {
        proc_status[1] = '+';
    }

    // Open /proc/[pid]/statm to get virtual memory size
    sprintf(path, "/proc/%d/statm", pid);
    FILE *statm_file = fopen(path, "r");
    if (!statm_file)
    {
        char *s = "ERROR : opening statm file\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        //perror("Error opening statm file");
        return -1;
    }

    fscanf(statm_file, "%d", &vm_size);
    fclose(statm_file);

    // Get the executable path
    sprintf(path, "/proc/%d/exe", pid);
    ssize_t len = readlink(path, exec_path, sizeof(exec_path) - 1);
    if (len != -1)
    {
        exec_path[len] = '\0';
    }
    else
    {
        char *s = "ERROR : reading executable path\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        //perror("Error reading executable path\n");
        return -1;
    }

    // Print the required process information
    printf("pid : %d\n", pid);
    printf("process status : %s\n", proc_status);
    printf("Process Group : %d\n", process_group);
    printf("Virtual memory : %d\n", vm_size);
    print_path_relative_to_cwd(exec_path, root);
    return 0;
}