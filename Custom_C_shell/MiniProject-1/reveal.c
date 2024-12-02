#include "reveal.h"

void reveal_command(char *str, char *home, char *last_dir)
{

    int show_hidden = 0;
    int show_details = 0;
    char *path = "~";

    char *token;
    char command_copy[1024];
    strncpy(command_copy, str, sizeof(command_copy));
    command_copy[sizeof(command_copy) - 1] = '\0';

    // Tokenize the command to check for flags and path
    token = strtok(command_copy, " ");
    while (token != NULL)
    {
        if (strcmp(token, "reveal") == 0)
        {
            // Skip the "reveal" part
        }
        else if (strchr(token, '-') == token)
        {
            // Process flags
            if (strchr(token, 'a'))
            {
                show_hidden = 1;
            }
            if (strchr(token, 'l'))
            {
                show_details = 1;
            }
            if (token[strlen(token) - 1] == '-' || strchr(token, ' ') == token)
            {
                // Assume it's a path
                if (last_dir != NULL)
                {
                    path = last_dir;
                }
                else
                {
                    char *s = "OLDPWD not set\n";
                    printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
                    return;
                }
            }
        }
        else
        {
            // Assume it's a path
            path = token;
        }
        token = strtok(NULL, " ");
    }

    // Call the function to reveal files based on flags
    reveal_files(path, show_hidden, show_details, home, last_dir);
}

