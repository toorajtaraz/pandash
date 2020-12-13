#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "../headers/execute.h"
#include <signal.h>
#include <unistd.h>
#include "../headers/commands.h"
#include "../headers/ansi.h"
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <pthread.h>

#define PERMANENT_FIFO_PATH "/tmp/pandashbox_permanent_\0"

pid_t main_proc;
bool run = true;
bool new_line = true;
char* fifo_path;
void print_prompt();

void create_permanent_fifo() {
    fifo_path = malloc(sizeof(char)*150);
    char* path = malloc(sizeof(char)*150);
    int pid = getpid();
    sprintf(path, "%s%d", PERMANENT_FIFO_PATH, pid);
    mkfifo(path, 0666);
    strcpy(fifo_path, path);
    free(path);
}

void* listen() {
    int fd = open(fifo_path, O_RDONLY|O_NONBLOCK);
    if (fd == -1) {
        perror("COMMUNICATION FAILED");
        abort();
    }
    while (true) {
        char buf[125] = {'\0'};
        read(fd, buf, sizeof(char)*124);
        if (buf[0] != '\0') {
            printf("\nSOME OTHER PANDA : %s\n", buf);
            print_prompt();
        }
        fflush(stdout);
        sleep(2);
    }
}

void print_prompt() {
    if (new_line) puts("");
    printf(PROMPT_COLOR);
    printf(PROMPT);
    printf(SIGN);
    printf(reset);
    printf(BRACKET_COLOR);
    printf("[");
    printf(reset);
    char* path = malloc(sizeof(char)*1024);
    get_directory(path);
    printf(PATH_COLOR);
    printf("%s",path);
    printf(reset);
    printf(BRACKET_COLOR);
    printf("]");
    printf(reset); 
    printf(ARROW_COLOR);
    printf(">");
    printf(reset);
    fflush(stdout);
    free(path);
    new_line = true;
}

void handle_intrupt(int sig) {
    if (getpid() != main_proc) {
        exit(0);
    } else {
        print_prompt();
        run = false; 
    } 
}

int handle_reader_cntrl_c(int a, int b) {
    run = false;
    return 0;
}

void handle_exit(int sig) {
    exit(0);
}

bool get_input(char* str) {
    char* buf;
    buf = readline("");
    if (buf == NULL) handle_exit(3);
    if (strlen(buf) != 0) {
        if (strlen(buf) > 1024) {
            fprintf(stderr, "COMMAND IS TOO LONG");
            return false;
        }
        add_history(buf);
        strcpy(str, buf);
        free(buf);
        return true;
    } else {
        return false;
    }
}

void prompt_loop() {
    while (run) {
        print_prompt();
        char command[1024] = {'\0'};
        char err[512] = {'\0'};
        Parsed* parsed_command = malloc(sizeof(Parsed));
        parsed_command->flag_count = 0;
        parsed_command->head = parsed_command;
        parsed_command->is_piped = false;
        bool piped;
        bool built_in;
        if (get_input(command)) {
            piped = is_piped(command);
            built_in = is_built_in(command);
            if (!parse(command, parsed_command, err)) {
                perror(err);
                delete_parsed_command(parsed_command);
                continue;
            } else {
                new_line = false;
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
        }
    }    
}

void pandash() {
    create_permanent_fifo();
    pthread_t  tid;
    pthread_create(&tid, NULL, listen, (void *)NULL);
    main_proc = getpid();
    printf(BWHT);
    printf("\n**ID : %d**\n", main_proc);
    printf(reset);
    signal(SIGINT, handle_intrupt); 
    LOOP:
        prompt_loop();
    if (!run) {
        run = true;
        goto LOOP;
    }
}
