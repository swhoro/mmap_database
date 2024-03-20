#include <stdio.h>
#include <stdlib.h>

#include "database.h"
#include "tools.h"

#define FILE_NAME "data"

int main() {
    Database *database = init_database(FILE_NAME);

    bool should_exit = false;
    while (!should_exit) {
        printf("command (h for help): ");

        char *input = readline(100);
        switch (input[0]) {
        case 'h':
            // print help information
            print_help();
            break;
        case 'l':
            // list all key value
            do_list(database);
            break;
        case 'c': {
            // create key value
            char key[MAX_KEY_LENGTH] = "";
            char value[MAX_VALUE_LENGTH] = "";
            sscanf(input, "c %s %s", key, value);
            if (!check_key(key) || !check_value(value)) {
                printf("key:%s or value:%s is invalid\n", key, value);
                break;
            }
            do_create(database, key, value);
            break;
        }
        case 'r': {
            // query value with key
            char key[MAX_KEY_LENGTH] = "";
            sscanf(input, "r %s", key);
            if (!check_key(key)) {
                printf("key:%s is invalid\n", key);
                break;
            }
            char *value = do_read(database, key);
            if (value == NULL) {
                printf("key:%s not found\n", key);
                break;
            }
            printf("value: %s\n", value);
            break;
        }
        case 'u': {
            // update key with value
            char key[MAX_KEY_LENGTH] = "";
            char value[MAX_VALUE_LENGTH] = "";
            sscanf(input, "u %s %s", key, value);
            if (!check_key(key) || !check_value(value)) {
                printf("key:%s or value:%s is invalid\n", key, value);
                break;
            }
            do_update(database, key, value);
            break;
        }
        case 'd': {
            // delete key value according to key
            char key[MAX_KEY_LENGTH] = "";
            sscanf(input, "d %s", key);
            if (!check_key(key)) {
                printf("key:%s is invalid\n", key);
                break;
            }
            do_delete(database, key);
            break;
        }
        case 's':
            // save database
            save_database(database);
        case 'q':
            // quit this program
            should_exit = true;
            printf("see you next time\n");
            break;
        }
    }

    save_database(database);
    free(database);
}