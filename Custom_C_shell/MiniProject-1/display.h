#ifndef DISPLAY_H
#define DISPLAY_H
#include "headers.h"
#define MAX_COMMANDS 100

// Get hostname buffer = 1024
extern char sysname[1024];
extern char *username;

extern int time_taken;
extern char *prompt;
extern int time_str[4096];
extern char command_store[MAX_COMMANDS][1024];
extern int count ;

char *get_username();
void setup();

char *get_prompt(char *sysname, char *username, char *root, char *cwd, char command_store[MAX_COMMANDS][1024], int time_str[MAX_COMMANDS], int count);
#endif