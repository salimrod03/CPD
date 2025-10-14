#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct Nodo {
    int clave;
    struct Nodo* sig;
} Nodo;

Nodo* cabeza = NULL;
pthread_mutex_t mtx;
int num_hilos = 1;
int total_operaciones = 1000000;
double porcentaje_lectura = 0.9;

int buscar(int clave) {
    Nodo* actual = cabeza;
    while (actual != NULL) {
        if (actual->clave == clave)
            return 1;
        actual = actual->sig;
    }
    return 0;
}

int insertar(int clave) {
    if (buscar(clave))
        return 0;
    Nodo* nuevo = malloc(sizeof(Nodo));
    nuevo->clave = clave;
    nuevo->sig = cabeza;
    cabeza = nuevo;
    return 1;
}

int eliminar(int clave) {
    Nodo *actual = cabeza, *prev = NULL;
    while (actual != NULL && actual->clave != clave) {
        prev = actual;
        actual = actual->sig;
    }
    if (actual == NULL)
        return 0;
    if (prev == NULL)
        cabeza = actual->sig;
    else
        prev->sig = actual->sig;
    free(actual);
    return 1;
}

void* tarea_hilo(void* arg) {
    long id = (long)arg;
    int ops = total_operaciones / num_hilos;
    unsigned int semilla = time(NULL) + id * 13;
    for (int i = 0; i < ops; i++) {
        int clave = rand_r(&semilla) % 1000;
        double prob = (double)rand_r(&semilla) / RAND_MAX;
        pthread_mutex_lock(&mtx);
        if (prob < porcentaje_lectura)
            buscar(clave);
        else if (prob < porcentaje_lectura + 0.05)
            insertar(clave);
        else
            eliminar(clave);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc > 1)
        num_hilos = atoi(argv[1]);
    pthread_t hilos[num_hilos];
    pthread_mutex_init(&mtx, NULL);

    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long i = 0; i < num_hilos; i++)
        pthread_create(&hilos[i], NULL, tarea_hilo, (void*)i);
    for (int i = 0; i < num_hilos; i++)
        pthread_join(hilos[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Hilos: %d | Tiempo total: %.6f s\n", num_hilos, tiempo);

    pthread_mutex_destroy(&mtx);
    return 0;
}

