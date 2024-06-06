#include "../lib/main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch ( status.MPI_TAG ) {
	    case REQUEST: 
                debug("Ktoś coś prosi. A niech ma!")
		sendPacket( 0, status.MPI_SOURCE, ACK );
	    break;
	    case ACK: 
                debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
	    break;
	    default:
	    break;
        }
    }
}

void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
}