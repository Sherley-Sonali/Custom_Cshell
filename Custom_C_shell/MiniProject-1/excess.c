#include "excess.h"

void capitalize_first_letter(char *str)
{
    if (str == NULL || str[0] == '\0')
    {
        // If the string is empty or null, do nothing
        return;
    }

    // Capitalize the first character if it's a letter
    str[0] = toupper(str[0]);
}
void command_trim(char *str)
{
    if (str == NULL)
        return;

    size_t len = strlen(str);
    size_t i = 0, j = 0;

    while (i < len && isspace((unsigned char)str[i]))
    {
        i++;
    }

    if (i == len)
    {
        str[0] = ' ';
        str[1] = '\0';
        return;
    }

    while (i < len)
    {
        if (isspace((unsigned char)str[i]))
        {
            if (j > 0 && str[j - 1] != ' ')
            {
                str[j++] = ' ';
            }
        }
        else
        {
            str[j++] = str[i];
        }
        i++;
    }

    str[j] = '\0';
    size_t end = j;
    while (end > 0 && isspace((unsigned char)str[end - 1]))
    {
        end--;
    }
    str[end] = '\0';
}

int check_pipe_validity(char *input)
{
    // Find the position of the pipe character '|'
    const char *pipe_pos = strchr(input, '|');

    // If no pipe is found, it's invalid
    if (pipe_pos == NULL)
    {
        printf("" COLOR_RED "Invalid\n" COLOR_RESET "");
        return -1;
    }

    // Extract the parts before and after the pipe
    int before_pipe_len = pipe_pos - input;
    int after_pipe_len = strlen(pipe_pos + 1); // +1 to skip the '|'

    // Check if both sides have non-empty strings
    if (before_pipe_len > 0 && after_pipe_len > 0)
    {
        // if(input[before_pipe_len - 1] == ' ' || input[before_pipe_len + 1] == ' ') {
        //     printf("Invalid use of pipe\n");
        //     return -1;
        // }
    }
    else
    {
        printf("" COLOR_RED "Invalid use of pipe\n" COLOR_RESET "");
        return -1;
    }
    return 1;
}
