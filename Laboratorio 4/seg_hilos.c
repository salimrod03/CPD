#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long contador = 0;
pthread_mutex_t mtx;
int num_hilos = 1;
long iteraciones = 10000000;

void* sin_seguridad(void* arg) {
    long ops = iteraciones / num_hilos;
    for (long i = 0; i < ops; i++)
        contador++;
    return NULL;
}

void* con_mutex(void* arg) {
    long ops = iteraciones / num_hilos;
    for (long i = 0; i < ops; i++) {
        pthread_mutex_lock(&mtx);
        contador++;
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

void* con_atomico(void* arg) {
    long ops = iteraciones / num_hilos;
    for (long i = 0; i < ops; i++)
        __sync_fetch_and_add(&contador, 1);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc > 1)
        num_hilos = atoi(argv[1]);
    pthread_t hilos[num_hilos];
    pthread_mutex_init(&mtx, NULL);

    struct timespec inicio, fin;
    double tiempo;

    printf("Modo 1: Sin seguridad\n");
    contador = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (long i = 0; i < num_hilos; i++)
        pthread_create(&hilos[i], NULL, sin_seguridad, (void*)i);
    for (int i = 0; i < num_hilos; i++)
        pthread_join(hilos[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &fin);
    tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Contador: %ld | Tiempo: %.6f s\n", contador, tiempo);

    printf("\nModo 2: Con mutex\n");
    contador = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (long i = 0; i < num_hilos; i++)
        pthread_create(&hilos[i], NULL, con_mutex, (void*)i);
    for (int i = 0; i < num_hilos; i++)
        pthread_join(hilos[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &fin);
    tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Contador: %ld | Tiempo: %.6f s\n", contador, tiempo);

    printf("\nModo 3: Con variable atómica\n");
    contador = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (long i = 0; i < num_hilos; i++)
        pthread_create(&hilos[i], NULL, con_atomico, (void*)i);
    for (int i = 0; i < num_hilos; i++)
        pthread_join(hilos[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &fin);
    tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Contador: %ld | Tiempo: %.6f s\n", contador, tiempo);

    pthread_mutex_destroy(&mtx);
    return 0;
}

