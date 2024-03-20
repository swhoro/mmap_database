#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_KEY_LENGTH 20
#define MAX_VALUE_LENGTH 100

typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} Data;

typedef enum { DELETED = 1, DATA, NOT_CRAETED } EntryStatus;

typedef struct {
    EntryStatus status;
    union {
        uint64_t next_add_offset; // used by status == DELETED
        Data real_data;           // used by status == DATA
    } data;
} Entry;

#define ENTRY_SIZE sizeof(Entry)
#define FILE_SIZE (ENTRY_SIZE * 100)

typedef struct {
    uint64_t *count;
    uint64_t *next_add_offset;
    Entry *entry_start;
} Database;

bool check_key(char *key);
bool check_value(char *value);

Database *init_database(char *filename);
void do_create(Database *database, char *key, char *value);
char *do_read(Database *database, char *key);
void do_update(Database *database, char *key, char *value);
void do_delete(Database *database, char *key);
void do_list(Database *database);
void save_database(Database *database);

#endif