#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int total = 1000000;           
    int n_bins = 10;              
    if(argc>1) total = atoi(argv[1]);
    if(argc>2) n_bins = atoi(argv[2]);

    int *data = NULL;
    int *sendcounts = NULL;
    int *displs = NULL;

    if(rank==0){
        data = malloc(sizeof(int)*total);
        srand(time(NULL));
        for(int i=0;i<total;i++) data[i] = rand()%n_bins;
        sendcounts = malloc(sizeof(int)*size);
        displs = malloc(sizeof(int)*size);
        int base = total/size;
        int rem = total%size;
        int offset=0;
        for(int i=0;i<size;i++){
            sendcounts[i] = base + (i<rem?1:0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    int local_n;
    MPI_Scatter(sendcounts,1,MPI_INT,&local_n,1,MPI_INT,0,MPI_COMM_WORLD);

    int *local_data = malloc(sizeof(int)* (local_n>0?local_n:1) );
    MPI_Scatterv(data, sendcounts, displs, MPI_INT,
                 local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    int *local_hist = calloc(n_bins,sizeof(int));//local
    for(int i=0;i<local_n;i++){
        int v = local_data[i];
        if(v>=0 && v<n_bins) local_hist[v]++;
    }

    int *global_hist = calloc(n_bins,sizeof(int));
    MPI_Reduce(local_hist, global_hist, n_bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank==0){
        printf("Global histogram (bins=%d, total=%d):\n", n_bins, total);
        int sum=0;
        for(int i=0;i<n_bins;i++){ printf("bin %2d : %d\n", i, global_hist[i]); sum+=global_hist[i]; }
        printf("Sum check = %d\n", sum);
    }

    free(local_data); free(local_hist); free(global_hist);
    if(rank==0){ free(data); free(sendcounts); free(displs); }
    MPI_Finalize();
    return 0;
}

