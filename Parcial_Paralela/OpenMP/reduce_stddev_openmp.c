#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
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

    // Calcular suma
    #pragma omp parallel for reduction(+:suma)
    for (i = 0; i < num_elementos; i++) {
        suma += numeros[i];
    }

    float promedio = suma / num_elementos;

    // Calcular diferencias cuadradas
    float suma_diferencias = 0.0;

    #pragma omp parallel for reduction(+:suma_diferencias)
    for (i = 0; i < num_elementos; i++) {

        float diferencia = numeros[i] - promedio;

        suma_diferencias += diferencia * diferencia;
    }

    float desviacion = sqrt(suma_diferencias / num_elementos);

    double fin = omp_get_wtime();

    printf("Promedio = %f\n", promedio);

    printf("Desviacion estandar = %f\n", desviacion);

    printf("Tiempo de ejecucion = %f segundos\n", fin - inicio);

    free(numeros);

    return 0;
}