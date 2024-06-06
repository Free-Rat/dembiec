#include "../lib/main.h"
#include "../lib/queries.h"

int rank, size;
int next_query = 0;
int is_leader = 1;
int leader = 0;
int team[TEAM_SIZE] = {-1};
int team_size = 1;

int in_dembiec = 0;

void get_next_query() {
    next_query++;
    if (next_query == size) {
        next_query = rank + 1;
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
    leader = rank;

    while (1) {
        if (is_leader && rank != size - 1) {
            get_next_query();
            sendPacket(getp_req(), next_query, REQUEST);
            if (rank != 0) {
                packet_t* packet = getMessage(next_query, &status);
                handlePacket(packet);
            }
        }

		int number_amount;
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_PACKET_T, &number_amount);

		for (int i = 0; i < number_amount; i++) {
			packet_t* packet = getMessage(MPI_ANY_SOURCE, &status);
			handlePacket(packet);
		}
    }

    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
    return 0;
}
