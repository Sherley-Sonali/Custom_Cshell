#ifndef PROC_H
#define PROC_H
#include "headers.h"

#define MAX_PATH 1024

int proc_comm(int pid, char * root);
void print_path_relative_to_cwd(char *path, char *cwd);

#endif