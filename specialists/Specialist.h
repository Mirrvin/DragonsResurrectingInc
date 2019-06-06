#ifndef SPECIALIST
#define SPECIALIST
#include "../main.h"

class Specialist {
protected:
    int rank;
    int size;
    bool inTeam;
    int resurrectionCounter;
    void sendMessage(int tag, int target);
    void sendMessage(packet_t packet, int tag, int target);
    void broadcastMessage(int tag);
    void broadcastMessage(int tag, int targetType);
    void broadcastMessage(packet_t packet, int tag);
    void broadcastMessage(packet_t packet, int tag, int targetType);
    bool handleEnd(packet_t packet);

public:
    Specialist(int rank, int size);
    virtual ~Specialist();
    virtual bool handle(packet_t packet);
};


#endif