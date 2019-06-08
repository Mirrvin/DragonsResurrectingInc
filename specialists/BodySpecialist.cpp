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
            if(this->rank == this->deskPriority[i].status.MPI_SOURCE)
                return i;
        }
        return this->deskPriority.size();
    }

    void notifyHeadsWithNoBodiesAssigned() {
        if(this->headList.size() > this->bodyList.size()) {
            for(unsigned int i=this->bodyList.size() - 1; i<this->bodyList.size(); i++) {
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
            case NEED_DESK:
                success = this->handleNeedDesk(packet); break;
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
                this->sendMessage(NEED_BODY_POSITIVE, this->headRank);
                printf("Body specialist %d matched with head %d --->\n", this->rank, this->headRank);
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

    bool handleNeedDesk(packet_t packet) {
        this->removeFromDeskPriority(packet.status.MPI_SOURCE);
        return true;
    }

    bool handleDoPaperwork(packet_t packet) {
        // printf("TODO implement handling paperwork request in Body Specialist...\n");
        packet = this->broadcastMessage(this->createSelfPacket(),NEED_DESK,BODY); // mowi innym BODY, zeby go dodali do kolejki
        deskPriority.push_back(packet); // dodaje siebie
        std::sort(this->deskPriority.begin(), this->deskPriority.end(), comparePackets);
        int me = getSelfIndexFromDeskPriority();
        do { // czeka aż bedzie mial biurko, glupi loop, ale nie powienien dlugo wisiec
            std::sort(this->deskPriority.begin(), this->deskPriority.end(), comparePackets);
            int me = getSelfIndexFromDeskPriority();
        } while(me >= DESK_NUMBER);

        this->broadcastMessage(this->createSelfPacket(),DONE_PAPERWORK,BODY); //mówi BODYm, żeby go usunęli z kolejek
        this->removeFromDeskPriority(this->rank); // usuwanie siebie z kolejki
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
