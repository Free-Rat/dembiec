#include "../lib/queries.h"
#include "../lib/main.h"
#include <stdlib.h>

void fill_team(int* packet_team) {
    for (int i = 0; i < TEAM_SIZE; i++) {
        packet_team[i] = team[i];
    }
}

packet_t* getp_req() {
    packet_t* packet = (packet_t*)malloc(sizeof(packet_t));
    packet->type = REQUEST;
    fill_team(packet->team);
    packet->team_size = team_size;
    packet->src_rank = rank;

    return packet;
}

packet_t* getp_ans(int answer_type) {
    packet_t* packet = (packet_t*)malloc(sizeof(packet_t));
    packet->type = ANSWER;
    fill_team(packet->team);
    packet->team_size = team_size;
    packet->src_rank = rank;
    packet->answer = answer_type;
    packet->leader_rank = leader;

    return packet;
}

packet_t* getp_update() {
    packet_t* packet = (packet_t*)malloc(sizeof(packet_t));
    packet->type = UPDATE;
    fill_team(packet->team);
    packet->team_size = team_size;
    packet->src_rank = rank;
    packet->leader_rank = leader;
}