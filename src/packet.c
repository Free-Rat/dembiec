#include "../lib/main.h"
#include "../lib/packet.h"
#include "../lib/queries.h"
#include "../lib/dembiec.h"

MPI_Datatype MPI_PACKET_T;

struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { 
    { "prośbę o dołączenie do grupy", REQUEST },
	{ "odpowiedź na dołączenie do grupy", ANSWER },
	{ "aktualizację grupy", UPDATE }
};

const char *const tag2string( int tag )
{
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
		if ( tagNames[i].tag == tag ) {
			return tagNames[i].name;
		}
    }
    return "<unknown>";
}

void packet_init()
{
    int       blocklengths[PACKET_ITEMS] = {1, TEAM_SIZE, 1, 1, 1, 1};
    MPI_Datatype types[PACKET_ITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint     offsets[PACKET_ITEMS]; 
    offsets[0] = offsetof(packet_t, type);
    offsets[1] = offsetof(packet_t, team);
    offsets[2] = offsetof(packet_t, team_size);
    offsets[3] = offsetof(packet_t, src_rank);
	offsets[4] = offsetof(packet_t, answer);
	offsets[5] = offsetof(packet_t, leader_rank);

    MPI_Type_create_struct(PACKET_ITEMS, blocklengths, offsets, types, &MPI_PACKET_T);

    MPI_Type_commit(&MPI_PACKET_T);
}

void sendPacket(packet_t *pkt, int destination, int tag, int free_here)
{
    pkt->src_rank = rank;
	println("Wysyłam %s do %d\n", tag2string(tag), destination);
    MPI_Send(pkt, 1, MPI_PACKET_T, destination, tag, MPI_COMM_WORLD);
    if (free_here) {
        free(pkt);
	}
}

void replace_team(int* new_team) {
	for (int i = 0; i < TEAM_SIZE; i++) {
		team[i] = new_team[i];
	}
}

// ======================================================================
// zarządzanie ekipami
// ======================================================================

void printteam(int* team) {
	for (int i = 0; i < TEAM_SIZE; i++) {
		printf("team[%d] = %d\n", i, team[i]);
	}
}

void validate_team(int* team) {
	for (int i = 0; i < TEAM_SIZE; i++) {
		if (team[i] >= size) {
			println("Błąd w teamie");
			printteam(team);
		}
	}
}

int* merge(int* group1, int* group2) {
	// validate_team(group1);
	// validate_team(group2);
	//printteam(group1);
	//printteam(group2);
	// int* result = (int*)malloc(TEAM_SIZE * sizeof(int));
	// int i = 0;
	// int j = 0;
	// int k = 0;
	// while (group1[i] != -1 && group2[j] != -1) {
	// 	if (group1[i] < group2[j]) {
	// 		result[k] = group1[i];
	// 		i++;
	// 	} else {
	// 		result[k] = group2[j];
	// 		j++;
	// 	}
	// 	k++;
	// }
	// while (group1[i] != -1) {
	// 	result[k] = group1[i];
	// 	i++;
	// 	k++;
	// 	if (i == TEAM_SIZE) {
	// 		break;
	// 	}
	// }
	// while (group2[j] != -1) {
	// 	result[k] = group2[j];
	// 	j++;
	// 	k++;
	// 	if (j == TEAM_SIZE) {
	// 		break;
	// 	}
	// }
	// validate_team(result);
	
	int* result = (int*)malloc(TEAM_SIZE * sizeof(int));
	int i = 0;
	int j = 0;
	int k = 0;

	while (group1[i] != -1) {
		if (k == TEAM_SIZE || i == TEAM_SIZE) {
			break;
		}
		result[k] = group1[i];
		printf("result[%d] = %d\n", k, result[k]);
		i++;
		k++;
	}

	while (group2[j] != -1) {
		if (k == TEAM_SIZE || i == TEAM_SIZE) {
			break;
		}
		result[k] = group2[j];
		printf("result[%d] = %d\n", k, result[k]);
		j++;
		k++;
	}

	while (k < TEAM_SIZE) {
		result[k] = -1;
		k++;
	}

	// print_team(result);
	return result;
}

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void sort(int* tab) {
	// -1 on the end
	// from 0 to inf from the beginning
	for (int i = 0; i < TEAM_SIZE; i++) {
		for (int j = TEAM_SIZE - 1; j > i; j--) {
			if (tab[i] < 0 && tab[j] >= 0) {
				swap(&tab[i], &tab[j]);
			} else if (tab[j] < tab[i] && tab[j] >= 0 && tab[i] >= 0) {
				swap(&tab[i], &tab[j]);
			}
	  	}
	}
}

int* trim(int* g1, int len_g1, int len_g2) {
	// print_team(g1);
	if (len_g1 + len_g2 < TEAM_SIZE) {
		return g1;
	}
	int* wynik = (int*)malloc(TEAM_SIZE * sizeof(int));
	for (int i = 0; i < TEAM_SIZE - len_g2; i++) {
		wynik[i] = g1[i];
	}
	for (int i = TEAM_SIZE - len_g2; i < TEAM_SIZE; i++) {
		wynik[i] = -1;
	}
	// print_team(wynik);
	return wynik;
}

packet_t *getMessage(int from, MPI_Status* status)
{
    packet_t* packet = (packet_t*)malloc(sizeof(packet_t));
    MPI_Recv(packet, 1, MPI_PACKET_T, from, MPI_ANY_TAG, MPI_COMM_WORLD, status);

    return packet;
}

void update_leader() {
	for (int i = 0; i < TEAM_SIZE; i++) {
		if (team[i] != -1) {
			leader = team[i];
			if (leader == rank) {
				is_leader = 1;
			}
			else {
				is_leader = 0;
			}
			break;
		}
	}
}

void team_merge(int* rec_team, int rec_team_size) {

	sort(team);
	sort(rec_team);

	rec_team = trim(rec_team, rec_team_size, team_size);

	int* new_team = merge(team, rec_team);
	replace_team(new_team);
	team_size += rec_team_size;
	if (team_size > TEAM_SIZE) {
		team_size = TEAM_SIZE;
	}
	update_leader();

	free(new_team);
}

void sendTeamPacket(packet_t* packet, int tag) {
	for (int i = 0; i < TEAM_SIZE; i++) {
		// print_team(team);
		if (team[i] > -1 && team[i] != rank) {
			// println("Wysyłam %s do team:%d", tag2string(tag), team[i]);
			sendPacket(packet, team[i], tag, 0);
		}
	}

	free(packet);
}

void leave_team() {
	for (int i = 0; i < TEAM_SIZE; i++) {
		if (team[i] == rank) {
			team[i] = -1;
			team_size--;
			sort(team);
			break;
		}
	}

	sendTeamPacket(getp_update(), UPDATE);
	fill_tab(team, TEAM_SIZE, -1);
}

void try_go_dembiec() {
	if (team_size >= TEAM_SIZE) {
		in_dembiec = 1;
		sendTeamPacket(getp_gogo(), GO_DEMBIEC);
	}
}

void handlePacket(packet_t* packet) {
    switch (packet->type) {
        case REQUEST:
            println("Otrzymałem prośbę od %d", packet->src_rank);
			if (dead_list[rank]) {
				sendPacket(getp_ans(DEAD), packet->src_rank, ANSWER, 1);
				break;
			}
			if (!in_dembiec) {
				if (is_leader) {
					team_merge(packet->team, packet->team_size);
					sendPacket(getp_ans(OK), packet->src_rank, ANSWER, 1);
					sendTeamPacket(getp_update(), UPDATE);
					try_go_dembiec();
				}
				else {
					sendPacket(getp_ans(NOT_LEADER), packet->src_rank, ANSWER, 1);
				}
			}
			else {
				sendPacket(getp_ans(AWAY), packet->src_rank, ANSWER, 1);
			}
            break;
		case ANSWER:
			println("Otrzymałem odpowiedź od %d", packet->src_rank);
			switch (packet->answer) {
				case OK:
					team_merge(packet->team, packet->team_size);
					println("Połączono z %d", packet->src_rank);
					//print_team();
					break;
				case NOT_LEADER:
					next_query = packet->leader_rank - 1;
					break;
				case DEAD:
					dead_list[packet->src_rank] = 1;
					break;
			}
			break;

		case UPDATE:
			if (in_team(packet->src_rank)) {
				replace_team(packet->team);
				update_leader();
				//print_team();
			}
			break;

		case GO_DEMBIEC:
			in_dembiec = 1;
			println("Lecim na dembiec!");
			fun_in_dembiec();
			break;
        default:
            break;
    }

    free(packet);
}
