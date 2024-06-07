#include "../lib/dembiec.h"
#include "../lib/packet.h"
#include "../lib/main.h"

void fun_in_dembiec() {
    while (1) {
        if (rand() % 100 == 0) {
            hp -= 10;
            sleep(10000);
            leave_team();
            in_dembiec = 0;
            break;
        }

        sleep(1000);
        int number_amount;
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_PACKET_T, &number_amount);

		for (int i = 0; i < number_amount; i++) {
			packet_t* packet = getMessage(MPI_ANY_SOURCE, &status);
			handlePacket(packet);
		}
    }
}