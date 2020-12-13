#include "../headers/commands.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <pthread.h>
#include "../headers/ansi.h"
#define PATH_FIFO "/tmp/pandashbox_"
#define PERMANENT_FIFO_PATH "/tmp/pandashbox_permanent_\0"

int fd;

void help() {
    printf(GRN);
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$**$$$$$$$$$**$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$\"   ^$$$$$$F    *$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$     z$$$$$$L    ^$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$    e$$$$$$$$$e  J$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$eee$$$$$$$$$$$$$e$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$b$$$$$$$$$$$$$$$$$$*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$)$$$$P\"e^$$$F$r*$$$$F\"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$d$$$$  \"z$$$$\"  $$$$%  $3$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$*\"\"\"*$$$  .$$$$$$ z$$$*   ^$e*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$\"     *$$ee$$$$$$$$$$*\"     $$$C$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$.      \"***$$\"*\"$$\"\"        $$$$e*$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$b          \"$b.$$\"          $$$$$b\"$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$c.         \"\"\"            $$$$$$$^$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$e..                     $$$$$$$$^$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$eeee..            J$$$$$$$$b\"$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$r          z$$$$$$$$$$r$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\"         z$$$$$**$$$$$^$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$*\"          z$$$P\"   ^*$$$ $$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$*\"           .d$$$$       $$$ $$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$\"           .e$$$$$F       3$$ $$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$.         .d$$$$$$$         $PJ$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$eeeeeeed$*\"\"\"\"**\"\"         $\\$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$                  $d$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$.                 $$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$e.              d$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$eeeeeee$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ToorajT$$$$");
    printf(reset);
    printf(BLU);
    puts("WELCOME TO PANDASH, A SIMPLE SHELL WRITTEN IN C");
    printf(reset);
    printf(BWHT);
    puts("BUILT IN COMMANDS:");
    printf(reset);
    printf(CYN);
    puts("\thelp:");
    printf(reset);
    printf(BRED);
    puts("\t\tIT WILL PRINT THIS PROMT");
    printf(reset);
    printf(CYN);
    puts("\tcd [path?]:");
    printf(reset);
    printf(BRED);
    puts("\t\tIT WILL CHANGE CWD TO 'PATH' OR $HOME");
    printf(reset);
    printf(CYN);
    puts("\texit:");
    printf(reset);
    printf(BRED);
    puts("\t\tIT WILL TERMINATE THE CURRENT SHELL");
    printf(reset);
    printf(CYN);
    puts("\tpandash_talk [YOUR_ID] [THE_OTHER_SHELL_ID]: (ID can be anything)");
    printf(reset);
    printf(BRED);
    puts("\t\tGOES TO CHAT MODE AND YOU CAN CHAT WITH ONE OTHER PANDASH SHELL");
    printf(reset);
    printf(CYN);
    puts("\tpandash_send [THE_OTHER_SHELL_ID] [YOUR QOUTED MSG]: (ID can be anything)");
    printf(reset);
    printf(BRED);
    puts("\t\tTRYS TO DELIEVER YOUR MSG, YOUR ID IS PRINTED AS SOON AS YOU EXECUTE THIS PROGRAM");
    printf(reset);
}

bool change_directory(char* dir) {
    if (dir == NULL || dir[0] == '\0') {
        char* user = getlogin();
        char temp[513] = {'\0'}; 
        strcat(temp, "/home/");
        strcat(temp, user);
        if (chdir(temp) != 0) {
            return false;
        } 
       return true; 
    }
    if (chdir(dir) != 0) {
        return false;
    } 
    return true;
}

void get_directory(char* path) {
    getcwd(path, 512);
}

void exit_pandash() {
    exit(0);
}

void* handle_read_talk(void* path) {
    fd = open((char*) path, O_RDONLY|O_NONBLOCK);
    if (fd == -1) {
        perror("COMMUNICATION FAILED");
        abort();
    }
    while (true) {
        char buf[125] = {'\0'};
        read(fd, buf, sizeof(char)*124);
        if (buf[0] != '\0')
            printf("\nTHE OTHER PANDA : %s\nYOU: ", buf);
        fflush(stdout);
        sleep(2);
    }
}

void pandash_talk(char* id1, char*id2) {
    puts("WAITING FOR OTHER PANDA...");
    char* fifo_path = malloc(sizeof(char)*124);
    strcpy(fifo_path, PATH_FIFO);
    strcat(fifo_path, id1);
    mkfifo(fifo_path, 0666);
    puts(fifo_path);
    char* path = malloc(sizeof(char)*124);
    strcpy(path, fifo_path);
    pthread_t  tid; 
    pthread_create(&tid, NULL, handle_read_talk, (void *)path); 
    strcpy(fifo_path, PATH_FIFO);
    strcat(fifo_path, id2);
    mkfifo(fifo_path, 0666);
    puts(fifo_path);
    int wfd = open(fifo_path, O_WRONLY);
    if (fd == -1) {
        perror("COMMUNICATION FAILED");
        abort();
    }
    while (true) {
        char buf[125] = {'\0'};
        printf("YOU : ");
        read(0, buf, sizeof(char)*124);
        fflush(stdout);
        write(wfd, buf, strlen(buf)+1);
    } 
}
void pandash_send(char*id, char* msg) {
    char* fifo_path = malloc(sizeof(char)*124);
    strcpy(fifo_path, PERMANENT_FIFO_PATH);
    strcat(fifo_path, id);
    puts(fifo_path);
    int wfd = open(fifo_path, O_WRONLY);
    if (fd == -1) {
        perror("NO PANDA WITH THAT ID");
        abort();
    }
    if (write(wfd, msg, strlen(msg)+1) == -1) {
        perror("COULD NOT DELIEVER");
        abort();
    }
}

