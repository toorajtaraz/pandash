#define HEADER_FILE
#define clear() printf("\033[H\033[J")
#include <stdbool.h>
bool change_directory(char *dir);
void get_directory(char *path);
void exit_pandash();
void pandash_talk(char *id1, char *id2);
void pandash_send(char *id, char *msg);
void help();
