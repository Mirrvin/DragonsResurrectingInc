#include "Specialist.h"
#include "../Monitor.h"

Specialist::Specialist(int rank, int size) { 
    this->rank = rank;
    this->size = size;
    this->resurrectionCounter = 0;
    this->inTeam = false;
}
Specialist::~Specialist() { }
packet_t Specialist::sendMessage(int tag, int target) {
    packet_t packet;
    return this->sendMessage(packet, tag, target);
}
packet_t Specialist::sendMessage(packet_t packet, int tag, int target) {
    packet.lamport = Monitor::incrementLamportOnSend();
    MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, target, tag, MPI_COMM_WORLD);
    return packet;
}

packet_t Specialist::broadcastMessage(int tag) {
    packet_t packet;
    return this->broadcastMessage(packet, tag);
}
packet_t Specialist::broadcastMessage(packet_t packet, int tag) {
    packet.lamport = Monitor::incrementLamportOnSend();
    for(int i=1;i<this->size;i++){
        if(i != this->rank) {
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, tag, MPI_COMM_WORLD);
        }
    }
    return packet;
}
packet_t Specialist::broadcastMessage(int tag, int targetType) {
    packet_t packet;
    return this->broadcastMessage(packet, tag, targetType);
}
packet_t Specialist::broadcastMessage(packet_t packet, int tag, int targetType) {
    packet.lamport = Monitor::incrementLamportOnSend();
    for(int i=1;i<this->size;i++){
        if(i != this->rank && (i%3 + 1) == targetType) {
            MPI_Send(&packet, sizeof(packet_t), MPI_BYTE, i, tag, MPI_COMM_WORLD);
        }
    }
    return packet;
}

bool Specialist::handle(packet_t packet) {
    printf("Rank %d receiving packet from rank %d with tag %d. ", this->rank, packet.status.MPI_SOURCE, packet.status.MPI_TAG);
    printf("Handle not implemented\n");
    return false;
}

bool Specialist::handleEnd(packet_t packet){
    return false;
}

packet_t Specialist::createSelfPacket() {
    packet_t packet;
    packet.ressurectCounter = this->resurrectionCounter;
    packet.status.MPI_SOURCE = this->rank;
    packet.lamport = Monitor::getLamport();
    return packet;
}
