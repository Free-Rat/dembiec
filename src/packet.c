#include "../lib/main.h"
#include "../lib/packet.h"
MPI_Datatype MPI_PACKET_T;

struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { 
    { "pakiet aplikacyjny", APP_PKT }, 
    { "finish", FINISH }, 
    { "potwierdzenie", ACK }, 
    { "prośbę o sekcję krytyczną", REQUEST }, 
    { "zwolnienie sekcji krytycznej", RELEASE }
};

const char *const tag2string( int tag )
{
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
	if ( tagNames[i].tag == tag )  return tagNames[i].name;
    }
    return "<unknown>";
}

void packet_init()
{
    int       blocklengths[PACKET_ITEMS] = {1, TEAM_SIZE, 1, 1};
    MPI_Datatype types[PACKET_ITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint     offsets[PACKET_ITEMS]; 
    offsets[0] = offsetof(packet_t, type);
    offsets[1] = offsetof(packet_t, team);
    offsets[2] = offsetof(packet_t, team_size);
    offsets[3] = offsetof(packet_t, src_rank);

    MPI_Type_create_struct(PACKET_ITEMS, blocklengths, offsets, types, &MPI_PACKET_T);

    MPI_Type_commit(&MPI_PACKET_T);
}

void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt=0;
    if (pkt == 0) { 
        pkt = malloc(sizeof(packet_t)); 
        freepkt=1;
    }

    pkt->src_rank = rank;
    MPI_Send(pkt, 1, MPI_PACKET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string(tag), destination);
    if (freepkt) 
        free(pkt);
}
