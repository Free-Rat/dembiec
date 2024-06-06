#include "../lib/main.h"
#include "../lib/queries.h"

int rank, size;
int next_query = 0;
int is_leader = 1;
int team[TEAM_SIZE] = {-1};
int team_size = 1;

void get_next_query() {
    next_query++;
    if (next_query == size) {
        next_query = rank;
    }
}

int main(int argc, char **argv)
{
    MPI_Status status;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    srand(rank);
    packet_init(); // tworzy typ pakietu
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    next_query = rank;
    team[0] = rank;

    while (1) {
        if (is_leader) {
            get_next_query();
            sendPacket(getp_req(), next_query, REQUEST);
        }
        if (rank != 0) {
            packet_t* packet = getMessage(MPI_ANY_SOURCE, &status);
            handlePacket(packet);
        }        
    }

    //while (1)
    //println("%d", rank);

    //packet_t packet = {1, {1, 2, 3}, 3, rank};
    //sendPacket(&packet, rank+1, REQUEST);
    //MPI_Recv(&packet, 1, MPI_PACKET_T, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    //println("Recived from %d", packet.team[1]);
    
    //finalizuj();
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
    return 0;
}