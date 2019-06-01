#include "main.h"
// class Specialist;
#include "specialists/Specialist.h"
class Monitor {
private:
    bool listening = false;
    unsigned int lamport = 0;
    pthread_mutex_t *lamportMutex = nullptr;
    Specialist *specialist = nullptr;

    void incrementLamportOnReceive(packet_t packet) {
        pthread_mutex_lock(this->lamportMutex);
        this->lamport = std::max(packet.lamport, this->lamport) + 1;
        pthread_mutex_unlock(this->lamportMutex);
    }

public:
    Monitor(Specialist *specialist, pthread_mutex_t *lamportMutex) {
        this->specialist = specialist;
        this->lamportMutex = lamportMutex;
    }

    void listen() {
        this->listening = true;
        MPI_Status status;
        packet_t packet;
        while(listening) {
            MPI_Recv( &packet, sizeof(packet_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("hlep");
            this->incrementLamportOnReceive(packet);
            this->specialist->handle(packet, status);
            this->endListening();
        }
    }

    void endListening() {
        this->listening = false;
    }

    void incrementLamportOnSend() {
        pthread_mutex_lock(this->lamportMutex);
        this->lamport += 1;
        pthread_mutex_unlock(this->lamportMutex);
    }
};
