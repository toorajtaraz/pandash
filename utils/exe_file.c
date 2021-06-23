#include "../headers/exe_file.h"
#include "../headers/commands.h"
#include "../headers/execute.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool get_input_from_file(char *str, FILE *fp) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  if ((read = getline(&line, &len, fp)) != -1) {
    if (len > 1024) {
      perror("COMMAND IS TOO LONG");
      abort();
      return false;
    }
    for (int i = 0; i < len; i++) {
      if (line[i] == '\n') {
        line[i] = '\0';
        break;
      }
    }
    strcpy(str, line);
  }
  if (line)
    free(line);
  if (read <= 0) {
    return false;
  }
  return true;
}

void process_file(char *file) {
  FILE *fp;
  fp = fopen(file, "r");
  if (fp == NULL) {
    perror("COULD NOT OPEN THE FILE");
    abort();
  }
  while (true) {
    char command[1024] = {'\0'};
    char err[512] = {'\0'};
    Parsed *parsed_command = malloc(sizeof(Parsed));
    parsed_command->flag_count = 0;
    parsed_command->head = parsed_command;
    parsed_command->is_piped = false;
    bool piped;
    bool built_in;
    if (get_input_from_file(command, fp)) {
      piped = is_piped(command);
      built_in = is_built_in(command);
      if (!parse(command, parsed_command, err)) {
        perror(err);
        delete_parsed_command(parsed_command);
        continue;
      }
      if (built_in) {
        parsed_command = parsed_command->head;
        handle_built_in(parsed_command);
        delete_parsed_command(parsed_command);
      } else {
        if (piped) {
          parsed_command = parsed_command->head;
          pipes(parsed_command);
          delete_parsed_command(parsed_command);
        } else {
          parsed_command = parsed_command->head;
          spawn_proc(parsed_command);
          delete_parsed_command(parsed_command);
        }
      }
    } else {
      fclose(fp);
      break;
    }
  }
}
