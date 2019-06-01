#include "../main.h"
#include "Specialist.h"

class HeadSpecialist: public Specialist {

public:
    HeadSpecialist() { }

    void handle(packet_t packet, MPI_Status status) override {
        printf("Dupa");
    }

    // void send() {
    //     this->sendMessage();
    // }

    // void broadcast() {
    //     this->broadcastMessage();
    // }
};
