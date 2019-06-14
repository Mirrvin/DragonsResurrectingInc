#include "../main.h"
#include "Specialist.h"
// #include "../Monitor.h"

class BodySpecialist: public Specialist {
private:
    int headRank;
    int tailRank;
    int needDeskReplyCounter;
    bool handlingNeedBody;
    bool gettingDesk;
    std::vector<packet_t> headList;
    std::vector<packet_t> bodyList;
    std::vector<packet_t> deskPriority;


    unsigned int getSelfIndexFromBodyList() {
        for(unsigned int i=0; i<this->bodyList.size(); i++) {
            if(this->rank == this->bodyList[i].status.MPI_SOURCE)
                return i;
        }
        return this->bodyList.size();
    }

    unsigned int getSelfIndexFromDeskPriority() {
        for(unsigned int i=0; i<this->deskPriority.size(); i++) {
            if(this->rank == this->deskPriority[i].status.MPI_SOURCE){
                // printf("getSelfIndexFromDeskPriority , rank: %d i: %d\n",this->rank,i);
                return i;
            }
        }
        return this->deskPriority.size();
    }

    void notifyHeadsWithNoBodiesAssigned() {
        if(this->headList.size() > this->bodyList.size()) {
            for(unsigned int i=this->bodyList.size() - 1; i<this->headList.size(); i++) {
                this->sendMessage(NEED_BODY_NEGATIVE, this->headList[i].status.MPI_SOURCE);
            }
        }
    }

