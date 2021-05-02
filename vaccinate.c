#include "vaccinate.h"


void handler(int signumber, siginfo_t *info, void *nonused){
    //printf("%i - %i \n",getpid(),getppid());
    int bus = info->si_value.sival_int;
    printf("Bus %i: HARCRA_FEL!\n",bus + 1);
    printf("A következő ID-k kapnak SMS-t: %i",ids[bus][0]);
    for (int i = 1; i < 5; i++)
    {
      printf(", %i",ids[bus][i]);
    }
    printf("\n busnak irás előtt vagyunk: %i\n", bus);
    int reust = write(buses[bus].input_pipe,ids[bus],sizeof(ids[bus]));
    printf(" busnak irás után vagyunk, ezt írtuk: %i\n",reust );
}


Bus create_and_send_bus(int (*chid_proc) (int,int,int), int bus_id) {
    //parameterek sorban:
    //az alprocess main-je
    //busnak az ID-ja
    Bus bus;
    bus.exist = 1;
    int pipein[2];
    int pipeout[2];

    if (pipe(pipeout) == -1 ||
        pipe(pipein) == -1) {

        perror("Pipe Error!");
        exit(1);
    }

    bus.pid = fork();
    if (bus.pid == -1) {
        perror("Fork error");
        exit(1);
    }

    if (bus.pid == 0) { //Bus
        close(pipein[1]);   //chillba érkező adat
        close(pipeout[0]);
        //printf("Child(%i) with: %i, %i\n",getpid(),pipein[0],pipeout[1]);
        int result = chid_proc(pipein[0],pipeout[1],bus_id); 
        close(pipein[0]);
        close(pipeout[1]);
        exit(result);
    } else { // Main
        close(pipein[0]);
        close(pipeout[1]);
        //printf("Main(%i) with: %i, %i\n",bus.pid,pipein[1],pipeout[0]);
        bus.input_pipe = pipein[1];
        bus.output_pipe = pipeout[0];
        return bus;
    }
}

int bus_main(int pipe_in,int pipe_out, int bus_id){
    //busz alprocess mainje
    
    union sigval s_value_int = {(int)(bus_id)};
    sigqueue(getppid(), HARCRA_FEL, s_value_int);
    int buffer[5];
    //printf("innen olvas: %i, ide ír: %i\n", pipe_in, pipe_out);
    printf("tőrzsből olvasás előtt, %i\n", bus_id);
    read(pipe_in, buffer, sizeof(buffer));
    printf("tőrzsből olvasás után, %i\n", bus_id);
    printf("A következők megérkeztek az oltópontra:");
    for (int i = 0; i < 5; i++)
    {
        if(rand()%101>90){
            buffer[i]=-1;
        }else{
            printf(" %i ", buffer[i]);
        }
    }
    printf("\n");
    write(pipe_out,buffer,sizeof(buffer));
}


int main_vaccinated(Storage* storage){
    buses[0].exist = 0;
    buses[1].exist = 0;

    //2. beadando logokia része
    struct sigaction sigact;
    sigact.sa_sigaction = handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigaction(HARCRA_FEL, &sigact, NULL);

    for(int i=0; i<2; i++){
        if( (storage_getPeopleVacc(storage, i*5, ids[i])) >4){
            buses[i] = create_and_send_bus(bus_main,i);
            //sleep(2);
        };
    }
    sleep(2);
    Line line;
    line.vaccinated=1;
    for(int i=0; i<2; i++){
        if(buses[i].exist){
            int buffer[5];
            //printf("Start read\n");
            read(buses[i].output_pipe, buffer, sizeof(buffer));
            //printf("End read\n");
            for (int i = 0; i < 5; i++)
            {
                if(buffer[i]!=-1){
                    storage_edit(storage,buffer[i],&line,VACC);
                }
            }
        }
    }
    
    for(int i=0; i<2; i++) if(buses[i].exist) {
        waitpid(buses[i].pid, NULL, 0);
        close(buses[i].input_pipe);
        close(buses[i].output_pipe);
        };
}
