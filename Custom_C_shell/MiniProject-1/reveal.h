#ifndef REVEAL_H
#define REVEAL_H
#include "headers.h"

#define COLOR_BOLD    "\033[1m"
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"  // Green for executables
#define COLOR_WHITE   "\033[37m"  // White for regular files
#define COLOR_BLUE    "\033[34m"  // Blue for directories
#define COLOR_RED   "\033[31m"  // Red
#define COLOR_ORANGE   "\033[33m"  // Orange

void reveal_command(char *command, char* home, char* last_dir);
void reveal_files(const char *path, int show_hidden, int show_details, const char *home, char *last_dir);
int get_total_blocks(char *path);
void print_file_details(const char *path, int show_details);
char *get_absolute_path(const char *path, const char *home);
#endif

