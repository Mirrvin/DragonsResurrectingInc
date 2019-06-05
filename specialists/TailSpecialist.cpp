#include "../main.h"
#include "Specialist.h"
extern bool end;
class TailSpecialist: public Specialist {

public:
    TailSpecialist(int rank, int size):Specialist(rank, size) { }

    void handle(packet_t packet) {
        switch(packet.status.MPI_TAG){
            case NEED_TAIL:
                break;
            case END: 
                handleEnd(packet); break;
            default: 
                printf("handle Dupa %d\n",pthread_self()); break;
        }
    }

    // void send() {
    //     this->sendMessage();
    // }

    // void broadcast() {
    //     this->broadcastMessage();
    // }
};