void reveal_files(const char *path, int show_hidden, int show_details, const char *home, char *last_dir)
{
    struct dirent **namelist;
    struct stat file_stat;
    int n;
    char resolved_path[1024];

    // Resolve the path if it starts with '~'
    if (strncmp(path, "-", 1) == 0)
    {
        // printf("last dir %s\n", last_dir);
        if (last_dir != NULL)
            strcpy(resolved_path, last_dir);
        else
        {
            char *s = "OLDPWD not set\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);

            return;
        }
    }
    else if (strncmp(path, "~/", 2) == 0)
    {
        path = path + 1;
        strcpy(resolved_path, home);
        strcat(resolved_path, path);
    }
    else if (strcmp(path, "~") == 0)
    {
        snprintf(resolved_path, sizeof(resolved_path), "%s", home);
    }
    else
    {
        snprintf(resolved_path, sizeof(resolved_path), "%s", path);
    }

    char *new_path = get_absolute_path(resolved_path, home);
    // Call stat to get file information
    if (stat(new_path, &file_stat) == -1)
    {
        // Handle error (e.g., file not found, permission denied)
        char *s = "ERROR : \n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        // perror("Error");
        return;
    }
    if (S_ISDIR(file_stat.st_mode))
    {

        n = scandir(resolved_path, &namelist, NULL, alphasort);
        if (n < 0)
        {
            char *s = "ERROR : scandir - No such file or directory\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            //perror("Error - scandir");
            return;
        }
        printf("total %d\n", get_total_blocks(resolved_path));
        for (int i = 0; i < n; i++)
        {
            struct dirent *entry = namelist[i];
            if (!show_hidden && entry->d_name[0] == '.')
            {
                free(entry);
                continue; // Skip hidden files if not showing hidden
            }

            if (show_details)
            {
                // Display detailed information
                struct stat file_stat;
                char fullpath[2048];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", resolved_path, entry->d_name);

                if (stat(fullpath, &file_stat) == -1)
                {
                    char *s = "ERROR : getting stat\n";
                    printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
                    //perror("Error - stat");
                    free(entry);
                    continue;
                }
                // File type and permissions
                printf("%c", S_ISDIR(file_stat.st_mode) ? 'd' : '-');
                printf("%c", (file_stat.st_mode & S_IRUSR) ? 'r' : '-');
                printf("%c", (file_stat.st_mode & S_IWUSR) ? 'w' : '-');
                printf("%c", (file_stat.st_mode & S_IXUSR) ? 'x' : '-');
                printf("%c", (file_stat.st_mode & S_IRGRP) ? 'r' : '-');
                printf("%c", (file_stat.st_mode & S_IWGRP) ? 'w' : '-');
                printf("%c", (file_stat.st_mode & S_IXGRP) ? 'x' : '-');
                printf("%c", (file_stat.st_mode & S_IROTH) ? 'r' : '-');
                printf("%c", (file_stat.st_mode & S_IWOTH) ? 'w' : '-');
                printf("%c", (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

                // Number of links
                printf(" %ld", (long)file_stat.st_nlink);

                // Owner and group
                struct passwd *pw = getpwuid(file_stat.st_uid);
                struct group *gr = getgrgid(file_stat.st_gid);
                printf(" %s %s", pw->pw_name, gr->gr_name);

                // Size
                printf(" %5ld", (long)file_stat.st_size);

                // Last modification time
                char timebuf[80];
                strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&file_stat.st_mtime));
                printf(" %s", timebuf);
            }

            // Determine color based on file type
            struct stat file_stat;
            char fullpath[2048];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", resolved_path, entry->d_name);
            stat(fullpath, &file_stat);

            if (S_ISDIR(file_stat.st_mode))
            {
                printf(COLOR_BOLD COLOR_BLUE); // Directory
            }
            else if (file_stat.st_mode & S_IXUSR)
            {
                printf(COLOR_BOLD COLOR_GREEN); // Executable
            }
            else
            {
                printf(COLOR_WHITE); // Regular file
            }

            // File name
            printf(" %s\n", entry->d_name);
            printf(COLOR_RESET);
            free(entry);
        }
        free(namelist);
    }
    else if (S_ISREG(file_stat.st_mode))
    {
        print_file_details(resolved_path, show_details);
    }
    else
    {
        char *s = "Invalid path\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
    }
}
void print_file_details(const char *path, int show_details)
{
    struct stat file_stat;

    // Get file status
    if (stat(path, &file_stat) == -1)
    {
        char *s = "ERROR : stat\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        //perror("Error - stat");
        return;
    }

    if (show_details)
    {

        // File type and permissions
        printf("%c", S_ISDIR(file_stat.st_mode) ? 'd' : '-');
        printf("%c", (file_stat.st_mode & S_IRUSR) ? 'r' : '-');
        printf("%c", (file_stat.st_mode & S_IWUSR) ? 'w' : '-');
        printf("%c", (file_stat.st_mode & S_IXUSR) ? 'x' : '-');
        printf("%c", (file_stat.st_mode & S_IRGRP) ? 'r' : '-');
        printf("%c", (file_stat.st_mode & S_IWGRP) ? 'w' : '-');
        printf("%c", (file_stat.st_mode & S_IXGRP) ? 'x' : '-');
        printf("%c", (file_stat.st_mode & S_IROTH) ? 'r' : '-');
        printf("%c", (file_stat.st_mode & S_IWOTH) ? 'w' : '-');
        printf("%c", (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

        // Number of links
        printf(" %ld", (long)file_stat.st_nlink);

        // Owner and group
        struct passwd *pw = getpwuid(file_stat.st_uid);
        struct group *gr = getgrgid(file_stat.st_gid);
        printf(" %s %s", pw->pw_name, gr->gr_name);

        // Size
        printf(" %5ld", (long)file_stat.st_size);

        // Last modification time
        char timebuf[80];
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&file_stat.st_mtime));
        printf(" %s", timebuf);
    }
    // Determine color based on file type
    if (file_stat.st_mode & S_IXUSR) // If the file is executable by the user
    {
        printf(" " COLOR_GREEN COLOR_BOLD "%s" COLOR_RESET "\n", path);
    }
    else // Regular file
    {
        printf(" " COLOR_WHITE "%s" COLOR_RESET "\n", path);
    }
}

int get_total_blocks(char *path)
{
    FILE *fp;
    char buffer[1024];
    int total_blocks = -1;

    // Build the command
    snprintf(buffer, sizeof(buffer), "ls -l %s", path);

    // Execute the command
    fp = popen(buffer, "r");
    if (fp == NULL)
    {
        char *s = "ERROR : popen failed\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        //perror("popen failed");
        return -1;
    }

    // Read the first line of output
    if (fgets(buffer, sizeof(buffer) - 1, fp) != NULL)
    {
        // Extract the total number of blocks
        if (sscanf(buffer, "total %d", &total_blocks) != 1)
        {
            total_blocks = -1; // Set to -1 if parsing failed
        }
    }

    // Close the command stream
    pclose(fp);

    return total_blocks;
}

char *get_absolute_path(const char *path, const char *home)
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