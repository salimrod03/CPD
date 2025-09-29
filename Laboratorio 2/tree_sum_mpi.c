#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);

    long long local = rank + 1; 
    long long sum = local;

    int p = 1;
    while(p < size){
        if(rank % (2*p) == 0){
            int src = rank + p;
            if(src < size){
                long long recvv;
                MPI_Recv(&recvv, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum += recvv;
            }
        } else {
            int dst = rank - p;
            MPI_Send(&sum, 1, MPI_LONG_LONG, dst, 0, MPI_COMM_WORLD);
            break;
        }
        p *= 2;
    }

    if(rank==0) printf("Tree-sum result = %lld (expected sum = %lld)\n", sum, (long long)size*(size+1)/2);
    MPI_Finalize();
    return 0;
}

