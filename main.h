#ifndef MAINH
#define MAINH

//  rodzaje procesów 
#define ISSUER 0
#define HEAD 1
#define BODY 2
#define TAIL 3

//  tagi wiadomości 
#define TASK 100
#define END 101
#define NEED_BODY 102
#define NEED_TAIL 103
#define AVENGERS_ASSEMBLE 104
#define ACK_NEED_BODY 105
#define FINISH_NEED_BODY 106
#define NEED_BODY_POSITIVE 107
#define NEED_BODY_NEGATIVE 108


//  parametry 
#define SKELETON_NUMBER 5
#define DESK_NUMBER 4


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

inline bool operator> (const packet_t &a, const packet_t &b) {
    return a.data >= b.data && a.lamport >= b.lamport && a.status.MPI_SOURCE > b.status.MPI_SOURCE;
}
inline bool comparePackets(const packet_t &a, const packet_t &b) {
    return a.data <= b.data && a.lamport <= b.lamport && a.status.MPI_SOURCE < b.status.MPI_SOURCE;
}

#endif

