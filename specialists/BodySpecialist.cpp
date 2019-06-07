#include "../main.h"
#include "Specialist.h"
#include "../Monitor.h"

extern bool end;
class BodySpecialist: public Specialist {
private:
    std::vector<packet_t> headsList;
    std::vector<packet_t> bodyList;

    unsigned int getSelfIndexFromBodyList() {
        for(unsigned int i=0; i<this->bodyList.size(); i++) {
            if(this->rank == this->bodyList[i].status.MPI_SOURCE)
                return i;
        }
        return this->bodyList.size();
    }
    bool amIFirstBody() {
        if(!this->bodyList.empty()) {
            if(this->bodyList[0].status.MPI_SOURCE == this->rank)
                return true;
        }
        return false;
    }
    void notifyHeadsWithNoBodiesAssigned() {
        if(this->headsList.size() > this->bodyList.size()) {
            for(unsigned int i=this->bodyList.size() - 1; i<this->bodyList.size(); i++) {
                this->sendMessage(NEED_BODY_NEGATIVE, this->headsList[i].status.MPI_SOURCE);
            }
        }
    }
    packet_t createSelfPacket() {
        packet_t packet;
        packet.status.MPI_SOURCE = this->rank;
        packet.lamport = Monitor::getLamport();
        return packet;
    }

public:
    BodySpecialist(int rank, int size):Specialist(rank, size) { }

    bool handle(packet_t packet) {
        printf("Body specialist %d handling message with tag %d\n", this->rank, packet.status.MPI_TAG);
        bool success = false;
        switch(packet.status.MPI_TAG){
            case NEED_BODY:
                success = this->handleNeedBody(packet); break;
            case ACK_NEED_BODY:
                success = this->handleAckNeedBody(packet); break;
            case FINISH_NEED_BODY:
                success = this->handleFinishNeedBody(packet); break;
            case END:
                success = this->handleEnd(packet); break;
            default: 
                printf("No handler for tag %d in BodySpecialist\n", packet.status.MPI_TAG); break;                
        }
        return success;
    }

    bool handleNeedBody(packet_t packet) {
        if(!this->inTeam) {
            this->bodyList.clear();
            this->headsList.clear();
            this->headsList.push_back(packet);
            this->broadcastMessage(ACK_NEED_BODY, BODY);
            this->sendMessage(FINISH_NEED_BODY, this->rank);
        }
        return true;
    }

    bool handleAckNeedBody(packet_t packet) {
        if(!this->inTeam) {
            this->bodyList.push_back(packet);
        }
        return true;
    }

    bool handleFinishNeedBody(packet_t packet) {
        if(!this->inTeam) {
            this->bodyList.push_back(this->createSelfPacket());
            std::sort(this->bodyList.begin(), this->bodyList.end(), comparePackets);
            std::sort(this->headsList.begin(), this->headsList.end(), comparePackets);
            unsigned int me = getSelfIndexFromBodyList();
            bool haveMatchingHead = this->headsList.size() > me;
            if(haveMatchingHead) {
                this->sendMessage(NEED_BODY_POSITIVE, this->headsList[me].status.MPI_SOURCE);
                this->inTeam = true;
                printf("Body specialist %d matched with head %d --->\n", this->rank, this->headsList[me].status.MPI_SOURCE);
                if(me == 0) {
                    this->notifyHeadsWithNoBodiesAssigned();
                }
                this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
            }
        }
        return true;
    }

};
