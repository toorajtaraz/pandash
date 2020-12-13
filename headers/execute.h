#define HEADER_FILE
#include "./parser.h"

int pipes(Parsed* parsed_command);
int spawn_proc(Parsed* parsed_command);
void handle_built_in(Parsed* parsed_command);
