#include "headers.h"

#define MAX_LINE_LENGTH 1024
#define MAX_NAME_LENGTH 1024
#define MAX_BODY_LENGTH 4096

typedef struct Alias {
    char *name;
    char *command;
} Alias;

// Structure to hold functions
typedef struct Function {
    char *name;
    char *body;
} Function;

extern Alias aliases[10];  
extern Function functions[10];  
extern int alias_count ;
extern int function_count;

void add_alias(char *name, char *command);
void add_function(char *name, char *body);
char *find_alias(char *name);
char* find_function(const char *name);
void parse_myshrc(const char *filename);
void tokenize_and_store(char *input, char *arr[]) ;
void command_trim_2(char* str);