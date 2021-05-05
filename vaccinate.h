#ifndef VACCINATE_H
#define VACCINATE_H

#include "storage.h"

#define HARCRA_FEL SIGUSR1
#include <unistd.h> // for pipe()
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct _bus{
    int exist;
    pid_t pid;
    int output_pipe;
    int input_pipe;
    int test_pipe;
}Bus;

Bus buses[2];
int ids[2][6];

void handler(int signumber, siginfo_t *info, void *nonused);
Bus create_and_send_bus(int (*chid_proc) (int,int,int), int bus_id);
int bus_main(int pipe_in,int pipe_out, int bus_id);
int main_vaccinated(Storage* storage);


#endif