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
#include "../headers/commands.h"

const char* built_in[] = {"cd", "exit", "pandash_talk", "help"};

bool is_piped(char* command) {
    if (strstr(command, "|")) {
        return true;
    }
    return false;
}

bool is_built_in(char* command) {
    int len = sizeof(built_in) / sizeof(built_in[0]); 
    for (int i = 0; i < len; i++) {
        if (strstr(command, built_in[i])) {
            return true;
        }
    }
    return false;
}

void delete_parsed_command(Parsed* parsed_command) {
    parsed_command = parsed_command->head;
    Parsed* next;
    if (parsed_command->is_piped) {
        while (parsed_command->command[0] != '\0')
        {
            next = parsed_command->next_command;
            free(parsed_command->command);
            if(parsed_command->flag_count != 0) {
                for (int i = 0;; i++) {
                    free(parsed_command->flags[i]);
                    if (!parsed_command->flags[i]) break;
                }
            }
            free(parsed_command);
            parsed_command = next;
        }
        free(parsed_command->command);
        if(parsed_command->flag_count != 0) {
            for (int i = 0;; i++) {
                free(parsed_command->flags[i]);
                if (!parsed_command->flags[i]) break;
            }
        }
        free(parsed_command);
    }
    else {
        free(parsed_command->command);
        if(parsed_command->flag_count != 0) {
            for (int i = 0;; i++) {
                free(parsed_command->flags[i]);
                if (!parsed_command->flags[i]) break;
            }
        }
        free(parsed_command);
    }
}

bool parse_flags(char* token, Parsed* parsed_command, int start) {
    int j = 0;
    int k = 0;
    int len = strlen(token);
    parsed_command->flags = malloc(sizeof(char*)*25);
    parsed_command->flags[j] = malloc(sizeof(char)*124);
    strcpy(parsed_command->flags[j++], parsed_command->command); 
    parsed_command->flags[j] = malloc(sizeof(char)*512);
    parsed_command->flags[j][0] = '\0';
    while(token[start] == ' ') start++;
    for(; start < len; start++) {
        if(token[start] != ' ') {
            if (token[start] == '\'') {
                start++;
                while (token[start] != '\'') {
                    parsed_command->flags[j][k++] = token[start];
                    start++;
                    if (start >= len) {
                        return false;
                    }
                }
            } else if (token[start] == '\"') {
                start++;
                while (token[start] != '\"') {
                    parsed_command->flags[j][k++] = token[start];
                    start++;
                    if (start >= len) {
                        return false;
                    }
                }
            } else 
                parsed_command->flags[j][k++] = token[start];
            if (start + 1 == len) {
                parsed_command->flags[j][k] = '\0';
                j++;
                parsed_command->flags[j] = malloc(sizeof(char)*512);
                parsed_command->flags[j][0] = '\0';
                parsed_command->flag_count = j;
                break;          
            }
        } else {
            parsed_command->flags[j][k] = '\0';
            while(token[start] == ' ' && start < len) start++;
            start--;
            j++;
            k = 0;
            parsed_command->flags[j] = malloc(sizeof(char)*512);
            parsed_command->flags[j][0] = '\0';
            parsed_command->flag_count = j;
        }
    }
    return true; 
}

bool make_pipe_commands(char* token, Parsed* parsed_command) {
    parsed_command->command = (char*) malloc(sizeof(char)*100);
    int i = 0;
    int j = 0;
    while(token[i] == ' ') i++;
    for(; i < strlen(token); i++) {
        if(token[i] != ' ') {
            parsed_command->command[j++] = token[i];
        } else {
            parsed_command->command[j] = '\0';
            break;
        }
    }
    if (i == strlen(token)) {
        parsed_command->command[j] = '\0';
    }
    return parse_flags(token, parsed_command, i);
}

bool parse(char* command, Parsed* parsed_command, char* err) {
    char* cmd = malloc(sizeof(char)*1024);
    strcpy(cmd, command);
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
    } else {
        if (!make_pipe_commands(command, parsed_command)) {
            parsed_command -> is_piped = false;
            strcpy(err, "syntax error");
            return false;
        }
        if (parsed_command->flag_count != 0)
            parsed_command->flags[parsed_command->flag_count] = NULL;
        else
            parsed_command->flags[1] = NULL;
    }
    return true;
}
