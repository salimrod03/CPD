#include <mpi.h>
#include <stdio.h>

int is_power_of_two(int n){ return (n&(n-1))==0; }

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);

    long long local = rank + 1;
    long long sum = local;

    if(is_power_of_two(size)){
        int d=1;
        while(d < size){
            int partner = rank ^ d;
            long long recvv;
            MPI_Sendrecv(&sum,1,MPI_LONG_LONG,partner,0,
                         &recvv,1,MPI_LONG_LONG,partner,0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += recvv;
            d <<= 1;
        }
        if(rank==0) printf("Butterfly sum (pow2) = %lld\n", sum);
    } else {
        long long total=0;
        MPI_Reduce(&local, &total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        if(rank==0) printf("Fallback MPI_Reduce sum = %lld\n", total);
    }

    MPI_Finalize();
    return 0;
}

