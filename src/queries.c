#include "../lib/queries.h"
#include "../lib/main.h"
#include <stdlib.h>

void fill_team(int* packet_team) {
    for (int i = 0; i < TEAM_SIZE; i++) {
        packet_team[i] = team[i];
    }
}

packet_t* getp_req()
{
    packet_t* packet = (packet_t*)malloc(sizeof(packet_t));
    packet->type = REQUEST;
    fill_team(packet->team);
    packet->team_size = team_size;
    packet->src_rank = rank;

    return packet;
}