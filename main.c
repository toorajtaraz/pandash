#include "headers/exe_file.h"
#include "headers/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **args) {
  switch (argc) {
  case 1: {
    pandash();
    break;
  }
  case 2: {
    if (fork() == 0) {
      process_file(args[1]);
      abort();
    } else {
      wait(NULL);
      break;
    }
  }
  default: {
    fprintf(stderr, "TOO MANY ARGS\n");
    exit(EXIT_FAILURE);
  }
  }
}
