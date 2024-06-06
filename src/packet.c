#include "../lib/main.h"
#include "../lib/packet.h"
#include "../lib/queries.h"

MPI_Datatype MPI_PACKET_T;

struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { 
    { "prośbę o sekcję krytyczną", REQUEST }, 
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


// ======================================================================
// zarządzanie ekipami
// ======================================================================

int* merge(int* group1, int* group2) {
	int* result = (int*)malloc(TEAM_SIZE * sizeof(int));
	int i = 0;
	int j = 0;
	int k = 0;
	while (group1[i] != -1 && group2[j] != -1) {
		if (group1[i] < group2[j]) {
			result[k] = group1[i];
			i++;
		} else {
			result[k] = group2[j];
			j++;
		}
		k++;
	}
	while (group1[i] != -1) {
		result[k] = group1[i];
		i++;
		k++;
		if (i == TEAM_SIZE) {
			break;
		}
	}
	while (group2[j] != -1) {
		result[k] = group2[j];
		j++;
		k++;
		if (j == TEAM_SIZE) {
			break;
		}
	}
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
	return wynik;
}

packet_t *getMessage(int from, MPI_Status* status)
{
    packet_t* packet = (packet_t*)malloc(sizeof(packet_t));
    MPI_Recv(packet, 1, MPI_PACKET_T, from, MPI_ANY_TAG, MPI_COMM_WORLD, status);

    return packet;
}

void handlePacket(packet_t* packet) {
    switch (packet->type) {
        case REQUEST:
            println("Otrzymałem prośbę od %d", packet->src_rank);
			if (!in_dembiec) {
				if (is_leader) {
					sendPacket(getp_ans(OK), packet->src_rank, ANSWER);
				}
				else {
					sendPacket(getp_ans(NOT_LEADER), packet->src_rank, ANSWER);
				}
			}
			else {
				sendPacket(getp_ans(AWAY), packet->src_rank, ANSWER);
			}
            break;
		case ANSWER:
			println("Otrzymałem odpowiedź od %d", packet->src_rank);
			break;
        default:
            break;
    }

    free(packet);
}
