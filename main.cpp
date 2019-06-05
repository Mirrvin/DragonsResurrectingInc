#include "main.h"
#include "Monitor.h"
#include "specialists/Specialist.h"
#include "specialists/HeadSpecialist.cpp"

extern bool end;

//extern void inicjuj(int *argc, char ***argv);
void rootLoop(int size){
    packet_t packet;
    packet.id = 0;
    // while(!end){
    //     sleep(rand()%1000+1000);
        for(int i = 1;i < size; i++){
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, 1, TASK, MPI_COMM_WORLD);
            printf("wyslane\n");
        }
        for(int i = 1;i < size; i++){
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, END, MPI_COMM_WORLD);
            printf("wyslane\n");
        }
        // taskId++;
    // }

}

void *handleLoop (void* s ) {
    Specialist *specialist = (Specialist*) s;

    while(!end) {
        pthread_mutex_lock(&Monitor::handleMutex); // czeka, aż kolejka nie będzie pusta

        pthread_mutex_lock(&Monitor::messageQueueMutex); // dostęp do kolejki wiadomości
        packet_t packet = Monitor::messageQueue.front();
        Monitor::messageQueue.pop;
        pthread_mutex_unlock(&Monitor::messageQueueMutex);

        specialist->handle(packet);

        pthread_mutex_lock(&Monitor::messageQueueMutex);
        if(!Monitor::messageQueue.empty) {
            pthread_mutex_unlock(&Monitor::handleMutex); // kolejka nie jest pusta, lecimy dalej
        }
        pthread_mutex_unlock(&Monitor::messageQueueMutex);
    }
    Monitor::endListening();
    // pthread_detach(pthread_self());
    pthread_exit(NULL);
}

void headLoop(){
    printf("headloop %d\n", Monitor::rank);
    // int resurrectionCounter = 0;
    // int bodyId, tailId;

    Specialist *specialist = new HeadSpecialist(Monitor::rank, Monitor::size);
    pthread_t handleThread;
    pthread_create( &handleThread, NULL, &handleLoop, specialist);
    Monitor::listen();
    pthread_join(handleThread,NULL);
    delete specialist;
}

void bodyLoop(){
    printf("bodyloop %d\n", Monitor::rank);
    int resurrectionCounter = 0;
    int headId, tailId;
}
void tailLoop(){
    printf("tailloop %d\n", Monitor::rank);
    int resurrectionCounter = 0;
    int headId, bodyId;
}

int main(int argc, char **argv)
{
    /* Tworzenie wątków, inicjalizacja itp */
    int provided;
    printf("main\n");
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    Monitor::initialize();    

    int type = Monitor::rank%3;
    if (Monitor::rank == ROOT){
        rootLoop(Monitor::size);
    } else if (type == HEAD) {
        headLoop();
    } else if (type == BODY) {
        bodyLoop();
    } else if (type == TAIL) {
        //tailLoop();
    }
    //finalizuj();
    MPI_Finalize();
    return 0;
}