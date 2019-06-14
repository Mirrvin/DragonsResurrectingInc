#include "../main.h"
#include "Specialist.h"
#include "../Monitor.h"

class TailSpecialist: public Specialist {
private:
    int headRank;
    int bodyRank;
    int orderPriorityReplyCounter;
    int availableOrders;
    bool handlingNeedTail;
    bool gettingResurrectionOrder;
    bool resurrecting;
    std::vector<packet_t> tailList;
    std::vector<packet_t> bodyList;
    std::vector<packet_t> orderPriority;

    unsigned int getSelfIndexFromTailList() {
        for(unsigned int i=0; i<this->tailList.size(); i++) {
            if(this->rank == this->tailList[i].status.MPI_SOURCE){
                return i;
            }
        }
        return this->bodyList.size();
    }

    unsigned int getIndexFromOrderPriority(int rank) {
        for(unsigned int i=0; i<this->orderPriority.size(); i++) {
            if(rank == this->orderPriority[i].status.MPI_SOURCE){
                return i;
            }
        }
        return this->orderPriority.size();
    }

    void notifyBodiesWithNoTailsAssigned() {
        if(this->bodyList.size() > this->tailList.size()) {
            for(unsigned int i=this->bodyList.size() - 1; i<this->tailList.size(); i++) {
                this->sendMessage(NEED_TAIL_NEGATIVE, this->bodyList[i].status.MPI_SOURCE);
            }
        }
    }

public:
    TailSpecialist(int rank, int size):Specialist(rank, size) {
        this->headRank = 0;
        this->bodyRank = 0;
        this->handlingNeedTail = false;
        this->gettingResurrectionOrder = false;
        this->availableOrders = 0;
    }

