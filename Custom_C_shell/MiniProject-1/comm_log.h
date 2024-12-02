#include "headers.h"

#define MAX_LOG_SIZE 15
#define LOG_FILE_PATH "/home/sherleysonalip/Desktop/MON - 24/OSN/MiniProject-1/comlog.txt"

extern char c_log[15][1024];
extern int log_count;

void load_log(char c_log[MAX_LOG_SIZE][1024], int *log_count);
void execute_command(char *command, char*home);
void log_execute_command(char c_log[MAX_LOG_SIZE][1024], int *log_count, int index, char*home);
void save_log(char c_log[MAX_LOG_SIZE][1024], int log_count);
void add_command_to_log(char c_log[MAX_LOG_SIZE][1024], int *log_count, const char *command);
void display_log(char c_log[MAX_LOG_SIZE][1024], int log_count);
char **tokenize_string_2(char *str);