#include "../headers/commands.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool change_directory(char* dir) {
    if (chdir(dir) != 0) {
        return false;
    } 
    return true;
}

void get_directory(char* path) {
    getcwd(path, 1024);
}

void exit_pandash() {
    exit(0);
}
