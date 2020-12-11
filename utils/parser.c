/**
  * accteptable syntax:
  *     [command] [flags]? ([|] [command] [flags]?)*
  * flags:
  *     ([-] [flag] [flagparams]*)*
 **/
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/parser.h"

bool parse_flags(char* token, Parsed* parsed_command, int start) {
    int j = 0;
    int k = 0;
    parsed_command->flags = malloc(sizeof(char*)*25);
    parsed_command->flags[j++] = parsed_command->command; 
    parsed_command->flags[j] = malloc(sizeof(char)*100);
    parsed_command->flags[j][0] = '\0';
    while(token[start] == ' ') start++;
    for(; start < strlen(token); start++) {
        if(token[start] != ' ') {
            if (token[start] == '\'') {
                start++;
                while (token[start] != '\'') {
                    parsed_command->flags[j][k++] = token[start];
                    start++;
                    if (start > strlen(token)) {
                        return false;
                    }
                }
            } else 
                parsed_command->flags[j][k++] = token[start];
        } else {
            while(token[start] == ' ') start++;
            start--;
            j++;
            k = 0;
            parsed_command->flags[j] = malloc(sizeof(char)*100);
            parsed_command->flags[j][0] = '\0';
            parsed_command->flag_count = j;
        }
    }
    return true; 
}

bool make_pipe_commands(char* token, Parsed* parsed_command) {
    parsed_command->command = (char*) malloc(sizeof(char)*100);
    bool flag = false;
    int i = 0;
    int j = 0;
    while(token[i] == ' ') i++;
    for(; i < strlen(token); i++) {

        if(!flag && token[i] != ' ') {
            parsed_command->command[j++] = token[i];
        } else {
            break;
        }
    }
    return parse_flags(token, parsed_command, i);
}

bool parse(char* command, Parsed* parsed_command, char* err) {
    char* cmd = strdup(command);
    if(strstr(command, "|")) {
        /*handling piped commands*/
        parsed_command -> is_piped = true;
        int count_pipe = 0;
        count_pipe(count_pipe, cmd);
        count_pipe++;
        int count_tokens = 0;
        char* tokens = strtok(command, "|");
        while (tokens != NULL) {
            count_tokens++;
            if (!make_pipe_commands(tokens, parsed_command)) {
                strcpy(err, "syntax error");
                return false;
            }
            if (parsed_command->flag_count != 0)
                parsed_command->flags[parsed_command->flag_count] = NULL;
            else
                parsed_command->flags[1] = NULL;
            Parsed *next = malloc(sizeof(Parsed));
            next->flag_count = 0;
            next->command = (char*) malloc(sizeof(char));
            next->command[0] = '\0';
            next->head = parsed_command->head;
            parsed_command->next_command = next;
            parsed_command = next;
            tokens = strtok(NULL, "|");
        }
        if (count_tokens != count_pipe) {
            strcpy(err, "syntax error, no command after pipe\0");
            return false;
        }
    }
    return true;
}
