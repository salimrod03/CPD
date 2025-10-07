#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long long int lanzamientos_totales, dentro_circulo = 0;
int num_hilos;

void* calcular_pi(void* id) {
    long long int lanzamientos_locales = lanzamientos_totales / num_hilos;
    long long int contador_local = 0;
    unsigned int semilla = (unsigned int)time(NULL) ^ (long)id;

    for (long long int i = 0; i < lanzamientos_locales; i++) {
        double x = (double)rand_r(&semilla) / RAND_MAX;
        double y = (double)rand_r(&semilla) / RAND_MAX;
        if (x * x + y * y <= 1.0) contador_local++;
    }
    dentro_circulo += contador_local;

    return NULL;
}

int main(int argc, char* argv[]) {
    struct timespec inicio, fin;

    if (argc != 3) {
        printf("Uso: %s <numero_de_lanzamientos> <num_hilos>\n", argv[0]);
        return -1;
    }

    lanzamientos_totales = atoll(argv[1]);
    num_hilos = atoi(argv[2]);
    pthread_t hilos[num_hilos];

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < num_hilos; i++)
        pthread_create(&hilos[i], NULL, calcular_pi, (void*)(long)i);

    for (int i = 0; i < num_hilos; i++)
        pthread_join(hilos[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &fin);

    double pi = 4.0 * dentro_circulo / lanzamientos_totales;
    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Estimación de Pi con hilos: %.6f\n", pi);
    printf("Tiempo de ejecución: %.6f segundos\n", tiempo);

    return 0;
}

