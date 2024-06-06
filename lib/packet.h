#ifndef PACKETH
#define PACKETH
#include "main.h"
#include "globals.h"

/* typ pakietu */
typedef struct {
    int type;
    int team[TEAM_SIZE];
    int team_size;
    int src_rank;
} packet_t;

extern MPI_Datatype MPI_PACKET_T;
void packet_init();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);

typedef enum {InRun, InMonitor, InWant, InSection, InFinish} state_t;
/* zmiana stanu, obwarowana muteksem */
void changeState( state_t );
#endif
