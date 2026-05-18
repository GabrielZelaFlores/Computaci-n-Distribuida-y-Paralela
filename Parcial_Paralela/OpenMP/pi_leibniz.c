#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main() {

    int numeroHilos;
    clock_t tiempo_inicio, tiempo_final;

    numeroHilos = omp_get_max_threads();
    omp_set_num_threads(numeroHilos);

    double respuesta = 0.0;

    long numeroIteraciones;

    printf("Ejecución con %d threads:\n", numeroHilos);
    printf("Ingresar el numero de iteraciones: ");
    scanf("%ld", &numeroIteraciones);

    // Memoria dinámica
    double* sumasParciales = (double*)malloc(numeroHilos * sizeof(double));

    tiempo_inicio = clock();

    #pragma omp parallel
    {
        int idHilo = omp_get_thread_num();

        sumasParciales[idHilo] = 0.0;

        for (long indice = idHilo; indice < numeroIteraciones; indice += numeroHilos) {

            if (indice % 2 == 0) {
                sumasParciales[idHilo] += 4.0 / (2.0 * indice + 1.0);
            }
            else {
                sumasParciales[idHilo] -= 4.0 / (2.0 * indice + 1.0);
            }
        }
    }

    tiempo_final = clock();

    for (int indice = 0; indice < numeroHilos; indice++) {
        respuesta += sumasParciales[indice];
    }

    printf("La respuesta es: %.10f\n", respuesta);

    printf("Tiempo de ejecución: %f segundos\n",
        (double)(tiempo_final - tiempo_inicio) / CLOCKS_PER_SEC);

    free(sumasParciales);

    return 0;
}