#include "io.h"

void io_direction_fg(char *cmd, char *root)
{
    char command_io[1024] = "";
    char *input_file = NULL;
    char *output_file = NULL;
    
    int append = 0; // 0 for '>', 1 for '>>'
    int i = 0;

    // Save the original stdin and stdout file descriptors
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);

    // Process the command string to handle redirection
    while (cmd[i] != '\0')
    {
        if (cmd[i] == '<')
        {
            // Handle input redirection
            cmd[i] = '\0'; // Terminate the command before '<'
            i++;
            while (cmd[i] == ' ')
                i++;              // Skip any spaces
            input_file = &cmd[i]; // Set input file
            while (cmd[i] != ' ' && cmd[i] != '\0' && cmd[i] != '>')
                i++; // Find the end of the filename
            if (cmd[i] != '\0' && cmd[i] != '>')
            {
                cmd[i] = '\0';
                i++;
            }
        }
        else if (cmd[i] == '>')
        {
            if (cmd[i + 1] == '>')
            {
                // Handle append output redirection
                append = 1;
                cmd[i] = '\0'; // Terminate the command before '>>'
                i += 2;        // Skip '>>'
            }
            else
            {
                // Handle overwrite output redirection
                cmd[i] = '\0'; // Terminate the command before '>'
                i++;
            }
            while (cmd[i] == ' ')
                i++;               // Skip any spaces
            output_file = &cmd[i]; // Set output file
            while (cmd[i] != ' ' && cmd[i] != '\0' && cmd[i] != '<')
                i++; // Find the end of the filename
            if (cmd[i] != '\0' && cmd[i] != '<')
            {
                cmd[i] = '\0'; // Terminate the filename
                i++;
            }
        }
        else
        {
            i++;
        }
    }
    strncpy(command_io, cmd, sizeof(command_io) - 1);
    command_io[sizeof(command_io) - 1] = '\0'; // Ensure null-termination
    // printf("Command: %s--\n", command_io);

    // Handle Input Redirection
    if (input_file)
    {
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in < 0)
        {
            printf("" COLOR_RED "No such input file found!\n" COLOR_RESET "");
            return;
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }

    // Handle Output Redirection
    if (output_file)
    {
        int fd_out;
        if (append)
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (fd_out < 0)
        {
            printf("" COLOR_RED "Error opening output file\n" COLOR_RESET "");
            return;
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }

    // Execute the command with the modified input/output
    fg_run(command_io, root);

    // Restore original stdin and stdout
    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);

    // Close the duplicated file descriptors
    close(original_stdin);
    close(original_stdout);
}
