#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct Nodo {
    int clave;
    struct Nodo* sig;
    pthread_mutex_t mtx;
} Nodo;

Nodo* cabeza = NULL;
int num_hilos = 1;
int total_operaciones = 1000000;
double porcentaje_lectura = 0.9;

int buscar(int clave) {
    Nodo* actual = cabeza;
    while (actual != NULL) {
        pthread_mutex_lock(&actual->mtx);
        if (actual->clave == clave) {
            pthread_mutex_unlock(&actual->mtx);
            return 1;
        }
        Nodo* siguiente = actual->sig;
        pthread_mutex_unlock(&actual->mtx);
        actual = siguiente;
    }
    return 0;
}

int insertar(int clave) {
    Nodo* nuevo = malloc(sizeof(Nodo));
    nuevo->clave = clave;
    pthread_mutex_init(&nuevo->mtx, NULL);
    pthread_mutex_lock(&nuevo->mtx);
    nuevo->sig = cabeza;
    cabeza = nuevo;
    pthread_mutex_unlock(&nuevo->mtx);
    return 1;
}

int eliminar(int clave) {
    Nodo *actual = cabeza, *prev = NULL;
    while (actual != NULL) {
        pthread_mutex_lock(&actual->mtx);
        if (actual->clave == clave) {
            if (prev)
                prev->sig = actual->sig;
            else
                cabeza = actual->sig;
            pthread_mutex_unlock(&actual->mtx);
            pthread_mutex_destroy(&actual->mtx);
            free(actual);
            return 1;
        }
        if (prev)
            pthread_mutex_unlock(&prev->mtx);
        prev = actual;
        actual = actual->sig;
    }
    if (prev)
        pthread_mutex_unlock(&prev->mtx);
    return 0;
}

void* tarea_hilo(void* arg) {
    long id = (long)arg;
    int ops = total_operaciones / num_hilos;
    unsigned int semilla = time(NULL) + id * 13;
    for (int i = 0; i < ops; i++) {
        int clave = rand_r(&semilla) % 1000;
        double prob = (double)rand_r(&semilla) / RAND_MAX;
        if (prob < porcentaje_lectura)
            buscar(clave);
        else if (prob < porcentaje_lectura + 0.05)
            insertar(clave);
        else
            eliminar(clave);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc > 1)
        num_hilos = atoi(argv[1]);
    pthread_t hilos[num_hilos];

    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long i = 0; i < num_hilos; i++)
        pthread_create(&hilos[i], NULL, tarea_hilo, (void*)i);
    for (int i = 0; i < num_hilos; i++)
        pthread_join(hilos[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Hilos: %d | Tiempo total: %.6f s\n", num_hilos, tiempo);
    return 0;
}

