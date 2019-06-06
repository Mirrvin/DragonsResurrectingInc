#include "../main.h"
#include "Specialist.h"

class HeadSpecialist: public Specialist {
private:
    // std::vector<packet_t> bodyList;
    int bodyRank;
    int tailRank;

public:
    HeadSpecialist(int rank, int size):Specialist(rank, size) {
        this->bodyRank = 0;
        this->tailRank = 0;
        this->sendMessage(AVENGERS_ASSEMBLE,rank);
    }

    bool handle(packet_t packet) {
        printf("Head specialist %d handling message with tag %d\n", this->rank, packet.status.MPI_TAG);
        this->bodyRank = 0;
        bool success = false;
        switch(packet.status.MPI_TAG){
            case AVENGERS_ASSEMBLE:
                success = this->handleAvengersAssemble(packet); break;
            case NEED_BODY_POSITIVE:
                success = this->handleNeedBodyPositive(packet); break;
            case NEED_BODY_NEGATIVE:
                success = this->handleNeedBodyNegative(packet); break;
            case END:
                success = this->handleEnd(packet); break;
            default: 
                printf("No handler for tag %d in HeadSpecialist\n", packet.status.MPI_TAG); break;
        }
        return success;
    }
    bool handleAvengersAssemble(packet_t packet) {
        // this->bodyList.clear();
        this->broadcastMessage(NEED_BODY, BODY);
        return true;
    }

    bool handleNeedBodyPositive(packet_t packet) {
        if(!this->inTeam) {
            printf("<-- Head specialist %d matched with body %d\n", this->rank, packet.status.MPI_SOURCE);
            this->bodyRank = packet.status.MPI_SOURCE;
            this->inTeam = true;
        }
        return true;
    }

    bool handleNeedBodyNegative(packet_t packet) {
        this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
        return true;
    }
};
