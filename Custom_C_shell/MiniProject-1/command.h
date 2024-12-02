#include "headers.h"

#define COLOR_BOLD "\033[1m"
#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[32m"  // Green for executables
#define COLOR_WHITE "\033[37m"  // White for regular files
#define COLOR_BLUE "\033[34m"   // Blue for directories
#define COLOR_RED "\033[31m"    // Red
#define COLOR_ORANGE "\033[33m" // Orange

extern char *last_dir;
extern char resolved_path[1024];
extern char *token;
extern char *tokens[1024];
extern int token_count;
typedef struct
{
    pid_t pid;
    char command[1024];
    int job_id;
} Job;

extern Job jobs[1024];
extern Job fg_jobs[1024];
extern int fg_job_count;
extern int job_count;
extern int var1;
extern int var2;
extern bool status_bg;
extern int foreground_flag;
extern int ctrlc;
extern char * root2;
extern int univeral;
extern int do_pid;
extern int do_pid2;
extern int univeral2;

char **tokenize_string(char *str, char *original_str);
void execution(char **tokens, char *root, int token_count);
int exec_comm_fg(char *command, char *root);
void exec_comm_bg(char *command, int background, int original_stdout, int fd_out);
int hop_command(char *path, char *home, char **last_dir);
void command_trim(char *str);
void append_command_time(const char *command, int time_taken);
void handle_sigchld(int sig);
void capitalize_first_letter(char *str);
void fg_run(char *command, char *root);
void io_direction_fg(char *cmd, char *root);
void io_direction_bg(char *cmd);
void exec_pipe_fg(char *cmd, char *root);
void exec_pipe_bg(char *cmd, char *root);
int check_pipe_validity(char *input);
void handle_sigint(int sig);
void kill_all_jobs();
void handle_exit() ;
void handle_sigtstp(int sig);
