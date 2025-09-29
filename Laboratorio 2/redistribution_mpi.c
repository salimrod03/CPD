#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);

    int N = 100000;
    if(argc>1) N = atoi(argv[1]);
    int *vec = NULL;
    if(rank==0){
        vec = malloc(sizeof(int)*N);
        for(int i=0;i<N;i++) vec[i]=i;
    }

    int base = N/size, rem = N%size;
    int *sendcounts = malloc(sizeof(int)*size);
    int *displs = malloc(sizeof(int)*size);
    int off=0;
    for(int i=0;i<size;i++){
        sendcounts[i] = base + (i<rem?1:0);
        displs[i] = off;
        off += sendcounts[i];
    }
    int my_n = sendcounts[rank];
    int *my_block = malloc(sizeof(int)*my_n);
    MPI_Scatterv(vec, sendcounts, displs, MPI_INT,
                 my_block, my_n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();
    int *sends = malloc(sizeof(int)*size);
    int *sdis = malloc(sizeof(int)*size);
    int *recvs = malloc(sizeof(int)*size);
    int *rdis = malloc(sizeof(int)*size);
    for(int p=0;p<size;p++) sends[p]=0;
    for(int i=0;i<my_n;i++){
        int global_idx = displs[rank] + i;
        int target = global_idx % size;
        sends[target]++;
    }
    sdis[0]=0;
    for(int p=1;p<size;p++) sdis[p]=sdis[p-1]+sends[p-1];
    int total_send = 0; for(int p=0;p<size;p++) total_send += sends[p];
    int *sendbuf = malloc(sizeof(int)*total_send);
    int *tmpc = malloc(sizeof(int)*size);
    for(int p=0;p<size;p++) tmpc[p]=0;
    for(int i=0;i<my_n;i++){
        int g = displs[rank] + i;
        int tgt = g % size;
        sendbuf[sdis[tgt] + tmpc[tgt]++] = my_block[i];
    }
    
    MPI_Alltoall(sends,1,MPI_INT,recvs,1,MPI_INT,MPI_COMM_WORLD);
    rdis[0]=0;
    for(int p=1;p<size;p++) rdis[p]=rdis[p-1]+recvs[p-1];
    int total_recv = 0; for(int p=0;p<size;p++) total_recv += recvs[p];
    int *recvbuf = malloc(sizeof(int)*total_recv);
    MPI_Alltoallv(sendbuf, sends, sdis, MPI_INT, recvbuf, recvs, rdis, MPI_INT, MPI_COMM_WORLD);
    double t1 = MPI_Wtime();
    double tblock2cyc = t1-t0;

    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = MPI_Wtime();
    
    for(int p=0;p<size;p++) sends[p]=0;
    for(int i=0;i<total_recv;i++){
        int val = recvbuf[i];
        int target = val / (N/size + ( (val % size) < (N%size) ? 1:0 )); 
        int owner=0;
        for(int p=0;p<size;p++){
            if(val >= displs[p] && val < displs[p]+sendcounts[p]){ owner=p; break; }
        }
        sends[owner]++;
    }
    sdis[0]=0; for(int p=1;p<size;p++) sdis[p]=sdis[p-1]+sends[p-1];
    total_send=0; for(int p=0;p<size;p++) total_send+=sends[p];
    int *sendbuf2 = malloc(sizeof(int)*total_send);
    for(int p=0;p<size;p++) tmpc[p]=0;
    for(int i=0;i<total_recv;i++){
        int val = recvbuf[i];
        int owner=0;
        for(int p=0;p<size;p++){
            if(val >= displs[p] && val < displs[p]+sendcounts[p]){ owner=p; break; }
        }
        sendbuf2[sdis[owner] + tmpc[owner]++] = val;
    }
    MPI_Alltoall(sends,1,MPI_INT,recvs,1,MPI_INT,MPI_COMM_WORLD);
    rdis[0]=0; for(int p=1;p<size;p++) rdis[p]=rdis[p-1]+recvs[p-1];
    total_recv=0; for(int p=0;p<size;p++) total_recv+=recvs[p];
    int *recvbuf2 = malloc(sizeof(int)*total_recv);
    MPI_Alltoallv(sendbuf2, sends, sdis, MPI_INT, recvbuf2, recvs, rdis, MPI_INT, MPI_COMM_WORLD);
    double t3 = MPI_Wtime();
    double tcyc2block = t3-t2;

    if(rank==0){
        printf("Block->Cyclic time = %f sec\n", tblock2cyc);
        printf("Cyclic->Block time = %f sec\n", tcyc2block);
    }

    free(vec); free(sendcounts); free(displs);
    free(my_block); free(sends); free(sdis); free(recvs); free(rdis);
    free(sendbuf); free(recvbuf); free(sendbuf2); free(recvbuf2); free(tmpc);
    MPI_Finalize();
    return 0;
}

