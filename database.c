#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "database.h"

Database *init_database(char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat buf;
    fstat(fd, &buf);
    if (FILE_SIZE > buf.st_size) {
        int result = posix_fallocate(fd, 0, FILE_SIZE);
        if (result != 0) {
            perror("posix_fallocate");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    void *t = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    Database *database = malloc(sizeof(Database));
    memset(database, 0, sizeof(Database));
    // if database is not created, create it
    if (strcmp(t, "CREATED") != 0) {
        strcpy(t, "CREATED");
        *(uint64_t *)(t + 8) = 0;                  // count
        *(uint64_t *)(t + 16) = 0;                 // next_add_offset
        ((Entry *)(t + 24))->status = NOT_CRAETED; // first_entry
    }
    database->count = (uint64_t *)(t + 8);
    database->next_add_offset = (uint64_t *)(t + 16);
    database->entry_start = (Entry *)(t + 24);

    return database;
}

bool check_key(char *key) {
    if (strlen(key) == 0 || strlen(key) > MAX_KEY_LENGTH) {
        return false;
    }
    return true;
}

bool check_value(char *value) {
    if (strlen(value) == 0 || strlen(value) > MAX_VALUE_LENGTH) {
        return false;
    }
    return true;
}

Entry *find_entry(Database *database, char *key) {
    Entry *entry = database->entry_start;
    while (entry->status != NOT_CRAETED) {
        if (entry->status == DELETED) {
            goto next;
        }
        if (strcmp(entry->data.real_data.key, key) == 0) {
            return entry;
        }
    next:
        entry += 1;
    }
    return NULL;
}

void do_create(Database *database, char *key, char *value) {
    Entry *add_entry = database->entry_start + *database->next_add_offset;
    switch (add_entry->status) {
    case DELETED: {
        *database->next_add_offset = add_entry->data.next_add_offset;
        break;
    }
    case NOT_CRAETED: {
        *database->next_add_offset += 1;
        (add_entry + 1)->status = NOT_CRAETED;
        break;
    }
    default:
        break;
    }
    add_entry->status = DATA;
    strcpy(add_entry->data.real_data.key, key);
    strcpy(add_entry->data.real_data.value, value);
    *database->count += 1;
}

char *do_read(Database *database, char *key) {
    Entry *entry = find_entry(database, key);
    if (entry == NULL) {
        return NULL;
    }
    return entry->data.real_data.value;
}

void do_update(Database *database, char *key, char *value) {
    Entry *entry = find_entry(database, key);
    if (entry == NULL) {
        printf("key %s not found\n", key);
        return;
    }
    strcpy(entry->data.real_data.value, value);
    printf("update key:%s sunccess", key);
}

void do_delete(Database *database, char *key) {
    Entry *entry = find_entry(database, key);
    if (entry == NULL) {
        printf("key %s not found\n", key);
        return;
    }
    entry->status = DELETED;
    entry->data.next_add_offset = *database->next_add_offset;
    *database->next_add_offset = entry - database->entry_start;
    *database->count -= 1;
}

void do_list(Database *database) {
    Entry *entry = database->entry_start;
    while (entry->status != NOT_CRAETED) {
        if (entry->status == DELETED) {
            goto next;
        }
        printf("key:%s value:%s\n", entry->data.real_data.key,
               entry->data.real_data.value);
    next:
        entry += 1;
    }
}

void save_database(Database *database) {
    msync((void *)(database->count) - 8, FILE_SIZE, MS_SYNC);
}
