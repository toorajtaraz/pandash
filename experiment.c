#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

void set_raw_mode(struct termios *orig_termios) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig_termios);

    raw = *orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void reset_terminal_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, orig_termios);
}

int is_printable(char c) {
    return (c >= 32 && c <= 126);
}

void write_str(const char *str) {
    write(STDOUT_FILENO, str, strlen(str));
}


void write_str_cursor(const char *str, int cursor) {

    const char *clean_scrn = "\033[2J\033[H";
    const char *set_colors = "\033[30;47m";
    const char *reset_colors = "\033[0m";
    if (cursor > 0) {
        write(STDOUT_FILENO, clean_scrn, strlen(clean_scrn));
        write(STDOUT_FILENO, str, cursor - 1);
        write(STDOUT_FILENO, set_colors, strlen(set_colors));
        write(STDOUT_FILENO, str + cursor - 1, 1);
        write(STDOUT_FILENO, reset_colors, strlen(reset_colors));
        write(STDOUT_FILENO, str + cursor, strlen(str) - cursor);
    } else {
        write(STDOUT_FILENO, clean_scrn, strlen(clean_scrn));
        write(STDOUT_FILENO, set_colors, strlen(set_colors));
        write(STDOUT_FILENO, str, 1);
        write(STDOUT_FILENO, reset_colors, strlen(reset_colors));
        write(STDOUT_FILENO, str + 1, strlen(str) - 1);
    }
}

void write_char(char c) {
    write(STDOUT_FILENO, &c, 1);
}

void write_str_ln(const char *str) {
    write_str(str);
    write_str("\r\n");
}

int main() {
    struct termios orig_termios;
    char c;
    char buffer[1024] = {'\0'};
    int editor_pointer = 0;
    int max_index_written_to = 0;
    const char *hide_cursor = "\033[?25l";
    const char *show_cursor = "\033[?25h";
    set_raw_mode(&orig_termios);

    write(STDOUT_FILENO, hide_cursor, strlen(hide_cursor));
    write_str_ln("Press any key. Press 'q' to quit.");

    while (1) {
        // printf("%d\n\r", editor_pointer);
        if (read(STDIN_FILENO, &c, 1) < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (c == '\x1b') {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) < 0 || read(STDIN_FILENO, &seq[1], 1) < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A':
                        // write_str_ln("Up arrow key pressed");
                        break;
                    case 'B':
                        // write_str_ln("Down arrow key pressed");
                        break;
                    case 'C':
                        if (editor_pointer < max_index_written_to) {
                            editor_pointer++; 
                        }
                        write_str_cursor(buffer, editor_pointer + 1);
                        // write_str_ln("Right arrow key pressed");
                        break;
                    case 'D':
                        if (editor_pointer > 0) {
                            editor_pointer--;
                        }
                        write_str_cursor(buffer, editor_pointer + 1);
                        // write_str_ln("Left arrow key pressed");
                        break;
                    default:
                        // write_str("Unknown escape sequence: \\x1b[");
                        // write_char(seq[0]);
                        // write_char(seq[1]);
                        // write_str_ln("");
                        break;
                }
            } else {
                // write_str("Unknown escape sequence: \\x1b");
                // write_char(seq[0]);
                // write_char(seq[1]);
                // write_str_ln("");
            }
        } else if (c == 'q') {
            write_str_ln("Quitting...");
            break;
        } else {
            if (is_printable(c)) {
                if (max_index_written_to == 1024) {
                    perror("Exceeded the buffer!"); 
                    exit(-1);
                }
                if (editor_pointer != max_index_written_to) {
                    for (int i = max_index_written_to; i >= editor_pointer; i--) {
                        buffer[i+1] = buffer[i];
                    } 
                }
                buffer[editor_pointer] = c;
                // editor_pointer=max_index_written_to;

                if (editor_pointer == max_index_written_to - 1) {
                    max_index_written_to = ++editor_pointer;
                    buffer[max_index_written_to++] = ' ';
                } else {
                    ++editor_pointer;
                    ++max_index_written_to;
                }
                buffer[max_index_written_to] = '\0';
                write_str_cursor(buffer, editor_pointer);
                // write_str("Key pressed: ");
                // write_char(c);
                // write_str(" (ASCII: ");
                // char ascii_str[4];
                // snprintf(ascii_str, sizeof(ascii_str), "%d", c);
                // write_str(ascii_str);
                // write_str_ln(")");
            } else {
                // write_str("Non-printable key pressed: ASCII ");
                // char ascii_str[4];
                // snprintf(ascii_str, sizeof(ascii_str), "%d", c);
                // write_str(ascii_str);
                // write_str_ln("");
            }
        }
    }

    reset_terminal_mode(&orig_termios);
    write(STDOUT_FILENO, show_cursor, strlen(show_cursor));
    return 0;
}

