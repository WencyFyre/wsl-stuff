#include "vaccinate.h"
#include <fcntl.h>
int test;
void handler(int signumber, siginfo_t *info, void *nonused){
    //printf("%i - %i \n",getpid(),getppid());
    int bus = info->si_value.sival_int;
    printf("Bus %i: HARCRA_FEL!\n",bus );
    fflush(stdout);
    printf("A következő ID-k kapnak SMS-t: %i",ids[bus][0]);
    for (int i = 1; i < 5; i++)
    {
      printf(", %i",ids[bus][i]);
    }
    printf("\n");
    //printf("\nbusnak irás előtt vagyunk: %i\n", buses[bus].input_pipe);
    int reust = write(buses[bus].input_pipe,ids[bus],sizeof(ids[bus]));
    //printf("\nbusnak irás után vagyunk, ezt írtuk: %i\n",buses[bus].input_pipe);
}


Bus create_and_send_bus(int (*chid_proc) (int,int,int), int bus_id) {
    //parameterek sorban:
    //az alprocess main-je
    //busnak az ID-ja
    Bus bus;
    bus.exist = 1;
    int pipein[2];
    int pipeout[2];


    if (pipe(pipein) == -1 ||
        pipe(pipeout) == -1) {

        perror("Pipe Error!");
        exit(1);
    }
    bus.input_pipe = pipein[1];
    bus.output_pipe = pipeout[0];
    buses[bus_id] = bus;

    //fcntl(pipeout[1], F_SETFL, O_NONBLOCK);
    //printf("ez kétszer futle?\n");
    //printf("Bus pipe: IN[%i - %i] OUT[%i - %i], index: %i %i\n", pipein[0], pipein[1],pipeout[0],pipeout[1], bus_id, getpid());
    bus.pid = fork();
    if (bus.pid == -1) {
        perror("Fork error");
        exit(1);
    }

    if (bus.pid == 0) { //Bus
        close(pipein[1]);   //chillba érkező adat
        close(pipeout[0]);  //chillból kimenő adat
        //printf("Child(%i) with: %i, %i\n",getpid(),pipein[0],pipeout[1]);
        int result = chid_proc(pipein[0],pipeout[1],bus_id); 
        close(pipein[0]);
        close(pipeout[1]);
        exit(result);
        //printf("KILEPTÉLTEKÖCSÖGsdasdsadasd123 1231231231313231233131312312\n");
    } else { // Main
        close(pipein[0]);
        close(pipeout[1]);
        //printf("Main(%i) with: %i, %i\n",bus.pid,pipein[1],pipeout[0]);

        //printf("parent: IN: %i - OUT %i\n",  bus.input_pipe , bus.output_pipe );
        return bus;
    }
}

int bus_main(int pipe_in,int pipe_out, int bus_id){
    //busz alprocess mainje
    //printf("child: IN: %i - OUT %i\n",pipe_in, pipe_out );
    union sigval s_value_int = {(int)(bus_id)};
    if (bus_id)
    {
        sigqueue(getppid(), HARCRA_FEL, s_value_int);
    }
    else{
        sigqueue(getppid(), SIGUSR2, s_value_int);
    }
    
   
    int buffer[5];
    //printf("innen olvas: %i, ide ír: %i\n", pipe_in, pipe_out);
    //printf("tőrzsből olvasás előtt, %i\n", pipe_in);
    read(pipe_in, buffer, sizeof(buffer));
    //printf("tőrzsből olvasás után, %i\n", pipe_in);
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
    return 0;
}


int main_vaccinated(Storage* storage){
    buses[0].exist = 0;
    buses[1].exist = 0;
    //setbuf(stdout, NULL);
    //setvbuf(stdout, NULL, _IONBF, 0); 
    //2. beadando logokia része
    struct sigaction sigact;
    sigact.sa_sigaction = handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigaction(HARCRA_FEL, &sigact, NULL);

    struct sigaction sigact2;
    sigact2.sa_sigaction = handler;
    sigemptyset(&sigact2.sa_mask);
    sigact2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sigact2, NULL);

    for(int i=0; i<2; i++){
        if( (storage_getPeopleVacc(storage, i*5, ids[i])) >4){
            buses[i]=create_and_send_bus(bus_main,i);
        };
    }
    for(int i=0; i<2; i++) if(buses[i].exist) {
        waitpid(buses[i].pid, NULL, 0);
        };

    Line line;
    line.vaccinated=1;
    for(int i=0; i<2; i++){
        if(buses[i].exist){
            int buffer[5];
            //printf("Start read\n");
            read(buses[i].output_pipe, buffer, sizeof(buffer));
           // printf("End read\n");        
            close(buses[i].input_pipe);
            close(buses[i].output_pipe);
            for (int i = 0; i < 5; i++)
            {
                if(buffer[i]!=-1){
                    storage_edit(storage,buffer[i],&line,VACC);
                }
            }
        }
    }
    
}
