#include "../main.h"
#include "Specialist.h"

class HeadSpecialist: public Specialist {

public:
    HeadSpecialist(int rank, int size):Specialist(rank, size) {
        this->sendMessage(AVENGERS_ASSEMBLE,rank);
    }

    bool handle(packet_t packet) {
        printf("Head specialist %d handling message with tag %d\n", this->rank, packet.status.MPI_TAG);
        bool success = false;
        switch(packet.status.MPI_TAG){
            case AVENGERS_ASSEMBLE:
                success = this->handleAvengersAssemble(packet); break;
            case END:
                success = Specialist::handleEnd(packet); break;
            default: 
                printf("No handler for tag %d in HeadSpecialist\n", packet.status.MPI_TAG); break;
        }
        return success;
    }
    bool handleAvengersAssemble(packet_t packet) {
        this->broadcastMessage(NEED_BODY);
        return true;
    }
    // void send() {
    //     this->sendMessage();
    // }

    // void broadcast() {
    //     this->broadcastMessage();
    // }
};
