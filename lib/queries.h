#ifndef QUERIESH
#define QUERIESH

#include "packet.h"

packet_t* getp_req();

packet_t* getp_ans(int answer_type);

packet_t* getp_update();

packet_t* getp_gogo();

#endif