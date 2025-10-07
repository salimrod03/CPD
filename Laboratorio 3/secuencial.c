#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char* argv[]) {
    long long int lanzamientos, dentro_circulo = 0;
    double x, y, distancia, pi;
    struct timespec inicio, fin;

    srand(time(NULL));

    if (argc != 2) {
        printf("Uso: %s <numero_de_lanzamientos>\n", argv[0]);
        return -1;
    }

    lanzamientos = atoll(argv[1]);
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long long int i = 0; i < lanzamientos; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        distancia = x * x + y * y;
        if (distancia <= 1.0) dentro_circulo++;
    }

    pi = 4.0 * dentro_circulo / lanzamientos;
    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Estimación de Pi noramlito: %.6f\n", pi);
    printf("Tiempo de ejecución: %.6f segundos\n", tiempo);

    return 0;
}

