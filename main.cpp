#include "main.h"
#include "Monitor.h"
#include "specialists/Specialist.h"
#include "specialists/HeadSpecialist.cpp"

bool check_thread_support(int provided) {
    return provided >= MPI_THREAD_FUNNELED;
}

void rootLoop(int size){
    packet_t packet;
    packet.id = 0;
    // while(!end){
    //     sleep(rand()%1000+1000);
        for(int i = 1;i < size; i++){
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, 1, TASK, MPI_COMM_WORLD);
            printf("Issuer with rank %d sending data %d to rank %d\n", 0, packet.id, 1);
        }
        for(int i = 1;i < size; i++){
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, END, MPI_COMM_WORLD);
            printf("Issuer with rank %d sending data %d to rank %d\n", 0, packet.id, i);
        }
        // taskId++;
    // }

}

void *handleLoop (void* s ) {
    Specialist *specialist = (Specialist*) s;
    bool end = false;
    while(!end) {
        pthread_mutex_lock(&Monitor::handleMutex); // czeka, aż kolejka nie będzie pusta

        pthread_mutex_lock(&Monitor::messageQueueMutex); // dostęp do kolejki wiadomości
        packet_t packet = Monitor::messageQueue.front();
        Monitor::messageQueue.pop();
        pthread_mutex_unlock(&Monitor::messageQueueMutex);
        printf("Message to handle by %d: { source: %d, tag: %d }\n",Monitor::rank, packet.status.MPI_SOURCE, packet.status.MPI_TAG);
        if(!specialist->handle(packet)) {
            end = true;
            printf("Specialist with rank %d is stopping\n", Monitor::rank);
        }

        pthread_mutex_lock(&Monitor::messageQueueMutex);
        if(!Monitor::messageQueue.empty()) {
            pthread_mutex_unlock(&Monitor::handleMutex); // kolejka nie jest pusta, lecimy dalej
        }
        pthread_mutex_unlock(&Monitor::messageQueueMutex);
    }
    Monitor::endListening();
    pthread_exit(NULL);
}

void headLoop(){
    printf("headloop %d\n", Monitor::rank);

    Specialist *specialist = new HeadSpecialist(Monitor::rank, Monitor::size);
    pthread_t handleThread;
    pthread_create( &handleThread, NULL, &handleLoop, specialist);
    Monitor::listen();
    pthread_join(handleThread,NULL);
    delete specialist;
}

void bodyLoop(){
    printf("bodyloop %d\n", Monitor::rank);
    // int resurrectionCounter = 0;
    // int headId, tailId;
}
void tailLoop(){
    printf("tailloop %d\n", Monitor::rank);
    // int resurrectionCounter = 0;
    // int headId, bodyId;
}

int main(int argc, char **argv)
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if(check_thread_support(provided)) {
        Monitor::initialize();

        int type = Monitor::rank%3 + 1;
        if (Monitor::rank == ISSUER){
            rootLoop(Monitor::size);
        } else if (type == HEAD) {
            headLoop();
        } else if (type == BODY) {
            bodyLoop();
        } else if (type == TAIL) {
            tailLoop();
        }
    } else {
        printf("Threads are not supported. Exiting application");
    }
    MPI_Finalize();
    return 0;
}