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


// ======================================================================
// zarządzanie ekipami
// ======================================================================

int* scal(int* grupa1, int* grupa2) {
	int* wynik = (int*)malloc(size * sizeof(int));
	int i = 0;
	int j = 0;
	int k = 0;
	while (grupa1[i] != -1 && grupa2[j] != -1) {
		if (grupa1[i] < grupa2[j]) {
			wynik[k] = grupa1[i];
			i++;
		} else {
			wynik[k] = grupa2[j];
			j++;
		}
		k++;
	}
	while (grupa1[i] != -1) {
		wynik[k] = grupa1[i];
		i++;
		k++;
		if (i == size) {
			break;
		}
	}
	while (grupa2[j] != -1) {
		wynik[k] = grupa2[j];
		j++;
		k++;
		if (j == size) {
			break;
		}
	}
	return wynik;
}

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void sort(int* tab) {
	// -1 on the end
	// from 0 to inf from the beginning
	for (int i = 0; i < size; i++) {
		for (int j = size - 1; j > i; j--) {
			if (tab[i] < 0 && tab[j] >= 0) {
				swap(&tab[i], &tab[j]);
			} else if (tab[j] < tab[i] && tab[j] >= 0 && tab[i] >= 0) {
				swap(&tab[i], &tab[j]);
			}
	  	}
	}
}

int* trim(int* g1, int len_g1, int len_g2) {
	if (len_g1 + len_g2 < size) {
		return g1;
	}
	int* wynik = (int*)malloc(size * sizeof(int));
	for (int i = 0; i < size - len_g2; i++) {
		wynik[i] = g1[i];
	}
	for (int i = size - len_g2; i < size; i++) {
		wynik[i] = -1;
	}
	return wynik;
}

void wypisz(int* tab) {
	// int i = 0;
	// while (tab[i] != -1) {
	// 	printf("%d ", tab[i]);
	// 	i++;
	// }
	for (int i = 0; i < size; i++) {
		printf("%d ", tab[i]);
	}
	printf("\n");
}











