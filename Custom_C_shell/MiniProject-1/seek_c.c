#include "seek_c.h"

int no_match_flag = 1;
int arr[2] = {0, 0};
char *found[2] = {NULL, NULL};
char buff[MAX_PATH_LEN];

void color_print(const char *base_dir, const char *path, int is_dir)
{
    char relative_path[MAX_PATH_LEN];

    if (strstr(path, base_dir) == path)
    {
        // The path is inside the base directory, so print it as relative
        snprintf(relative_path, sizeof(relative_path), ".%s", path + strlen(base_dir));
        printf(is_dir ? "\033[1;34m%s\033[0m\n" : "\033[1;32m%s\033[0m\n", relative_path);
        no_match_flag = 0;
    }
    else
    {
        // The path is outside the base directory, print it as absolute
        printf(is_dir ? "\033[1;34m%s\033[0m\n" : "\033[1;32m%s\033[0m\n", path);
        no_match_flag = 0;
    }
}

int has_permission(const char *path, int is_dir)
{
    if (is_dir)
    {
        return access(path, X_OK) == 0;
    }
    else
    {
        return access(path, R_OK) == 0;
    }
}
void seek_directory(const char *dir, const char *target, int d_flag, int f_flag, int e_flag, const char *start_dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char path[MAX_PATH_LEN];
    int file_count = 0, dir_count = 0;
    char found_file[MAX_PATH_LEN], found_dir[MAX_PATH_LEN];

    // Get the current working directory as the base directory
    char base_dir[MAX_PATH_LEN];
    // getcwd(base_dir, sizeof(base_dir));
    strcpy(base_dir, start_dir);
    // printf("base dir %s\n", base_dir);
    if ((dp = opendir(dir)) == NULL)
    {
        no_match_flag = 0;
        char *s = "ERROR : opening directory\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        //perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
        lstat(path, &statbuf);

        if (S_ISDIR(statbuf.st_mode))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            // Check if the directory name starts with the target
            if (strncmp(entry->d_name, target, strlen(target)) == 0 && !f_flag)
            {
                strcpy(found_dir, path);
                dir_count++;
                if (dir_count > 0 && !f_flag)
                {
                    color_print(base_dir, found_dir, 1);
                    arr[1]++;
                    if (arr[1] == 1)
                    {
                        found[1] = found_dir;
                        strncpy(buff, found[1], sizeof(buff));
                    }
                }
            }

            seek_directory(path, target, d_flag, f_flag, e_flag, start_dir);
        }
        else if (strncmp(entry->d_name, target, strlen(target)) == 0 && !d_flag)
        {
            // Check if the file name starts with the target
            strcpy(found_file, path);
            file_count++;
            if (file_count > 0 && !d_flag)
            {
                color_print(base_dir, found_file, 0);
                arr[0]++;
                if (arr[0] == 1)
                    found[0] = found_file;
            }
        }
    }
    closedir(dp);

    //     if (file_count + dir_count >= 1 )
    //     {
    //         if (file_count > 0 && !d_flag)
    //         {
    //             color_print(base_dir, found_file, 0);
    //             arr[0]++;
    //             if(arr[0] == 1)
    //             found[0] = found_file;
    //         }
    //         if (dir_count > 0 && !f_flag)
    //         {
    //             color_print(base_dir, found_dir, 1);
    //             arr[1]++;
    //             if(arr[1] == 1){
    //             found[1] = found_dir;
    //             strncpy(buff, found[1], sizeof(buff));
    //             }
    //         }

    //    }
}

void check_if_match_found(int no_match_flag)
{
    if (no_match_flag)
        printf("" COLOR_RED COLOR_BOLD "No match found\n" COLOR_RESET "");
}

void check_e_flag(int file_count, int dir_count, char *found_file, char *found_dir, int d_flag, int f_flag)
{
    if (file_count == 1 && (dir_count == 0 || f_flag))
    {
        handle_file(found_file);
    }
    else if (dir_count == 1 && (file_count == 0 || d_flag))
    {
        if (has_permission(found_dir, 1))
        {
            chdir(found_dir);
        }
        else
        {
            printf("" COLOR_RED COLOR_BOLD "Missing permissions for task!\n" COLOR_RESET "");
        }
    }
}
void handle_file(char *path)
{
    struct stat file_stat;

    // Get file status
    if (stat(path, &file_stat) == -1)
    {
        char *s = "ERROR : getting file status\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        //perror("Error getting file status");
        return;
    }

    // Check if the file is marked as executable (for user, group, or others)
    int is_executable = (file_stat.st_mode & S_IXUSR) ||
                        (file_stat.st_mode & S_IXGRP) ||
                        (file_stat.st_mode & S_IXOTH);

    // Check if the file is executable
    if (access(path, X_OK) == 0)
    {
        // Execute the file
        char *args[] = {path, NULL};
        int p = fork();
        if (p == 0)
        {
            // Child process
            // printf("Executing file : %s\n", path);
            execvp(path, args);
            // If execl fails
            char *s = "ERROR : error executing file\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            //perror("Error executing file");
            exit(1);
        }
        else if (p < 0)
        {
            char *s = "ERROR : error forking process\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            //perror("Error forking process");
        }
        else
        {
            // Parent process
            wait(NULL);
        }
    }
    else if (access(path, R_OK) == 0 && !is_executable)
    {
        // The file is not executable but is readable
        // printf("Reading file : %s\n", path);
        FILE *file = fopen(path, "r");
        if (!file)
        {
            char *s = "ERROR : opening file\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            //perror("Error opening file");
            return;
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            printf("%s", buffer);
        }

        fclose(file);
    }
    else
    {
        printf("" COLOR_RED COLOR_BOLD "Missing permissions for task!\n" COLOR_RESET "");
    }
}

char *get_path(const char *path, const char *home)
{
    char resolved_path[1024]; // Temporary path storage
    char absolute_path[1024]; // Absolute path storage

    // If the path starts with '~', replace it with the home directory
    if (strncmp(path, "~/", 2) == 0)
    {
        snprintf(resolved_path, sizeof(resolved_path), "%s%s", home, path + 1);
    }
    else if (strcmp(path, "~") == 0)
    {
        snprintf(resolved_path, sizeof(resolved_path), "%s", home);
    }
    else
    {
        snprintf(resolved_path, sizeof(resolved_path), "%s", path);
    }
    realpath(resolved_path, absolute_path); // Resolve the path
    // printf("resolved path : %s\n", resolved_path);
    // printf("absolute path : %s\n", absolute_path);
    // Return a dynamically allocated copy of the absolute path
    return strdup(absolute_path);
}