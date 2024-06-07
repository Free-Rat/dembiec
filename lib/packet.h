#ifndef PACKETH
#define PACKETH
#include "globals.h"
#include <mpi.h>

/* typ pakietu */
typedef struct {
    int type;
    int team[TEAM_SIZE];
    int team_size;
    int src_rank;
    int answer;
    int leader_rank;
} packet_t;

extern MPI_Datatype MPI_PACKET_T;
void packet_init();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag, int free_here);

packet_t* getMessage(int from, MPI_Status* status);

void leave_team();

void fun_in_dembiec();


void handlePacket(packet_t* packet);

typedef enum {InRun, InMonitor, InWant, InSection, InFinish} state_t;
/* zmiana stanu, obwarowana muteksem */
void changeState( state_t );
#endif
