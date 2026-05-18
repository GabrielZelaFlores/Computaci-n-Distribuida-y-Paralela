#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main() {

    int num_elementos;
    int i;

    printf("Ingrese numero de elementos: ");
    scanf("%d", &num_elementos);

    float* numeros = (float*)malloc(num_elementos * sizeof(float));

    srand(time(NULL));

    // Generar números aleatorios
    for (i = 0; i < num_elementos; i++) {
        numeros[i] = (float)rand() / RAND_MAX;
    }

    float suma = 0.0;

    double inicio = omp_get_wtime();

    // Suma paralela
    #pragma omp parallel for reduction(+:suma)
    for (i = 0; i < num_elementos; i++) {
        suma += numeros[i];
    }

    double fin = omp_get_wtime();

    float promedio = suma / num_elementos;

    printf("Suma total = %f\n", suma);
    printf("Promedio = %f\n", promedio);

    printf("Tiempo de ejecucion = %f segundos\n", fin - inicio);

    free(numeros);

    return 0;
}