    void removeFromDeskPriority(int rank) {
        for(unsigned int i = 0; i < this->deskPriority.size(); i++) {
            if (this->deskPriority[i].status.MPI_SOURCE == rank) {
                this->deskPriority.erase(this->deskPriority.begin()+i);
            }
        }
    }

public:
    BodySpecialist(int rank, int size):Specialist(rank, size) {
        this->headRank = 0;
        this->tailRank = 0;
        this->needDeskReplyCounter = 0;
        this->handlingNeedBody = false;
        this->gettingDesk = false;
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
            case NEED_DESK_REQUEST:
                success = this->handleNeedDeskRequest(packet); break;
            case NEED_DESK_REPLY:
                success = this->handleNeedDeskReply(packet); break;
            case FREE_DESK:
                success = this->handleFreeDesk(packet); break;
            case DO_PAPERWORK:
                success = this->handleDoPaperwork(packet); break;
            case DONE_PAPERWORK:
                success = this->handleDonePaperwork(packet); break;
            case RESURRECTION_FINISHED:
                success = this->handleResurrectionFinished(packet); break;
            case NEED_BODY_POSITIVE:
                success = this->handleNeedBodyPositive(packet); break;
            case NEED_BODY_NEGATIVE:
                success = this->handleNeedBodyNegative(packet); break;
            case END:
                success = this->handleEnd(packet); break;
            default: 
                printf("%u: No handler for tag %d in BodySpecialist\n",Monitor::getLamport(), packet.status.MPI_TAG); break;                
        }
        return success;
    }

    bool handleNeedBody(packet_t packet) {
    // printf("%u: handleNeedBody, rank: %d\n", Monitor::getLamport(), this->rank);
        if(this->headRank == 0) {
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
    // printf("%u: handleFinishNeedBody, headRank: %d, rank: %d\n", Monitor::getLamport(), this->headRank, this->rank);
        if(this->headRank == 0) {
            this->handlingNeedBody = false;
            std::sort(this->bodyList.begin(), this->bodyList.end(), comparePackets);
            std::sort(this->headList.begin(), this->headList.end(), comparePackets);
            unsigned int me = this->getSelfIndexFromBodyList();
            bool haveMatchingHead = (this->headList.size() > me) && (me < this->bodyList.size());
            if(haveMatchingHead) {
                // printf("%u: handleFinishNeedBody haveMatchingHead, headRank: %d, rank: %d\n", Monitor::getLamport(), this->headRank, this->rank);
                this->inTeam = true;
                // this->headRank = this->headList[me].status.MPI_SOURCE; 
                this->sendMessage(NEED_BODY_POSITIVE, this->headList[me].status.MPI_SOURCE);
                // printf("%u: Body specialist %d matched with head %d --->\n",Monitor::getLamport(), this->rank, this->headRank);
                if(me == 0) {
                    this->notifyHeadsWithNoBodiesAssigned();
                }
                // this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
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
        if(this->tailRank == 0) {
            // printf("%u: <--- Body specialist %d matched with tail %d\n",Monitor::getLamport(), this->rank, packet.status.MPI_SOURCE);
            this->tailRank = packet.status.MPI_SOURCE;
            this->inTeam = true;
            this->sendMessage(NEED_TAIL_POSITIVE, packet.status.MPI_SOURCE);
        } else {
            this->sendMessage(NEED_TAIL_NEGATIVE, packet.status.MPI_SOURCE);
        }
        return true;
    }

    bool handleNeedTailNegative(packet_t packet) {
        this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
        return true;
    }

    // bool handleNeedDesk(packet_t packet) {
    //     this->removeFromDeskPriority(packet.status.MPI_SOURCE);
    //     return true;
    // }

    bool handleDoPaperwork(packet_t packet) {
        this->needDeskReplyCounter = 1/3*(this->size+1)-1;
        packet = this->broadcastMessage(NEED_DESK_REQUEST, BODY); // mowi innym BODY, zeby go dodali do kolejki
        deskPriority.push_back(packet); // dodaje siebie
        this->gettingDesk = true;
        return true;
    }

    bool handleNeedDeskRequest(packet_t packet) {
        // printf("DeskRequest , rank: %d\n",this->rank);
        this->deskPriority.push_back(packet);
        this->sendMessage(NEED_DESK_REPLY, packet.status.MPI_SOURCE);
        return true;
    }

    bool handleNeedDeskReply(packet_t packet) {
        this->needDeskReplyCounter -= 1;
        if(this->needDeskReplyCounter <= 0) {
            this->tryToOccupyDesk();
        }
        return true;
    }

    bool handleFreeDesk(packet_t packet) {
        // printf("FreeDesk , rank: %d\n",this->rank);
        this->removeFromDeskPriority(packet.status.MPI_SOURCE);
        if(this->gettingDesk) {
            this->tryToOccupyDesk();
        }
        return true;
    }

    bool handleDonePaperwork(packet_t packet) {
        // printf("DonePaperwork , rank: %d --- his tail: %d\n",this->rank,this->tailRank);
        this->sendMessage(DONE_PAPERWORK, this->tailRank);
        return true;
    }

    bool handleResurrectionFinished(packet_t packet) {
        // printf("FinishedResurrecting , rank: %d\n",this->rank);
        this->inTeam = false;
        this->headRank = 0;
        this->tailRank = 0;
        return true;
    }

    bool handleNeedBodyPositive(packet_t packet) {
        // printf("%u: <--- Body specialist %d matched with tail %d\n",Monitor::getLamport(), this->rank, packet.status.MPI_SOURCE);
        this->headRank = packet.status.MPI_SOURCE;
        this->sendMessage(AVENGERS_ASSEMBLE, this->rank);
        this->handlingNeedBody = false;
        return true;
    }

    bool handleNeedBodyNegative(packet_t packet) {
        this->headRank = 0;
        this->handlingNeedBody = false;
        this->broadcastMessage(AVENGERS_ASSEMBLE,HEAD);
        return true;
    }

    void tryToOccupyDesk() {
        // printf("tryToOccupyDesk , rank: %d\n",this->rank);
        std::sort(this->deskPriority.begin(), this->deskPriority.end(), comparePacketsLamport);
        unsigned int me = this->getSelfIndexFromDeskPriority();
        if(me < this->deskPriority.size() && me < DESK_NUMBER) {
            printf("%u: Got desk, doing Paper work, rank: %d\n",Monitor::getLamport(),this->rank);
            sleep(1);
            this->gettingDesk = false;
            this->removeFromDeskPriority(this->rank);
            this->broadcastMessage(FREE_DESK, BODY);
            this->sendMessage(DONE_PAPERWORK, this->rank);
            printf("%u: Freeing desk, doing Paper work, rank: %d\n",Monitor::getLamport(),this->rank);
        }
    }
};
