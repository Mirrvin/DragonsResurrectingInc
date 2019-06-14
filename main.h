#ifndef MAINH
#define MAINH

//  rodzaje procesów 
#define ISSUER 0
#define HEAD 1
#define BODY 2
#define TAIL 3

//  tagi wiadomości 
#define NEW_ORDER 100
#define END 101
#define NEED_BODY 102
#define NEED_TAIL 103
#define AVENGERS_ASSEMBLE 104
#define NEED_BODY_ACK 105
#define FINISH_NEED_BODY 106
#define NEED_BODY_POSITIVE 107
#define NEED_BODY_NEGATIVE 108
#define NEED_TAIL_ACK 109
#define FINISH_NEED_TAIL 110
#define NEED_TAIL_POSITIVE 111
#define NEED_TAIL_NEGATIVE 112
#define AVENGERS_ASSEMBLED 113
#define REQUEST_ORDER_PRIORITY 114
#define REPLY_ORDER_PRIORITY 115
#define FREE_ORDER_PRIORITY 116
#define DO_PAPERWORK 117
#define DONE_PAPERWORK 118
#define RESURRECTION_FINISHED 119
#define NEED_DESK_REQUEST 120
#define NEED_DESK_REPLY 121
#define FREE_DESK 122

//  parametry 
#define SKELETON_NUMBER 2
#define DESK_NUMBER 1
#define ORDER_NUMBER 5


#include <mpi.h>
#include <stdlib.h>
#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <queue>
#include <vector>


typedef struct {
    int data;
    int ressurectCounter;
    unsigned int lamport;
    MPI_Status status;
} packet_t;

inline bool comparePackets(const packet_t &a, const packet_t &b) {
    return (a.ressurectCounter < b.ressurectCounter) ||
           ((a.ressurectCounter == b.ressurectCounter) && (a.lamport < b.lamport)) ||
           ((a.ressurectCounter == b.ressurectCounter) && (a.lamport == b.lamport) && (a.status.MPI_SOURCE < b.status.MPI_SOURCE));
}

inline bool comparePacketsLamport(const packet_t &a, const packet_t &b) {
    return a.lamport < b.lamport;
}

#endif

