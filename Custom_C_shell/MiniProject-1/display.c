#include "display.h"

char *username = NULL;
char *prompt = NULL;
char sysname[1024];
int time_taken = 0;
int time_str[4096] = {0};
char command_store[MAX_COMMANDS][1024];
int count = 0;

char *get_username()
{
    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL)
    {
        return NULL;
    }
    return pw->pw_name;
}
void setup()
{
    int host_success = gethostname(sysname, 1024);
    if (host_success == -1)
    {
        printf("" COLOR_RED "Error in getting hostname\n" COLOR_RESET);
        exit(1);
    }

    // Get username buffer = 1024
    username = get_username();
    if (username == NULL)
    {
        printf("" COLOR_RED "Error in getting username\n" COLOR_RESET);
        exit(1);
    }
}
char *get_prompt(char *sysname, char *username, char *root, char *cwd, char command_store[MAX_COMMANDS][1024], int time_str[MAX_COMMANDS], int count)
{
    // Allocate enough memory for the prompt string
    char *prompt = (char *)malloc(4096);
    if (prompt == NULL)
    {
       printf("" COLOR_RED "Error in allocating memory for prompt\n" COLOR_RESET);
        exit(1);
    }

    // Initialize the prompt with an empty string
    prompt[0] = '\0';

    // Construct the initial part of the prompt
    if (strncmp(cwd, root, strlen(root)) == 0)
    {
        if (strlen(cwd) == strlen(root))
        {
            snprintf(prompt, 4096, "<%s@%s:~", username, sysname);
        }
        else
        {
            snprintf(prompt, 4096, "<%s@%s:~%s", username, sysname, cwd + strlen(root));
        }
    }
    else
    {
        snprintf(prompt, 4096, "<%s@%s:%s", username, sysname, cwd);
    }

    size_t current_length = strlen(prompt);
    
    // Append commands and times
    for (int i = 0; i < count; i++)
    {
        if (time_str[i] > 2)
        {
            // Check if there is enough space for the next command and its time
            if (current_length + strlen(command_store[i]) + 20 >= 4096) // 20 is for `; cmd : time`
            {
                printf("" COLOR_RED "Prompt buffer overflow while appending command and time\n" COLOR_RESET);
                free(prompt);
                exit(1);
            }

            // Append each command and its time
            if (i > 0 && time_str[i - 1] > 2)
            {
                snprintf(prompt + current_length, 4096 - current_length, "; %s : %ds", command_store[i], time_str[i]);
                
            }
            else
            {
                snprintf(prompt + current_length, 4096 - current_length, " %s : %ds", command_store[i], time_str[i]);
                
            }
            current_length = strlen(prompt);
        }
    }
    // Append closing "> "
    if (current_length + 3 < 4096) // 3 is for `> `
    {
        strcat(prompt, "> ");
    }
    else
    {
        printf("" COLOR_RED "Prompt buffer overflow while appending closing `> `\n" COLOR_RESET);
        free(prompt);
        exit(1);
    }

    return prompt;
}
