#include "Monitor.h"

int Monitor::rank;
int Monitor::size;
bool Monitor::listening = false;
unsigned int Monitor::lamport = 0;

pthread_mutex_t Monitor::lamportMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::handleMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::messageQueueMutex = PTHREAD_MUTEX_INITIALIZER;

std::queue<packet_t> Monitor::messageQueue;

void Monitor::incrementLamportOnReceive(packet_t packet) {
    pthread_mutex_lock(&Monitor::lamportMutex);
    Monitor::lamport = std::max(packet.lamport, Monitor::lamport) + 1;
    pthread_mutex_unlock(&Monitor::lamportMutex);
}

void Monitor::initialize() {
    MPI_Comm_rank(MPI_COMM_WORLD, &Monitor::rank);
    MPI_Comm_size(MPI_COMM_WORLD, &Monitor::size);
}

void Monitor::finalize() {
    pthread_mutex_destroy(&Monitor::handleMutex);
    pthread_mutex_destroy(&Monitor::messageQueueMutex);
    pthread_mutex_destroy(&Monitor::lamportMutex);
}

void Monitor::listen() {
    Monitor::listening = true;
    MPI_Status status;
    packet_t packet;
    while(Monitor::listening) {
        MPI_Recv( &packet, sizeof(packet_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        Monitor::incrementLamportOnReceive(packet);
        packet.status = status;
        // printf("Monitor of rank %d recived data %d from %d with tag %d\n", Monitor::rank, packet.data, packet.status.MPI_SOURCE, packet.status.MPI_TAG);
        pthread_mutex_lock(&Monitor::messageQueueMutex); // dostęp do kolejki wiadomości
        Monitor::messageQueue.push(packet);
        pthread_mutex_unlock(&Monitor::messageQueueMutex);
        pthread_mutex_unlock(&Monitor::handleMutex); // odblokowanie handleLoop
        if(packet.status.MPI_TAG == END)
            listening = false;
    }
}

void Monitor::endListening() {
    Monitor::listening = false;
}

unsigned int Monitor::incrementLamportOnSend() {
    pthread_mutex_lock(&Monitor::lamportMutex);
    Monitor::lamport += 1;
    unsigned int incrementedLamport = Monitor::lamport;
    pthread_mutex_unlock(&Monitor::lamportMutex);
    return incrementedLamport;
}

unsigned int Monitor::getLamport() {
    return Monitor::lamport;
}

