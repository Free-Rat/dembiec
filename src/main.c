#include "../lib/main.h"
#include "../lib/queries.h"

int rank, size;
int next_query = 0;
int is_leader = 1;
int leader = 0;
int team[TEAM_SIZE];
int team_size = 1;

int in_dembiec = 0;
int* dead_list;
int hp = 100;

int all_dead = 0;

MPI_Status status;


void get_next_query() {
    next_query++;
    
    if (next_query == size) {
        next_query = rank + 1;
    }
}

int in_team(int rank_check) {
    for (int i = 0; i < TEAM_SIZE; i++) {
        if (team[i] == rank_check) {
            return 1;
        }
    }

    return 0;
}

int try_next() {
    get_next_query();

    int checked = 0;
    while ((checked <= size && dead_list[next_query] == 1) || in_team(next_query)) {
        get_next_query();
        checked++;
    }

    if (checked <= size) {
        return 1;
    }
    
    return 0;
}

int are_all_dead() {
    for (int i = 0; i < TEAM_SIZE; i++) {
        if (dead_list[i] == 0) {
            return 0;
        }
    }

    return 1;
}

void print_team() {
    printnoln("Aktualna drużyna: ");
    for (int i = 0; i < TEAM_SIZE; i++) {
        printnoln("%d ", team[i]);
    }
    printf("\n");
}

void fill_tab(int* tab, int s, int val) {
    for (int i = 0; i < s; i++) {
        tab[i] = val;
    }
}

int main(int argc, char **argv)
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    srand(rank);
    packet_init(); // tworzy typ pakietu
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    dead_list = (int*)malloc(size * sizeof(int));
    fill_tab(dead_list, size, 0);
    next_query = rank;
    fill_tab(team, TEAM_SIZE, -1);
    team[0] = rank;
	for (int i = 1; i < TEAM_SIZE; i++) {
		team[i] = -1;
	}
    leader = rank;

    //print_team();

    while (1) {
        if (is_leader && rank != size - 1) {
            if (!try_next()) {
                break;
            }
            sendPacket(getp_req(), next_query, REQUEST, 1);
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

        if (are_all_dead()) {
            break;
        }
    }

    println("Wszyscy są martwi, kończę działanie...");
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
    return 0;
}
