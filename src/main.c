#include "../lib/main.h"

int rank, size;
int next_query = 0;

int main(int argc, char **argv)
{
    MPI_Status status;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    srand(rank);
    packet_init(); // tworzy typ pakietu
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (1)
    println("%d", rank);
    
    //finalizuj();
    return 0;
}

