#include "headers.h"

#define MAX_PROCESSES 100

// Define a structure to store process info
typedef struct {
    pid_t pid;
    char command[256];
    char state[10]; // "Running" or "Stopped"
} process_info;

extern process_info processes[MAX_PROCESSES];
extern int process_count;

void add_process(pid_t pid, char *command);
void update_process_state(pid_t pid, const char *new_state);
int compare_processes(const void *a, const void *b);
void print_activities();
void remove_process(pid_t pid);
char* check_pid(int pid);
bool if_in_process(int pid);
void update(int new_pid, int old_pid);