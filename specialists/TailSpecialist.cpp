#include "../main.h"
#include "Specialist.h"
extern bool end;
class TailSpecialist: public Specialist {
private:
    int headRank;
    int bodyRank;
    bool handlingNeedTail;
    std::vector<packet_t> tailList;
    std::vector<packet_t> bodyList;

    unsigned int getSelfIndexFromTailList() {
        for(unsigned int i=0; i<this->tailList.size(); i++) {
            if(this->rank == this->tailList[i].status.MPI_SOURCE)
                return i;
        }
        return this->bodyList.size();
    }
    void notifyBodiesWithNoTailsAssigned() {
        if(this->bodyList.size() > this->tailList.size()) {
            for(unsigned int i=this->tailList.size() - 1; i<this->tailList.size(); i++) {
                this->sendMessage(NEED_TAIL_NEGATIVE, this->bodyList[i].status.MPI_SOURCE);
            }
        }
    }

public:
    TailSpecialist(int rank, int size):Specialist(rank, size) {
        this->headRank = 0;
        this->bodyRank = 0;
        this->handlingNeedTail = false;
    }

    bool handle(packet_t packet) {
        printf("Tail specialist %d handling message with tag %d\n", this->rank, packet.status.MPI_TAG);
        bool success = false;
        switch(packet.status.MPI_TAG){
            case NEED_TAIL:
                success = this->handleNeedTail(packet); break;
            case ACK_NEED_TAIL:
                success = this->handleAckNeedTail(packet); break;
            case FINISH_NEED_TAIL:
                success = this->handleFinishNeedTail(packet); break;
            case AVENGERS_ASSEMBLED:
                success = this->handleAvengersAssembled(packet); break;
            case END: 
                success = this->handleEnd(packet); break;
            default: 
                printf("No handler for tag %d in HeadSpecialist\n", packet.status.MPI_TAG); break;
        }
        return success;
    }

    bool handleNeedTail(packet_t packet) {
        if(!this->inTeam) {
            if(!handlingNeedTail) {
                this->handlingNeedTail = true;
                this->tailList.clear();
                this->bodyList.clear();
                packet_t packet = this->createSelfPacket();
                packet_t sentPacket = this->broadcastMessage(packet, ACK_NEED_TAIL, TAIL);
                this->tailList.push_back(sentPacket);
                this->sendMessage(FINISH_NEED_TAIL, this->rank);
            }
            this->bodyList.push_back(packet);
        }
        return true;
    }

    bool handleAckNeedTail(packet_t packet) {
        if(!this->inTeam) {
            this->tailList.push_back(packet);
        }
        return true;
    }

    bool handleFinishNeedTail(packet_t packet) {
        if(!this->inTeam) {
            this->handlingNeedTail = false;
            std::sort(this->tailList.begin(), this->tailList.end(), comparePackets);
            std::sort(this->bodyList.begin(), this->bodyList.end(), comparePackets);
            unsigned int me = this->getSelfIndexFromTailList();
            bool haveMatchingBody = (this->bodyList.size() > me) && (me < this->tailList.size());
            if(haveMatchingBody) {
                this->inTeam = true;
                this->bodyRank = this->bodyList[me].status.MPI_SOURCE;
                this->headRank = this->bodyList[me].data;
                this->sendMessage(NEED_TAIL_POSITIVE, this->bodyList[me].status.MPI_SOURCE);
                printf("Tail specialist %d matched with body %d --->\n", this->rank, this->bodyList[me].status.MPI_SOURCE);
                if(me == 0) {
                    this->notifyBodiesWithNoTailsAssigned();
                }
                this->sendMessage(AVENGERS_ASSEMBLED, this->rank);
            }
        }
        return true;
    }

    bool handleAvengersAssembled(packet_t packet) {
        printf("--- Team assembled. Squad members: head %d, body %d, tail %d ---\n", this->headRank, this->bodyRank, this->rank);
        return true;
    }
};
