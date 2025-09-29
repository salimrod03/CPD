#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);

    int N = 8;
    if(argc>1) N = atoi(argv[1]);
    int base = N/size, rem = N%size;
    int *cols_per = malloc(sizeof(int)*size);
    int *col_disp = malloc(sizeof(int)*size);
    int offset=0;
    for(int i=0;i<size;i++){ cols_per[i] = base + (i<rem?1:0); col_disp[i]=offset; offset += cols_per[i]; }

    double *A_block = NULL;
    double *x = malloc(sizeof(double)*N);
    double *y_local = calloc(N,sizeof(double));

    if(rank==0){
        double *A = malloc(sizeof(double)*N*N);
        for(int i=0;i<N;i++) for(int j=0;j<N;j++) A[i*N+j] = (double)(i+j+1);
        for(int j=0;j<N;j++) x[j] = 1.0;

        int *sendcounts = malloc(sizeof(int)*size);
        int *sdispls = malloc(sizeof(int)*size);
        int pos=0;
        for(int p=0;p<size;p++){
            sendcounts[p] = N * cols_per[p];
            sdispls[p] = pos;
            pos += sendcounts[p];
        }
        double *sendbuf = malloc(sizeof(double)*pos);
        int write=0;
        for(int p=0;p<size;p++){
            for(int jj=0; jj<cols_per[p]; jj++){
                int col = col_disp[p] + jj;
                for(int i=0;i<N;i++){
                    sendbuf[write++] = A[i*N + col];
                }
            }
        }
        int recvcount = N * cols_per[rank];
        A_block = malloc(sizeof(double)*recvcount);
        MPI_Scatterv(sendbuf, sendcounts, sdispls, MPI_DOUBLE,
                     A_block, recvcount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(sendbuf); free(sendcounts); free(sdispls); free(A);
    } else {
        int recvcount = N * cols_per[rank];
        A_block = malloc(sizeof(double)*recvcount);
        MPI_Scatterv(NULL, NULL, NULL, MPI_DOUBLE,
                     A_block, recvcount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int local_cols = cols_per[rank];
    int col0 = col_disp[rank];
    for(int jj=0;jj<local_cols;jj++){
        double xval = x[col0 + jj];
        for(int i=0;i<N;i++){
            y_local[i] += A_block[jj*N + i] * xval; 
        }
    }

    double *y_global = NULL;
    if(rank==0) y_global = malloc(sizeof(double)*N);
    MPI_Reduce(y_local, y_global, N, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank==0){
        printf("Result y (first 10 elements):\n");
        for(int i=0;i< (N<10?N:10); i++) printf("%.2f ", y_global[i]);
        printf("\n");
        free(y_global);
    }

    free(A_block); free(x); free(y_local); free(cols_per); free(col_disp);
    MPI_Finalize();
    return 0;
}

