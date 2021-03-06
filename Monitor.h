#ifndef MONITOR
#define MONITOR
#include "main.h"

class Monitor {
private:
    static bool listening;
    static unsigned int lamport;
    static pthread_mutex_t lamportMutex;


public:
    static int currentSkeletons;
    static int rank;
    static int size;
    static pthread_mutex_t handleMutex;
    static pthread_mutex_t messageQueueMutex;
    static pthread_mutex_t skeletonsMutex;
    static pthread_mutex_t newOrderMutex;
    static std::queue<packet_t> messageQueue;

    static void initialize();
    static void finalize();
    static packet_t receiveMessage();
    static void listen();
    static void endListening();
    static void incrementLamportOnReceive(packet_t packet);
    static unsigned int incrementLamportOnSend();
    static unsigned int getLamport();
};

#endif
