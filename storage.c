#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> //open,creat
#include <sys/types.h> //open
#include <sys/stat.h>
#include <errno.h> //perror, errno
#include <string.h>

void print_line(Line* line){
    if(line->deleted) {printf("-\n"); return;}
    printf("%s, %d, %lli, %d, %s\n",line->name, line->birth, line->phonenumber, line->paying, line->vaccinated ? "OLTVA" : "");
}

int storage_save_header(Storage* storage){
    fseek(storage->file,0,SEEK_SET);
    fwrite(&storage->header,sizeof(struct _storage_header),1,storage->file);
}

int clearLine(Line * line){
    memset(line,'\0',sizeof(Line));
}

int storage_open(char* file_name, Storage* storage) {
    memset(storage,0,sizeof(Storage));

    storage->file = fopen (file_name, "rb+");
    if (storage->file == NULL) {
        return 1;
    }
    fseek(storage->file,0,SEEK_SET);
    size_t size = fread(&storage->header,sizeof(struct _storage_header),1,storage->file);
    if(storage->header.magic == STORAGE_START) {
        return 0;
    } else if(size != 0){
        return 1;
    }
    struct _storage_header header;
    header.magic = STORAGE_START ;
    header.size = 0;

    storage->header = header;
    storage_save_header(storage);
    return 0;
}

int storage_close(Storage* storage){
    fclose(storage->file);
    return 0;
}

int storage_getLine(Storage* storage, size_t id, Line *line){
    if(id >= storage->header.size) return 1;
    fseek(storage->file,sizeof(struct _storage_header)+id*sizeof(Line),SEEK_SET);
    fread(line,sizeof(Line),1,storage->file);
    return 0;
}


int storage_add(Storage* storage, Line* line) {
    fseek(storage->file,0,SEEK_END);
    fwrite(line,sizeof(Line),1,storage->file);
    
    storage->header.size++;
    storage_save_header(storage);
}

int storage_edit(Storage* storage, size_t id, Line* line, int mode) {
    Line templine;
    storage_getLine(storage,id, &templine);
    if(mode & NAME){
        strcpy(templine.name, line->name); 
    }
    if(mode & TEL){
        templine.phonenumber = line->phonenumber;
    }
    if(mode & PAY){
        templine.paying= line->paying;
    }
    if(mode & BIRTH){
        templine.birth = line->birth;
    }
    if(mode & VACC){
        templine.vaccinated = line->vaccinated;
    }
    fseek(storage->file,sizeof(struct _storage_header)+id*sizeof(Line),SEEK_SET);
    fwrite(&templine,sizeof(Line),1,storage->file);
    return 0;
}

int storage_delete(Storage* storage, size_t id) {
    if(id == -1 || id > storage->header.size) return 1;
    fseek(storage->file,sizeof(struct _storage_header)+id*sizeof(Line),SEEK_SET);
    Line line;
    clearLine(&line);
    line.deleted = 1;
    fwrite(&line,sizeof(Line),1,storage->file);
    return 0;
}

int storage_list(Storage* storage){
    fseek(storage->file,sizeof(struct _storage_header),SEEK_SET);
    for (size_t i = 0; i < storage->header.size; i++)
    {
        Line line;
        fread(&line,sizeof(line),1,storage->file);
        printf("%li ", i);
        print_line(&line);
    }
}

int storage_unvaccintedCount(Storage* storage){
    int count = 0;
    for (int i = 0;i < storage->header.size; ++i){
        Line templine;
        storage_getLine(storage, i, &templine);
        if(!templine.deleted && !templine.vaccinated){
            count++;
        }
    }
    return count;
}

int storage_getPeopleVacc(Storage* storage, int offset,  int* ids){
    int count = 0;
    int k = 0;
    for (int i = 0;i < storage->header.size; ++i){
        Line temp;
        storage_getLine(storage, i, &temp);
        if(!temp.deleted && !temp.vaccinated){
            count++;
            if(count > offset){
                ids[k++] = i;
            }
        }
        if(k >= 5){
            return k;
        }
    }
    return k;
}