    bool handle(packet_t packet) {
        // printf("Tail specialist %d handling message with tag %d\n", this->rank, packet.status.MPI_TAG);
        bool success = true;
        switch(packet.status.MPI_TAG){
            case NEED_TAIL:
                success = this->handleNeedTail(packet); break;
            case NEED_TAIL_ACK:
                success = this->handleNeedTailAck(packet); break;
            case FINISH_NEED_TAIL:
                success = this->handleFinishNeedTail(packet); break;
            case AVENGERS_ASSEMBLED:
                success = this->handleAvengersAssembled(packet); break;
            case REQUEST_ORDER_PRIORITY:
                success = this->handleRequestOrderPriority(packet); break;
            case REPLY_ORDER_PRIORITY:
                success = this->handleReplyOrderPriority(packet); break;
            case FREE_ORDER_PRIORITY:
                success = this->handleFreeOrderPriority(packet); break;
            case NEW_ORDER:
                success = this->handleNewOrder(packet); break;
            case DONE_PAPERWORK:
                success = this->handleDonePaperwork(packet); break;
            case RESURRECTION_FINISHED:
                success = this->handleResurrectionFinished(packet); break;
            case NEED_TAIL_POSITIVE:
                success = this->handleNeedTailPositive(packet); break;
            case NEED_TAIL_NEGATIVE:
                success = this->handleNeedTailNegative(packet); break;
            case END:
                success = this->handleEnd(packet); break;
            default:
                printf("%u: No handler for tag %d in HeadSpecialist\n",Monitor::getLamport(), packet.status.MPI_TAG); break;
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
                packet_t sentPacket = this->broadcastMessage(packet, NEED_TAIL_ACK, TAIL);
                this->tailList.push_back(sentPacket);
                this->sendMessage(FINISH_NEED_TAIL, this->rank);
            }
            this->bodyList.push_back(packet);
        }
        return true;
    }

    bool handleNeedTailAck(packet_t packet) {
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
                // this->inTeam = true;
                // this->bodyRank = this->bodyList[me].status.MPI_SOURCE;
                this->headRank = this->bodyList[me].data;
                this->sendMessage(NEED_TAIL_POSITIVE, this->bodyList[me].status.MPI_SOURCE);
                if(me == 0) {
                    this->notifyBodiesWithNoTailsAssigned();
                }
                // this->sendMessage(AVENGERS_ASSEMBLED, this->rank);
                // this->sendMessage(AVENGERS_ASSEMBLED, this->headRank);
            }
        }
        return true;
    }

    bool handleAvengersAssembled(packet_t packet) {
        printf("%u: --- Team assembled. Squad members: head %d, body %d, tail %d ---\n",Monitor::getLamport(),this->headRank, this->bodyRank, this->rank);
        this->gettingResurrectionOrder = true;
        // printf("%u: --- TA po gettingResurrectionOrder, rank: %d\n",Monitor::getLamport(), this->rank);
        this->orderPriorityReplyCounter = (this->size)/3 - 1;
        // printf("%u: --- TA po (this->size)/3 - 1, rank: %d\n",Monitor::getLamport(), this->rank);
        if(this->orderPriorityReplyCounter > 0) {
            // printf("%u: --- TA in IF, rank: %d\n",Monitor::getLamport(), this->rank);
            packet_t packet = this->broadcastMessage(REQUEST_ORDER_PRIORITY, TAIL);
            // printf("%u: --- TA in IF after broadcast, rank: %d\n",Monitor::getLamport(), this->rank);
            this->orderPriority.push_back(packet);
            // printf("%u: --- TA in IF aster push_back, rank: %d\n",Monitor::getLamport(), this->rank);

        } else {
            // printf("%u: --- TA in ELSE, rank: %d\n",Monitor::getLamport(), this->rank);
            this->sendMessage(REPLY_ORDER_PRIORITY, this->rank);
        }
        // printf("%u: --- TA RET, rank: %d\n",Monitor::getLamport(), this->rank);
        return true;
    }

    bool handleRequestOrderPriority(packet_t packet) {
        // printf("%u: handleRequestOrderPriority , rank: %d\n",Monitor::getLamport(), this->rank);
        this->orderPriority.push_back(packet);
        this->sendMessage(REPLY_ORDER_PRIORITY, packet.status.MPI_SOURCE);
        return true;
    }

    bool handleReplyOrderPriority(packet_t packet) {
        // printf("%u: handleReplyOrderPriority, orderPriorityReplyCounter: %d, rank: %d,    ",Monitor::getLamport(), this->orderPriorityReplyCounter,this->rank);
        if (this->orderPriorityReplyCounter == 1) {
            this->orderPriorityReplyCounter = 0;
        } else {
            this->orderPriorityReplyCounter = this->orderPriorityReplyCounter - 1;
        }
        // printf("%u: handleReplyOrderPriority2, orderPriorityReplyCounter: %d, rank: %d,    ",Monitor::getLamport(), this->orderPriorityReplyCounter,this->rank);
        if(this->orderPriorityReplyCounter == 0) {
            this->tryToAcceptOrder();
        }
        return true;
    }

    bool handleFreeOrderPriority(packet_t packet) {
        if(this->availableOrders > 0) this->availableOrders -= 1;
        // printf("%u: handleFreeOrderPriority, availableOrders: %d, rank: %d\n",Monitor::getLamport(), this->availableOrders, this->rank);
        if(this->getIndexFromOrderPriority(packet.status.MPI_SOURCE) < this->bodyList.size()) {
            this->orderPriority.erase(
            this->orderPriority.begin() + 
            this->getIndexFromOrderPriority(packet.status.MPI_SOURCE));
        }
        if(this->gettingResurrectionOrder && this->availableOrders > 0) {
            this->tryToAcceptOrder();
        }
        return true;
    }

    bool handleNewOrder(packet_t packet) {
        this->availableOrders += 1;
        // printf("%u: handleNewOrder, availableOrders: %d, rank: %d\n",Monitor::getLamport(), this->availableOrders, this->rank);
        if(this->gettingResurrectionOrder)  {
            this->tryToAcceptOrder();
        }
        return true;
    }

    bool handleDonePaperwork(packet_t packet) {
        // printf("tail, after paperwork, rank:%d\n",this->rank);
        if(this->resurrecting) {
            this->sendMessage(RESURRECTION_FINISHED, this->rank);
        }
        return true;
    }

    bool handleResurrectionFinished(packet_t packet) {
        printf("%u: --- Dragon resurrected by team head: %d, body: %d, tail: %d ---\n", Monitor::getLamport(), this->headRank, this->bodyRank, this->rank);
        this->sendMessage(RESURRECTION_FINISHED, this->headRank);
        this->sendMessage(RESURRECTION_FINISHED, this->bodyRank);  
        this->sendMessage(RESURRECTION_FINISHED, ISSUER);      
        this->resurrecting = false;
        this->inTeam = false;
        this->headRank = 0;
        this->bodyRank = 0;
        return true;
    }

    bool handleNeedTailPositive(packet_t packet) {
        // printf("%u: Tail specialist %d matched with body %d --->\n",Monitor::getLamport(), this->rank, packet.status.MPI_SOURCE);
        this->bodyRank = packet.status.MPI_SOURCE;
        this->sendMessage(AVENGERS_ASSEMBLED, this->rank);
        this->sendMessage(AVENGERS_ASSEMBLED, this->headRank);
        this->handlingNeedTail = false;
        this->inTeam = true;
        return true;
    }

    bool handleNeedTailNegative(packet_t packet) {
        this->headRank = 0;
        this->handlingNeedTail = false;
        this->inTeam = false;
        return true;
    }

    void tryToAcceptOrder() {
        std::sort(this->orderPriority.begin(), this->orderPriority.end(), comparePacketsLamport);
        unsigned int me = this->getIndexFromOrderPriority(this->rank);
        // printf("%u: tryToAcceptOrder, me: %d , rank: %d\n",Monitor::getLamport(),me, this->rank);
        if(me == 0 && this->availableOrders > 0) { 
            printf("%u: Got the order , rank: %d\n",Monitor::getLamport(), this->rank);
            // this->availableOrders -= 1;
            if(this->availableOrders > 0) this->availableOrders -= 1;
            this->gettingResurrectionOrder = false;
            this->resurrecting = true;
            this->broadcastMessage(FREE_ORDER_PRIORITY, TAIL);
            this->orderPriority.erase(
                this->orderPriority.begin() + me);
            this->beginDragonResurrection();
        }
    }

    void beginDragonResurrection() {
        // printf("beginResurrecting , rank: %d\n",this->rank);
        this->resurrecting = true;
        this->sendMessage(DO_PAPERWORK, this->bodyRank);
    }

};
