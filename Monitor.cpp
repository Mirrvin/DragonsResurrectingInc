#include "Monitor.h"

int Monitor::rank;
int Monitor::size;
bool Monitor::listening = false;
unsigned int Monitor::lamport = 0;

pthread_mutex_t Monitor::lamportMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::handleMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::messageQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::mpiMutex = PTHREAD_MUTEX_INITIALIZER;

std::queue<packet_t> Monitor::messageQueue;

void Monitor::incrementLamportOnReceive(packet_t packet) {
    pthread_mutex_lock(&lamportMutex);
    lamport = std::max(packet.lamport, Monitor::lamport) + 1;
    pthread_mutex_unlock(&lamportMutex);
}

void Monitor::initialize() {
    MPI_Comm_rank(MPI_COMM_WORLD, &Monitor::rank);
    MPI_Comm_size(MPI_COMM_WORLD, &Monitor::size);
}

void Monitor::finalize() {
    pthread_mutex_destroy(&Monitor::handleMutex);
    pthread_mutex_destroy(&Monitor::messageQueueMutex);
    pthread_mutex_destroy(&Monitor::lamportMutex);
    pthread_mutex_destroy(&Monitor::mpiMutex);
}

void Monitor::listen() {
    Monitor::listening = true;
    MPI_Status status;
    packet_t packet;
    while(listening) {
        pthread_mutex_lock(&Monitor::mpiMutex);
        MPI_Recv( &packet, sizeof(packet_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_unlock(&Monitor::mpiMutex);
        packet.status = status;
        printf("Monitor of rank %d recived data %d from %d with tag %d\n", Monitor::rank, packet.id, packet.status.MPI_SOURCE, packet.status.MPI_TAG);
        pthread_mutex_lock(&Monitor::messageQueueMutex); // dostęp do kolejki wiadomości
        Monitor::messageQueue.push(packet);
        pthread_mutex_unlock(&Monitor::messageQueueMutex);
        pthread_mutex_unlock(&Monitor::handleMutex); // odblokowanie handleLoop
    }
}

void Monitor::endListening() {
    listening = false;
}

void Monitor::incrementLamportOnSend() {
    pthread_mutex_lock(&Monitor::lamportMutex);
    lamport += 1;
    pthread_mutex_unlock(&Monitor::lamportMutex);
}
// };
