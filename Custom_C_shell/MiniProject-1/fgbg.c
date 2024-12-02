#include "fgbg.h"

void bring_to_foreground(pid_t pid,char* fg_for_exec, char *root) {
    // Send SIGCONT to the stopped process to resume it
    // if (kill(pid, SIGCONT) == -1) {
    //     perror("Failed to send SIGCONT");
    //     exit(EXIT_FAILURE);
    // }
    // // Wait for the process to finish in the foreground
    // int status;
    // if (waitpid(pid, &status, WUNTRACED) == -1) {
    //     perror("Failed to wait for process");
    //     exit(EXIT_FAILURE);
    // }

    // if (WIFEXITED(status)) {
    //     printf("Process %d finished with exit status %d\n", pid, WEXITSTATUS(status));
    // } else if (WIFSIGNALED(status)) {
    //     printf("Process %d was killed by signal %d\n", pid, WTERMSIG(status));
    // } else if (WIFSTOPPED(status)) {
    //     printf("Process %d was stopped by signal %d\n", pid, WSTOPSIG(status));
    // }
    
}