#ifndef SPECIALIST
#define SPECIALIST
#include "../main.h"
#include "../Monitor.h"

class Specialist {
protected:
    int rank;
    int size;
    bool inTeam;
    int resurrectionCounter;
    packet_t sendMessage(int tag, int target);
    packet_t sendMessage(packet_t packet, int tag, int target);
    packet_t broadcastMessage(int tag);
    packet_t broadcastMessage(int tag, int targetType);
    packet_t broadcastMessage(packet_t packet, int tag);
    packet_t broadcastMessage(packet_t packet, int tag, int targetType);
    bool handleEnd(packet_t packet);
    packet_t createSelfPacket();

public:
    Specialist(int rank, int size);
    virtual ~Specialist();
    virtual bool handle(packet_t packet);
};


#endif