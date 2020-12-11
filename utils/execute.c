#include <unistd.h>
#include "../headers/execute.h"
#include<sys/wait.h> 

int spawn_proc(Parsed *parsed_command) {
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        execvp(parsed_command->flags[0], parsed_command->flags);
    }
    return pid;
}

int spawn_proc_fork(int in, int out, Parsed* parsed_command) {
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
        return execvp (parsed_command->flags[0], parsed_command->flags);
    } else
        wait(NULL);
    return pid;
}

int fork_pipes(Parsed* parsed_command) {
    int i;
    pid_t pid;
    int in, fd [2];
    in = 0;
    for (;parsed_command->command[0] != '\0';) {
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
    return execvp (parsed_command->flags[0], parsed_command->flags);
}

int pipes(Parsed* parsed_command) {
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        fork_pipes(parsed_command);
    } else {
        wait(NULL);
    }
    return pid;
}
