class Monitor;
#include "../Monitor.h"

class Specialist {
protected:
    int rank;
    int size; 

public:
    Specialist(int rank, int size) { 
        this->rank = rank;
        this->size = size;
    }
    void sendMessage(int tag, int target) {
        packet_t packet;
        MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, target, tag, MPI_COMM_WORLD);
    }
    void sendMessage(packet_t packet, int tag, int target) {
        MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, target, tag, MPI_COMM_WORLD);
    }

    void broadcastMessage(int tag) {
        packet_t packet;
        for(int i=1;i<size;i++){
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, tag, MPI_COMM_WORLD);
        }
    }
    void broadcastMessage(packet_t packet, int tag) {
        for(int i=1;i<size;i++){
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, tag, MPI_COMM_WORLD);
        }
    }

    void handle(packet_t packet) {
        printf("Handle not implemented");
    }

    void handleEnd(packet_t packet){
        end = true;
    }
};
