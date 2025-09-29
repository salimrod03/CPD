#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmpint(const void*a,const void*b){ int ia=*(int*)a, ib=*(int*)b; return (ia>ib)-(ia<ib); }

int *merge_arrays(int *a,int na,int *b,int nb){
    int *r = malloc(sizeof(int)*(na+nb));
    int i=0,j=0,k=0;
    while(i<na && j<nb){
        if(a[i]<=b[j]) r[k++]=a[i++]; else r[k++]=b[j++];
    }
    while(i<na) r[k++]=a[i++];
    while(j<nb) r[k++]=b[j++];
    return r;
}

int is_power_of_two(int n){ return (n&(n-1))==0; }

int main(int argc,char**argv){
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);

    int local_n = 1000;
    if(argc>1) local_n = atoi(argv[1]);

    int *local = malloc(sizeof(int)*local_n);
    unsigned int seed = time(NULL) ^ (rank*1337);
    for(int i=0;i<local_n;i++) local[i] = rand_r(&seed) % 1000000;
    qsort(local, local_n, sizeof(int), cmpint);

    if(is_power_of_two(size)){
        int step = 1;
        int current_n = local_n;
        int *current = local;
        while(step < size){
            if(rank % (2*step) == 0){
                int src = rank + step;
                if(src < size){
                    int recv_n;
                    MPI_Recv(&recv_n,1,MPI_INT,src,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                    int *recvbuf = malloc(sizeof(int)*recv_n);
                    MPI_Recv(recvbuf, recv_n, MPI_INT, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    int *merged = merge_arrays(current, current_n, recvbuf, recv_n);
                    free(current); free(recvbuf);
                    current = merged; current_n += recv_n;
                }
            } else {
                int dst = rank - step;
                MPI_Send(&current_n,1,MPI_INT,dst,0,MPI_COMM_WORLD);
                MPI_Send(current, current_n, MPI_INT, dst, 1, MPI_COMM_WORLD);
                free(current);
                break;
            }
            step *= 2;
        }
        if(rank==0){
            printf("Root has sorted %d elements.\n", current_n);
            for(int i=0;i< (current_n<10?current_n:10); i++) printf("%d ", current[i]);
            printf("\n");
            free(current);
        }
    } else {
        int *counts = NULL; int *displs = NULL;
        if(rank==0){ counts = malloc(sizeof(int)*size); displs = malloc(sizeof(int)*size); }
        int ln = local_n;
        MPI_Gather(&ln,1,MPI_INT,counts,1,MPI_INT,0,MPI_COMM_WORLD);
        if(rank==0){
            int total=0; for(int i=0;i<size;i++){ displs[i]=total; total+=counts[i]; }
            int *all = malloc(sizeof(int)*total);
            MPI_Gatherv(local, local_n, MPI_INT, all, counts, displs, MPI_INT, 0, MPI_COMM_WORLD);
            qsort(all, total, sizeof(int), cmpint);
            printf("Fallback root sorted %d elements (gather+sort)\n", total);
            free(all); free(counts); free(displs);
        } else {
            MPI_Gatherv(local, local_n, MPI_INT, NULL, NULL, NULL, MPI_INT, 0, MPI_COMM_WORLD);
        }
        free(local);
    }

    MPI_Finalize();
    return 0;
}

