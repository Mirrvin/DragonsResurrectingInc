#ifndef MAINH
#define MAINH

//  rodzaje procesów 
#define ROOT 0
#define HEAD 1
#define BODY 2
#define TAIL 3

//  tagi wiadomości 
#define TASK 100
#define END 101
#define NEED_BODY 102
#define NEED_TAIL 103
#define AVENGERS_ASSEMBLE 104


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

bool end = false;

typedef struct {
    int id;
    int ressurectCounter;
    unsigned int lamport;
    MPI_Status status;
} packet_t;

// MPI_Datatype MPI_PAKIET_T;

#endif

