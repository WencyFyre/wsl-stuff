#include "vaccinate.h"

enum Mode{
    NONE,
    ADD,
    EDIT,
    REMOVE,
    LIST,
    VACCINATE,
    ERROR
};



int main(int argc,char** argv){
    //elso beadando kiegeszitve egy egy --vaccinate futassal
    srand(time(NULL));
    int mode = NONE;
    int edit = 0;
    int id = -1;
    Line line;
    clearLine(&line);
    Storage storage;
    if(storage_open("storage.txt",&storage)){
        printf("File Error, not empty, or not a db for this app! Make sure there is a storage.txt file in the folder");
        return 1;
    }
    

    for(int i=1; i< argc; i++) {
        char* act= argv[i];

        if(strcmp(act,"--add") == 0){
            mode = ADD;
        } else if(strcmp(act,"--vaccinate") == 0){
            mode = VACCINATE;
        } else if(strcmp(act,"--remove") == 0){
            id= atoi(argv[++i]);
            mode = REMOVE;
        } else if (strcmp(act,"--edit") == 0) {
            id= atoi(argv[++i]);
            mode = EDIT;
        } else if (strcmp(act,"--list") == 0) {
            mode = LIST;
        } else if(strcmp(act,"--name") == 0){
            strcpy(line.name,argv[++i]);
            edit |= NAME;
        } else if(strcmp(act,"--tel") == 0){
            line.phonenumber = atoll(argv[++i]);
            edit |= TEL;
        } else if(strcmp(act,"--birth") == 0){
            line.birth = atoi(argv[++i]);
            edit |= BIRTH;
        } else if(strcmp(act,"--pay") == 0){
            line.paying = argv[++i][0] == 'y' ? 1 : 0;
            edit |= PAY;
        }else{
            printf("invalid parameter: %s\n",act);
            mode=ERROR;
        }
    };

    switch (mode)
    {
    case NONE:
        printf("Not enough parameters!\n");
        return 0;
        break;
    case ADD:
        storage_add(&storage, &line);
        break;
    case EDIT:
        if(id== -1) {printf("error, invalid index \n"); break;}
        storage_edit(&storage, id, &line, edit);
        break;
    case REMOVE:
        if(id== -1) {printf("error, invalid index \n"); break;}
        if(storage_delete(&storage, id)) printf("error, invalid index \n");
        break;
    case LIST:
        storage_list(&storage);
        break;
    case VACCINATE:
        main_vaccinated(&storage);
        break;
    default:
        break;
  }
    storage_close(&storage);
    return 0;
}