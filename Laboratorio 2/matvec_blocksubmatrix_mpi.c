#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);

    int N = 8;
    if(argc>1) N = atoi(argv[1]);
    int q = (int)round(sqrt(size));
    if(q*q != size){
        if(rank==0) fprintf(stderr,"Error: number of processes must be a perfect square for this program.\n");
        MPI_Finalize(); return 1;
    }
    int nb = N / q;
    if(N % q != 0){ if(rank==0) fprintf(stderr,"Error: N must be divisible by sqrt(P).\n"); MPI_Finalize(); return 1; }

    int prow = rank / q;
    int pcol = rank % q;

    double *Ablock = malloc(sizeof(double)*nb*nb);
    double *x = malloc(sizeof(double)*N);
    double *y_local = calloc(nb, sizeof(double)); 

    if(rank==0){
        double *A = malloc(sizeof(double)*N*N);
        for(int i=0;i<N;i++) for(int j=0;j<N;j++) A[i*N+j] = (double)(i+j+1);
        for(int j=0;j<N;j++) x[j] = 1.0;

        for(int p=0;p<size;p++){
            int r = p / q;
            int c = p % q;
            double *tmp = malloc(sizeof(double)*nb*nb);
            int w=0;
            for(int i=r*nb;i<r*nb+nb;i++){
                for(int j=c*nb;j<c*nb+nb;j++){
                    tmp[w++] = A[i*N + j];
                }
            }
            if(p==0){
                for(int t=0;t<nb*nb;t++) Ablock[t] = tmp[t];
            } else {
                MPI_Send(tmp, nb*nb, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);
            }
            free(tmp);
        }
        free(A);
    } else {
        MPI_Recv(Ablock, nb*nb, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for(int i_local=0;i_local<nb;i_local++){
        int i_global = prow*nb + i_local;
        double s=0.0;
        for(int j_local=0;j_local<nb;j_local++){
            int j_global = pcol*nb + j_local;
            s += Ablock[i_local*nb + j_local] * x[j_global];
        }
        y_local[i_local] = s; 
    }

    double *y_row = NULL;
    if(pcol==0) y_row = malloc(sizeof(double)*nb);
    MPI_Reduce(y_local, y_row, nb, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(pcol==0){
        if(rank==0){
            double *y_full = malloc(sizeof(double)*N);
            for(int i=0;i<nb;i++) y_full[i] = y_row[i];

            for(int pr=1; pr<q; pr++){
                int src = pr*q + 0;
                MPI_Recv(y_full + pr*nb, nb, MPI_DOUBLE, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            printf("Result y (first 10): ");
            for(int i=0;i< (N<10?N:10); i++) printf("%.2f ", y_full[i]);
            printf("\n");
            free(y_full);
        } else {

            int dest = 0;
            MPI_Send(y_row, nb, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
        }
        free(y_row);
    }

    free(Ablock); free(x); free(y_local);
    MPI_Finalize();
    return 0;
}

