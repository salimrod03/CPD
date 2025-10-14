#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int N = 1024;
int num_hilos = 1;
double **matriz, *vector, *resultado;

void* multiplicar(void* arg) {
    long id = (long)arg;
    int inicio = id * N / num_hilos;
    int fin = (id + 1) * N / num_hilos;
    for (int i = inicio; i < fin; i++) {
        resultado[i] = 0;
        for (int j = 0; j < N; j++)
            resultado[i] += matriz[i][j] * vector[j];
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc > 1) N = atoi(argv[1]);
    if (argc > 2) num_hilos = atoi(argv[2]);
    matriz = malloc(N * sizeof(double*));
    for (int i = 0; i < N; i++)
        matriz[i] = malloc(N * sizeof(double));
    vector = malloc(N * sizeof(double));
    resultado = malloc(N * sizeof(double));

    for (int i = 0; i < N; i++) {
        vector[i] = 1.0;
        for (int j = 0; j < N; j++)
            matriz[i][j] = (double)(i + j) / N;
    }

    pthread_t hilos[num_hilos];
    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long i = 0; i < num_hilos; i++)
        pthread_create(&hilos[i], NULL, multiplicar, (void*)i);
    for (int i = 0; i < num_hilos; i++)
        pthread_join(hilos[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;

    double suma = 0;
    for (int i = 0; i < N; i++) suma += resultado[i];
    printf("N: %d | Hilos: %d | Tiempo: %.6f s | Suma resultado: %.2f\n", N, num_hilos, tiempo, suma);

    for (int i = 0; i < N; i++) free(matriz[i]);
    free(matriz); free(vector); free(resultado);
    return 0;
}

