#ifndef SPECIALIST
#define SPECIALIST
#include "../main.h"

class Specialist {
protected:
    int rank;
    int size;
    void sendMessage(int tag, int target);
    void sendMessage(packet_t packet, int tag, int target);
    void broadcastMessage(int tag);
    void broadcastMessage(packet_t packet, int tag);
    bool handleEnd(packet_t packet);

public:
    Specialist(int rank, int size);
    virtual ~Specialist();
    virtual bool handle(packet_t packet);
};


#endif