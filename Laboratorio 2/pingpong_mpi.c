#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int my_rank, partner;
    int tag = 0;
    int n = 1000;  
    int message = 0;
    double start, finish, total_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (MPI_Comm_size(MPI_COMM_WORLD, &partner) != MPI_SUCCESS) {
        printf("Este programa requiere exactamente 2 procesos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (my_rank == 0) {
        partner = 1;
        start = MPI_Wtime();
        for (int i = 0; i < n; i++) {
            message = i;
            MPI_Send(&message, 1, MPI_INT, partner, tag, MPI_COMM_WORLD);
            MPI_Recv(&message, 1, MPI_INT, partner, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        finish = MPI_Wtime();
        total_time = finish - start;
        printf("Tiempo total para %d ping-pongs: %f segundos\n", n, total_time);
        printf("Tiempo promedio por ping-pong: %e segundos\n", total_time / n);
    } else if (my_rank == 1) {
        partner = 0;
        for (int i = 0; i < n; i++) {
            MPI_Recv(&message, 1, MPI_INT, partner, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&message, 1, MPI_INT, partner, tag, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}

