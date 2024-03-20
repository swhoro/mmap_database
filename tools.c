#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void print_help() {
    printf("l \t\tlist all key value\n");
    printf("c <key> <value>\tcreate a new entry\n");
    printf("r <key>\t\tread the value\n");
    printf("u <key> <value>\tupdate key\n");
    printf("d <key>\t\tdelete an entry\n");
    printf("\n");
    printf("s\t\tsave the database\n");
    printf("q\t\tquit the program\n");
}

char *readline(int max_length) {
    char *buffer = malloc(max_length);

    bool should_exit = false;
    int i = 0;
    while (!should_exit) {
        char c = getchar();
        if (c != '\n') {
            buffer[i++] = c;
        } else {
            buffer[i] = '\0';
            should_exit = true;
        }
    }
    return buffer;
}