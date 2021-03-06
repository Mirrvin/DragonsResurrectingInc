#include "../main.h"
#include "Specialist.h"

class HeadSpecialist: public Specialist {
private:
    int bodyRank;
    int tailRank;

public:
    HeadSpecialist(int rank, int size):Specialist(rank, size) {
        this->bodyRank = 0;
        this->tailRank = 0;
        this->sendMessage(AVENGERS_ASSEMBLE,rank);
    }

    bool handle(packet_t packet) {    
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
        if(this->bodyRank == 0)
            this->broadcastMessage(this->createSelfPacket(), NEED_BODY, BODY);
        return true;
    }

    bool handleNeedBodyPositive(packet_t packet) {       
        if(!this->inTeam) {           
            this->bodyRank = packet.status.MPI_SOURCE;
            this->inTeam = true;
            this->sendMessage(NEED_BODY_POSITIVE, packet.status.MPI_SOURCE);
        } else {
            this->sendMessage(NEED_BODY_NEGATIVE, packet.status.MPI_SOURCE);
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
