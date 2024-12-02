#include "activity.h"

process_info processes[MAX_PROCESSES];
int process_count = 0;

void add_process(pid_t pid, char *command) {
    strcpy(processes[process_count].command, command);
    processes[process_count].pid = pid;
    strcpy(processes[process_count].state, "Running");
    process_count++;
}

void remove_process(pid_t pid) {
    int i, j;

    for (i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            for (j = i; j < process_count - 1; j++) {
                processes[j] = processes[j + 1];
            }
            process_count--;
            return;
        }
    }
    //printf("No process with PID %d found.\n", pid);
}

void update_process_state(pid_t pid, const char *new_state) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            strcpy(processes[i].state, new_state);
            break;
        }
    }
}
int compare_processes(const void *a, const void *b) {
    process_info *p1 = (process_info *)a;
    process_info *p2 = (process_info *)b;
    return strcmp(p1->command, p2->command);
}

void print_activities() {
    // Sort the processes lexicographically by command name
    qsort(processes, process_count, sizeof(process_info), compare_processes);

    // Print each process info in the required format
    for (int i = 0; i < process_count; i++) {
        printf("%d : %s - %s\n", processes[i].pid, processes[i].command, processes[i].state);
    }
}

char* check_pid(int pid){
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            char *s = processes[i].command;
            return s;
        }
    }
    return NULL;
}

void update(int new_pid, int old_pid){
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == old_pid) {
            processes[i].pid = new_pid;
            break;
        }
    }
}

bool if_in_process(int pid){
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            return true;
        }
    }
    return false;
}