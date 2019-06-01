class Monitor;
#include "../Monitor.h"

class Specialist {
protected:
    Monitor *monitor = nullptr;

public:
    Specialist() { }
    ~Specialist() {
        delete this->monitor;
    }
    void sendMessage() {
        printf("Sending dupa");
    }

    void broadcastMessage() {
        printf("Broadcasting dupa");
    }
    void setMonitor(Monitor *monitor) {
        this->monitor = monitor;
    }
    virtual void handle(packet_t packet, MPI_Status status) = 0;
};
