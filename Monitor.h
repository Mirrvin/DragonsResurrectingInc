#ifndef MONITOR
#define MONITOR
#include "main.h"

class Monitor {
private:
    static bool listening;
    static unsigned int lamport;
    static pthread_mutex_t lamportMutex;

    static void incrementLamportOnReceive(packet_t packet);

public:
    static int rank;
    static int size;
    static pthread_mutex_t handleMutex;
    static pthread_mutex_t messageQueueMutex;
    static std::queue<packet_t> messageQueue;

    static void initialize();
    static void finalize();
    static void listen();
    static void endListening();
    static unsigned int incrementLamportOnSend();
    static unsigned int getLamport();
};

#endif
