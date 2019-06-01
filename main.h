#ifndef MAINH
#define MAINH


//  boolean 
// #define TRUE 1
// #define FALSE 0

//  rodzaje procesów 
#define ROOT 0
#define HEAD 1
#define BODY 2
#define TAIL 3

//  tagi wiadomości 
#define TASK 100

//  parametry 
#define SKELETON_NUMBER 5
#define DESK_NUMBER 4


#include <mpi.h>
#include <stdlib.h>
#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>
#include <algorithm>

// #include "Monitor.cpp"
// #include "specialists/Specialist.cpp"
// #include "specialists/HeadSpecialist.cpp"


//#define FINISH 1
//#define APP_MSG 2
//#define GIVE_YOUR_STATE 3
//#define MY_STATE_IS 4
//  MAX_HANDLERS musi się równać wartości ostatniego typu pakietu + 1 
//#define MAX_HANDLERS 5 



typedef struct {
    int id;
    unsigned int lamport;
} packet_t;

// extern int rank,size;
MPI_Datatype MPI_PAKIET_T;
// extern pthread_t threadCom, threadM;

//  synchro do zmiennej konto 
// extern pthread_mutex_t konto_mut;

//  argument musi być, bo wymaga tego pthreads. Wątek monitora, który po jakimś czasie ma wykryć stan 
// extern void *monitorFunc(void *);
//  argument musi być, bo wymaga tego pthreads. Wątek komunikacyjny 
// extern void *comFunc(void *);

// #define PROB_OF_SENDING 25
// #define PROB_OF_PASSIVE 5
// #define PROB_OF_SENDING_DECREASE 1
// #define PROB_SENDING_LOWER_LIMIT 1
// #define PROB_OF_PASSIVE_INCREASE 1

//  makra do wypisywania na ekranie 
// #define P_WHITE printf("%c[%d;%dm",27,1,37);
// #define P_BLACK printf("%c[%d;%dm",27,1,30);
// #define P_RED printf("%c[%d;%dm",27,1,31);
// #define P_GREEN printf("%c[%d;%dm",27,1,33);
// #define P_BLUE printf("%c[%d;%dm",27,1,34);
// #define P_MAGENTA printf("%c[%d;%dm",27,1,35);
// #define P_CYAN printf("%c[%d;%d;%dm",27,1,36);
// #define P_SET(X) printf("%c[%d;%dm",27,1,31+(6+X)%7);
// #define P_CLR printf("%c[%d;%dm",27,0,37);

// #define println(FORMAT, ...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);

//  macro debug - działa jak printf, kiedy zdefiniowano
//    DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta 
   
//    używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
//    wyświetla rank

//    w związku z tym, zmienna "rank" musi istnieć.

// #ifdef DEBUG
// #define debug(...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);

// #else
// #define debug(...) ;
// #endif
#endif

