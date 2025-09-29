#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);

    long long total_tosses = 1000000;
    if(rank==0 && argc>1) total_tosses = atoll(argv[1]);
    MPI_Bcast(&total_tosses,1,MPI_LONG_LONG,0,MPI_COMM_WORLD);

    long long base = total_tosses / size;
    long long rem = total_tosses % size;
    long long my_tosses = base + (rank < rem ? 1 : 0);

    unsigned int seed = (unsigned int)time(NULL) ^ (rank*919);
    long long local_in = 0;
    for(long long i=0;i<my_tosses;i++){
        double x = (double)rand_r(&seed)/RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed)/RAND_MAX * 2.0 - 1.0;
        if(x*x + y*y <= 1.0) local_in++;
    }

    long long total_in = 0;
    MPI_Reduce(&local_in, &total_in, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if(rank==0){
        double pi_est = 4.0 * (double)total_in / (double)total_tosses;
        printf("Tosses=%lld  in=%lld  pi_est=%.10f\n", total_tosses, total_in, pi_est);
    }

    MPI_Finalize();
    return 0;
}

