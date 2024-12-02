#include "alias.h"

int alias_count = 0;
int function_count = 0;
Function functions[10]; 
Alias aliases[10];

void add_alias(char *name, char *command) {
    aliases[alias_count].name = strdup(name);
    aliases[alias_count].command = strdup(command);
    alias_count++;
}

// Function to add a function
void add_function(char *name, char *body) {
    functions[function_count].name = strdup(name);
    functions[function_count].body = strdup(body);
    function_count++;
}

char *find_alias(char *name) {
    char* m2 = NULL;
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            m2 = strdup(aliases[i].command);
            return m2;
        }
    }
    return NULL;
}

char* find_function(const char *name) {
    char *m = NULL;
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            char * m = strdup(functions[i].body);
            return m;
        }
    }
    return NULL;  // Function not found
}

void parse_myshrc(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("" COLOR_RED COLOR_BOLD "Error opening %s\n" COLOR_RESET "", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;
        if(strstr(line, "#") != NULL){
            continue;
        }
        // Handle aliases
        if (strncmp(line, "alias", 5) == 0) {
            char name[MAX_NAME_LENGTH];
            char command[MAX_LINE_LENGTH];

            // Extract alias name and command
            sscanf(line + 6, "%s = %[^\n]", name, command); // "alias name=command"
            add_alias(name, command);
        }
        // Handle functions
        else if (strchr(line, '(') && strchr(line, ')')) {
            char name[MAX_NAME_LENGTH];
            char body[MAX_BODY_LENGTH] = "";

            // Extract function name
            sscanf(line, "%[^()]()", name);

            // Read function body (assume it's one line for simplicity)
            while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
                strcat(body, line);
            }

            strcat(body, "}\n");
            // printf("body %s\n", body);
            // printf("name %s\n", name);
            add_function(name, body);
        }
    }

    fclose(file);
}
void tokenize_and_store(char *input, char *arr[]) {
    char *token2;
    int i = 0;
    // printf("input %s\n", input);
    // Tokenize the input string by newline to split the commands
    token2 = strtok(input, "{");
    // printf("token2 %s\n", token2);
    command_trim_2(token2);  // Remove leading/trailing spaces
    // printf("token2 %s\n", token2);
    while (token2 != NULL && i < 1) {
        command_trim_2(token2);  // Remove leading/trailing spaces
        // Tokenize by space to get the command name
        char *command = strtok(token2, " ");
        if (command != NULL) {
            arr[i] = command;
            i++;
        }
        
    }
    token2 = strtok(NULL, " ");
    token2 = strtok(NULL, " ");
    arr[i] = token2;
    //  printf("arr[0] %s\n", arr[0]);
    //     printf("arr[1] %s\n", arr[1]);
}
void command_trim_2(char *str)
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