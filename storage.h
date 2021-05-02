#ifndef STORATE_H
#define STORATE_H

#include <stdio.h>

typedef struct {
    int deleted;
    char name[64];
    int birth;
    long long phonenumber;
    int paying;
    int vaccinated;
}Line;

#define STORAGE_START (('Y') | ('E' << 8) | ('P' << 16) | ('P' << 24)) //magic
struct _storage_header {
    int magic;
    int size;
};

typedef struct _storage {
    FILE* file;
    struct _storage_header header;

} Storage;

enum EditAttr {
    NAME = 1,
    TEL  = 1 << 1,
    BIRTH = 1 << 2,
    PAY = 1 << 3,
    VACC = 1 << 4
};

void print_line(Line* line);
int storage_save_header(Storage* storage);
int clearLine(Line * line);
int storage_open(char* file_name, Storage* storage);
int storage_close(Storage* storage);
int storage_getLine(Storage* storage, size_t id, Line *line);
int storage_add(Storage* storage, Line* line);
int storage_edit(Storage* storage, size_t id, Line* line, int mode);
int storage_delete(Storage* storage, size_t id);
int storage_list(Storage* storage);
int storage_unvaccintedCount(Storage* storage);
int storage_getPeopleVacc(Storage* storage, int offset, int * ids);

#endif