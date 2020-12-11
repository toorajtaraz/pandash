#define HEADER_FILE
#include <stdbool.h>

#define count_pipe(i, cmd) for (i=0; cmd[i]; cmd[i]=='|' ? i++ : *cmd++)

typedef struct Parsed{
    struct Parsed* head;
    char* command;
    char** flags;
    int flag_count;
    bool is_piped;
    struct Parsed* next_command;
} Parsed;

bool parse_flags(char* token, Parsed* parsed_command, int start);
bool make_pipe_commands(char* token, Parsed* parsed_command);
bool parse(char* command, Parsed* parsed_command, char* err);
