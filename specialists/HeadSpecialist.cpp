#include "../main.h"
#include "Specialist.h"
extern bool end;
class HeadSpecialist: public Specialist {

public:
    HeadSpecialist(int rank, int size):Specialist(rank, size) {
        packet_t packet;
        this->sendMessage(packet, AVENGERS_ASSEMBLE,rank);
     }

    void handle(packet_t packet) {
        switch(packet.status.MPI_TAG){
            case AVENGERS_ASSEMBLE:
                handleAvengersAssemble(packet); break;
            case END: 
                handleEnd(packet); break;
            default: 
                printf("handle Dupa %d\n",pthread_self()); break;
        }
    }
    void handleAvengersAssemble(packet_t packet) {
        broadcastMessage(NEED_BODY);
    }
    // void send() {
    //     this->sendMessage();
    // }

    // void broadcast() {
    //     this->broadcastMessage();
    // }
};
