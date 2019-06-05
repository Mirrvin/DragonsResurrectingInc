#include "../main.h"
#include "Specialist.h"
extern bool end;
class BodySpecialist: public Specialist {
private:
    std::vector<packet_t> headsList;

public:
    BodySpecialist(int rank, int size):Specialist(rank, size) { }

    void handle(packet_t packet) {
        switch(packet.status.MPI_TAG){
            case NEED_BODY:
                handleNeedBody(packet); break;
            case END: 
                handleEnd(packet); break;
            default: 
                printf("handle Dupa %d\n",pthread_self()); break;
        }
    }

    void handleNeedBody(packet_t packet) {
        headsList.push_back(packet);
        printf("dodana glowa %i do kolejki w %i",packet.status.MPI_SOURCE,rank);
    }
    // void send() {
    //     this->sendMessage();
    // }

    // void broadcast() {
    //     this->broadcastMessage();
    // }
};
