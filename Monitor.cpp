#include "main.h"
#include "specialists/Specialist.h"
class Monitor {
private:
    static bool listening;
    static unsigned int lamport;

    static pthread_mutex_t lamportMutex;

    static void incrementLamportOnReceive(packet_t packet) {
        pthread_mutex_lock(&lamportMutex);
        lamport = std::max(packet.lamport, lamport) + 1;
        pthread_mutex_unlock(&lamportMutex);
    }

public:
    static int rank;
    static int size;
    static pthread_mutex_t handleMutex;
    static pthread_mutex_t messageQueueMutex;
    static std::queue<packet_t> messageQueue;

    static void initialize() {
        lamportMutex = PTHREAD_MUTEX_INITIALIZER;
        handleMutex = PTHREAD_MUTEX_INITIALIZER;
        messageQueueMutex = PTHREAD_MUTEX_INITIALIZER;
        lamport = 0;
        listening = false;

        MPI_Comm_rank(MPI_COMM_WORLD, &Monitor::rank);
        MPI_Comm_size(MPI_COMM_WORLD, &Monitor::size);
    }

    static void listen() {
        listening = true;
        MPI_Status status;
        packet_t packet;
        while(listening) {
            MPI_Recv( &packet, sizeof(packet_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("recive rank:%i thread:%d \n",rank,pthread_self());
            packet.status = status;
            pthread_mutex_lock(&Monitor::messageQueueMutex); // dostęp do kolejki wiadomości
            messageQueue.push(packet);
            pthread_mutex_unlock(&Monitor::messageQueueMutex);
            pthread_mutex_unlock(&handleMutex); // odblokowanie handleLoop

            // endListening();
        }
    }

    static void endListening() {
        listening = false;
    }

    static void incrementLamportOnSend() {
        pthread_mutex_lock(&lamportMutex);
        lamport += 1;
        pthread_mutex_unlock(&lamportMutex);
    }
};
