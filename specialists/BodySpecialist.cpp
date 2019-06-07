#include "../main.h"
#include "Specialist.h"
#include "../Monitor.h"

class BodySpecialist: public Specialist {
private:
    int headRank;
    int tailRank;
    bool handlingNeedBody;
    std::vector<packet_t> headList;
    std::vector<packet_t> bodyList;

    unsigned int getSelfIndexFromBodyList() {
        for(unsigned int i=0; i<this->bodyList.size(); i++) {
            if(this->rank == this->bodyList[i].status.MPI_SOURCE)
                return i;
        }
        return this->bodyList.size();
    }

    void notifyHeadsWithNoBodiesAssigned() {
        if(this->headList.size() > this->bodyList.size()) {
            for(unsigned int i=this->bodyList.size() - 1; i<this->bodyList.size(); i++) {
                this->sendMessage(NEED_BODY_NEGATIVE, this->headList[i].status.MPI_SOURCE);
            }
        }
    }

public:
    BodySpecialist(int rank, int size):Specialist(rank, size) {
        this->headRank = 0;
        this->tailRank = 0;
        this->handlingNeedBody = false;
    }

    bool handle(packet_t packet) {
        // printf("Body specialist %d handling message with tag %d\n", this->rank, packet.status.MPI_TAG);
        bool success = true;
        switch(packet.status.MPI_TAG){
            case NEED_BODY:
                success = this->handleNeedBody(packet); break;
            case NEED_BODY_ACK:
                success = this->handleNeedBodyAck(packet); break;
            case FINISH_NEED_BODY:
                success = this->handleFinishNeedBody(packet); break;
            case NEED_TAIL_POSITIVE:
                success = this->handleNeedTailPositive(packet); break;
            case NEED_TAIL_NEGATIVE:
                success = this->handleNeedTailNegative(packet); break;
            case AVENGERS_ASSEMBLE:
                success = this->handleAvengersAssemble(packet); break;
            case DO_PAPERWORK:
                success = this->handleDoPaperwork(packet); break;
            case RESURRECTION_FINISHED:
                success = this->handleResurrectionFinished(packet); break;
            case END:
                success = this->handleEnd(packet); break;
            default: 
                printf("No handler for tag %d in BodySpecialist\n", packet.status.MPI_TAG); break;                
        }
        return success;
    }

    bool handleNeedBody(packet_t packet) {
        if(!this->inTeam) {
            if(!handlingNeedBody) {
                this->handlingNeedBody = true;
                this->bodyList.clear();
                this->headList.clear();
                packet_t packet = this->createSelfPacket();
                packet_t sentPacket = this->broadcastMessage(packet, NEED_BODY_ACK, BODY);
                this->bodyList.push_back(sentPacket);
                this->sendMessage(FINISH_NEED_BODY, this->rank);
            }
            this->headList.push_back(packet);
        }
        return true;
    }

    bool handleNeedBodyAck(packet_t packet) {
        if(!this->inTeam) {
            this->bodyList.push_back(packet);
        }
        return true;
    }

    bool handleFinishNeedBody(packet_t packet) {
        if(!this->inTeam) {
            this->handlingNeedBody = false;
            std::sort(this->bodyList.begin(), this->bodyList.end(), comparePackets);
            std::sort(this->headList.begin(), this->headList.end(), comparePackets);
            unsigned int me = this->getSelfIndexFromBodyList();
            bool haveMatchingHead = (this->headList.size() > me) && (me < this->bodyList.size());
            if(haveMatchingHead) {
                this->inTeam = true;
                this->headRank = this->headList[me].status.MPI_SOURCE;
                this->sendMessage(NEED_BODY_POSITIVE, this->headList[me].status.MPI_SOURCE);
                printf("Body specialist %d matched with head %d --->\n", this->rank, this->headList[me].status.MPI_SOURCE);
                if(me == 0) {
                    this->notifyHeadsWithNoBodiesAssigned();
                }
                this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
            }
        }
        return true;
    }

    bool handleAvengersAssemble(packet_t packet) {
        if(this->inTeam) {
            packet_t packet = this->createSelfPacket();
            packet.data = this->headRank;
            this->broadcastMessage(packet, NEED_TAIL, TAIL);
        }
        return true;
    }

    bool handleNeedTailPositive(packet_t packet) {
        if(!this->inTeam) {
            printf("<--- Body specialist %d matched with tail %d\n", this->rank, packet.status.MPI_SOURCE);
            this->tailRank = packet.status.MPI_SOURCE;
            this->inTeam = true;
        }
        return true;
    }

    bool handleNeedTailNegative(packet_t packet) {
        this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
        return true;
    }

    bool handleDoPaperwork(packet_t packet) {
        printf("TODO implement handling paperwork request in Body Specialist...\n");
        this->sendMessage(DONE_PAPERWORK, this->tailRank);
        return true;
    }

    bool handleResurrectionFinished(packet_t packet) {
        this->inTeam = false;
        this->headRank = 0;
        this->tailRank = 0;
        return true;
    }
};
