#include "main.h"
#include "Monitor.h"
#include "specialists/Specialist.h"
#include "specialists/HeadSpecialist.cpp"

Specialist *specialist = new HeadSpecialist();
bool inTeam = false;
bool end = false;

int rank;

//extern void inicjuj(int *argc, char ***argv);
void rootLoop(int size){
    packet_t packet;
    packet.id = 0;
    // while(!end){
    //     sleep(rand()%1000+1000);
        // for(int i = 1;i < size; i++)
            // MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, TASK, MPI_COMM_WORLD);
    //     taskId++;
    // }
}

void *runMonitorListen(void* m) {
    Monitor *monitor = (Monitor*) m;
    monitor->listen();
}

void headLoop(int rank, int size){
    printf("grr0");
    int resurrectionCounter = 0;
    int bodyId, tailId;

    // pthread_mutex_t lamportMutex = PTHREAD_MUTEX_INITIALIZER;
    Specialist *specialist = new HeadSpecialist();
    // Monitor *monitor = new Monitor(specialist, &lamportMutex);
    // specialist->setMonitor(monitor);
    // pthread_t threadMonitor;
    // printf("grr1");
    // pthread_create( &threadMonitor, NULL, runMonitorListen, monitor);
    // printf("grr2");
    // pthread_join(threadMonitor, NULL);
    // monitor->listen();
    delete specialist;
    // pthread_mutex_destroy(&lamportMutex);
}

void bodyLoop(){
    int resurrectionCounter = 0;
    int headId, tailId;
}
void tailLoop(){
    int resurrectionCounter = 0;
    int headId, bodyId;
}
void handleMain(packet_t packet, MPI_Status status) {
    specialist->handle(packet, status);
}
int main(int argc, char **argv)
{
    /* Tworzenie wątków, inicjalizacja itp */
    int size;
    int provided;
    printf("omg");
    // MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int type = rand() % 3 + 1;
    if (rank == ROOT){
        rootLoop(size);
    } else //if (type == HEAD) {
        headLoop(rank, size);
    // } else if (type == BODY) {
    //     bodyLoop();
    // } else if (type == TAIL) {
    //     tailLoop();
    // }
    //finalizuj();
    MPI_Finalize();
    return 0;
}