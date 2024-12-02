#ifndef SEEK_H
#define SEEK_H  
#include "headers.h"
#define MAX_PATH_LEN 1024


extern int no_match_flag ;
extern int arr[2];
extern char* found[2];
extern char buff[MAX_PATH_LEN];
void color_print(const char *base_dir, const char *path, int is_dir);
int has_permission(const char *path, int is_dir);
void seek_directory(const char *dir, const char *target, int d_flag, int f_flag, int e_flag,const char *start_dir);
void check_if_match_found(int no_match_flag);
void check_e_flag(int file_count, int dir_count, char *found_file, char *found_dir, int d_flag, int f_flag);
void handle_file(char *path);
char *get_path(const char *path, const char *home);

#endif