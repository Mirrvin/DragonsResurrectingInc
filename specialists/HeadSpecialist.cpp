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
        // printf("Head specialist %d handling message with tag %d\n", this->rank, packet.status.MPI_TAG);
        bool success = true;
        switch(packet.status.MPI_TAG){
            case AVENGERS_ASSEMBLE:
                success = this->handleAvengersAssemble(packet); break;
            case NEED_BODY_POSITIVE:
                success = this->handleNeedBodyPositive(packet); break;
            case NEED_BODY_NEGATIVE:
                success = this->handleNeedBodyNegative(packet); break;
            case AVENGERS_ASSEMBLED:
                success = this->handleAvengersAssembled(packet); break;
            case RESURRECTION_FINISHED:
                success = this->handleResurrectionFinished(packet); break;
            case END:
                success = this->handleEnd(packet); break;
            default: 
                printf("%u: No handler for tag %d in HeadSpecialist\n", Monitor::getLamport(), packet.status.MPI_TAG); break;
        }
        return success;
    }
    bool handleAvengersAssemble(packet_t packet) {
        this->broadcastMessage(this->createSelfPacket(), NEED_BODY, BODY);
        return true;
    }

    bool handleNeedBodyPositive(packet_t packet) {
        if(!this->inTeam) {
            printf("%u: <--- Head specialist %d matched with body %d\n", Monitor::getLamport(), this->rank, packet.status.MPI_SOURCE);
            this->bodyRank = packet.status.MPI_SOURCE;
            this->inTeam = true;
        }
        return true;
    }

    bool handleNeedBodyNegative(packet_t packet) {
        this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
        return true;
    }

    bool handleAvengersAssembled(packet_t packet) {
        this->tailRank = packet.status.MPI_SOURCE;
        return true;
    }

    bool handleResurrectionFinished(packet_t packet) {
        this->inTeam = false;
        this->tailRank = 0;
        this->bodyRank = 0;
        this->sendMessage(AVENGERS_ASSEMBLE,rank);
        return true;
    }
};
