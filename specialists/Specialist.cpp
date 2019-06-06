#include "Specialist.h"
#include "../Monitor.h"

Specialist::Specialist(int rank, int size) { 
    this->rank = rank;
    this->size = size;
}
Specialist::~Specialist() { }
void Specialist::sendMessage(int tag, int target) {
    packet_t packet;
    this->sendMessage(packet, tag, target);
}
void Specialist::sendMessage(packet_t packet, int tag, int target) {
    MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, target, tag, MPI_COMM_WORLD);
}

void Specialist::broadcastMessage(int tag) {
    packet_t packet;
    this->broadcastMessage(packet, tag);
}
void Specialist::broadcastMessage(packet_t packet, int tag) {
    for(int i=1;i<this->size;i++){
        if(i != this->rank) {
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, tag, MPI_COMM_WORLD);
        }
    }
}

bool Specialist::handle(packet_t packet) {
    printf("Rank %d receiving packet from rank %d with tag %d. ", this->rank, packet.status.MPI_SOURCE, packet.status.MPI_TAG);
    printf("Handle not implemented\n");
    return false;
}

bool Specialist::handleEnd(packet_t packet){
    return false;
}
