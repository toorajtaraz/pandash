#include <stdio.h>
#include <unistd.h>
#include "../headers/execute.h"
#include<sys/wait.h> 
#include <string.h>
#include "../headers/commands.h"
#include <stdlib.h>

void handle_built_in(Parsed* parsed_command) {
   if (strstr(parsed_command->command, "help")) {
       help();
       return;
   }
   if (strstr(parsed_command->command, "cd")) {
       if (!change_directory(parsed_command->flag_count == 0 ? NULL : parsed_command->flags[1])) {
            fprintf(stderr, "COULD NOT CHANGE DIRECTORY\n");
       }
       return;
   }
   if (strstr(parsed_command->command, "exit")) {
       exit_pandash(); 
       return;
   } 
   if (strstr(parsed_command->command, "pandash_talk")) {
        if (parsed_command->flag_count <= 2) {
            fprintf(stderr, "YOU NEED TO PROVIDE 2 VALUES, YOUR ID AND THE OTHER SHELL\n");
            return;
        }
        pid_t pid = fork();
        if (pid == -1) {
            return;
        } else if (pid == 0) {
            pandash_talk(parsed_command->flags[1], parsed_command->flags[2]);
            abort();
        } else {
            wait(NULL);
        }
       return;
   }  
   if (strstr(parsed_command->command, "pandash_send")) {
        if (parsed_command->flag_count <= 2) {
            fprintf(stderr, "YOU NEED TO PROVIDE 2 VALUES, THE OTHER SHELL'S ID AND YOUR QOUTED MESSAGE\n");
            return;
        }
        pid_t pid = fork();
        if (pid == -1) {
            return;
        } else if (pid == 0) {
            pandash_send(parsed_command->flags[1], parsed_command->flags[2]);
            abort();
        } else {
            wait(NULL);
        }
       return;
   }
}

int spawn_proc(Parsed *parsed_command) {
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        execvp(parsed_command->flags[0], parsed_command->flags);
        perror("could not execute");
        abort();
    } else {
        wait(NULL);
    }
    return pid;
}

int spawn_proc_fork(int in, int out, Parsed* parsed_command) {/*here the parent should not wait otherwise the program will hang*/
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if(pid == 0) {
        if(in != 0) {
            dup2 (in, 0);
            close (in);
        }
        if(out != 1) {
            dup2 (out, 1);
            close (out);
        }
        execvp (parsed_command->flags[0], parsed_command->flags);
        perror("could not execute");
        abort();
    }
    return pid;
}

int fork_pipes(Parsed* parsed_command) {
    int i = 0;
    pid_t pid;
    int in, fd [2];
    in = 0;
    for (;parsed_command->command[0] != '\0'; i++) {
        if ((parsed_command->next_command)->command[0] == '\0') {
            break;
        }
        pipe(fd);
        spawn_proc_fork(in, fd [1], parsed_command);
        close(fd[1]);
        in = fd[0];
        parsed_command = parsed_command->next_command;
    }
    if (in != 0)
        dup2 (in, 0);
    execvp (parsed_command->flags[0], parsed_command->flags);
    perror("could not execute");
    abort();
}

int pipes(Parsed* parsed_command) {
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        return fork_pipes(parsed_command);
    } else {
        wait(NULL);
    }
    return pid;
}
