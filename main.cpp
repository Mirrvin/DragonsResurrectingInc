#include "main.h"
#include "Monitor.h"
#include "specialists/Specialist.h"
#include "specialists/HeadSpecialist.cpp"
#include "specialists/BodySpecialist.cpp"
#include "specialists/TailSpecialist.cpp"

bool check_thread_support(int provided) {
    return provided >= MPI_THREAD_MULTIPLE;
}

packet_t sendMessage(int tag, int target) {
    packet_t packet;
    packet.lamport = Monitor::incrementLamportOnSend();
    MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, target, tag, MPI_COMM_WORLD);
    return packet;
}

void *handleResurrectionFinished(void* x) {
    int t = rand()%2+1;
    sleep(t);
    pthread_mutex_lock(&Monitor::skeletonsMutex);
    Monitor::currentSkeletons+=1;
    pthread_mutex_unlock(&Monitor::skeletonsMutex);
    pthread_mutex_unlock(&Monitor::newOrderMutex);
    printf("%u: Dragon died after %ds\n",Monitor::getLamport(),t);
    pthread_exit(NULL);
}

void *issuerMonitor (void* x) {
    packet_t packet;
    pthread_t handleResurrection;
    bool end = false;
    while(!end) {
        packet = Monitor::receiveMessage();
        if(packet.status.MPI_TAG == RESURRECTION_FINISHED)
            pthread_create( &handleResurrection, NULL, &handleResurrectionFinished,NULL);
        else {
            end = true;
            printf("%u: issuer ends listining\n",Monitor::getLamport());
        }
    }
    pthread_exit(NULL);
}

void *handleLoop (void* s ) {
    Specialist *specialist = (Specialist*) s;
    bool end = false;
    int count = 0;
    while(!end) {
        // printf("%u: handleLoop BEG, count: %d, rank: %d\n",Monitor::getLamport(),count,Monitor::rank);
        pthread_mutex_lock(&Monitor::handleMutex); // czeka, aż kolejka nie będzie pusta

        pthread_mutex_lock(&Monitor::messageQueueMutex); // dostęp do kolejki wiadomości
        if(!Monitor::messageQueue.empty()) {
            packet_t packet = Monitor::messageQueue.front();
            // printf("%u: handleLoop FRONT, count: %d, rank: %d, tag: %d\n",Monitor::getLamport(),count,Monitor::rank,packet.status.MPI_TAG);
            Monitor::messageQueue.pop();
            pthread_mutex_unlock(&Monitor::messageQueueMutex);
            if(!specialist->handle(packet)) {
                end = true;
            }
        } else {
            pthread_mutex_unlock(&Monitor::messageQueueMutex);
        }

        pthread_mutex_lock(&Monitor::messageQueueMutex);
        if(!Monitor::messageQueue.empty()) {
            pthread_mutex_unlock(&Monitor::handleMutex); // kolejka nie jest pusta, lecimy dalej
        }
        pthread_mutex_unlock(&Monitor::messageQueueMutex);
        // printf("%u: handleLoop END, count: %d, rank: %d\n",Monitor::getLamport(),count,Monitor::rank);
        count++;
    }
    Monitor::endListening();
    pthread_exit(NULL);
}

void rootLoop(int size){
    printf("Heads: 3, 6, 9...\nBodies: 1, 4, 7...\nTails: 2, 5, 8...\nSKELETON_NUMBER %d\nDESK_NUMBER %d\nORDER_NUMBER %d\n",SKELETON_NUMBER,DESK_NUMBER,ORDER_NUMBER);
    pthread_t issuerMonitorThread;
    pthread_create( &issuerMonitorThread, NULL, &issuerMonitor,NULL);

    packet_t packet;
    packet.data = 0;
    int orderId = 1;
    sleep(12);

    // while(orderId <= ORDER_NUMBER) { 
    while(true) { 
        pthread_mutex_lock(&Monitor::newOrderMutex);

        pthread_mutex_lock(&Monitor::skeletonsMutex);
        Monitor::currentSkeletons-=1;
        pthread_mutex_unlock(&Monitor::skeletonsMutex);
        
        if(Monitor::currentSkeletons > 0)
            pthread_mutex_unlock(&Monitor::newOrderMutex);
        
        // sleep(2);
        packet.lamport = Monitor::getLamport();
        printf("%u: New order no. %d!\n",Monitor::getLamport() ,orderId);
        for(int i = 1;i < size; i++){ // broadcast NEW_ORDER
            if(i%3 + 1 == TAIL) {
                sendMessage(NEW_ORDER,i);
            }
        }
        orderId++;
    }

    // sleep(3);
    // for(int i = 1;i < size; i++){ // broadcast END
    //     MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, END, MPI_COMM_WORLD);
    // }
    // sendMessage(END, ISSUER);
}

void headLoop(){
    // printf("headloop %d\n", Monitor::rank);
    Specialist *specialist = new HeadSpecialist(Monitor::rank, Monitor::size);
    pthread_t handleThread;
    pthread_create( &handleThread, NULL, &handleLoop, specialist);
    Monitor::listen();
    pthread_join(handleThread,NULL);
    delete specialist;
}

void bodyLoop(){
    // printf("bodyloop %d\n", Monitor::rank);
    Specialist *specialist = new BodySpecialist(Monitor::rank, Monitor::size);
    pthread_t handleThread;
    pthread_create( &handleThread, NULL, &handleLoop, specialist);
    Monitor::listen();
    pthread_join(handleThread,NULL);
    delete specialist;
}
void tailLoop(){
    // printf("tailloop %d\n", Monitor::rank);
    Specialist *specialist = new TailSpecialist(Monitor::rank,Monitor::size);
    pthread_t handleThread;
    pthread_create(&handleThread, NULL, &handleLoop, specialist);
    Monitor::listen();
    pthread_join(handleThread,NULL);
    delete specialist;
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
        printf("Multiple threads are not supported by open mpi. Exiting application");
    }
    MPI_Finalize();
    return 0;
}