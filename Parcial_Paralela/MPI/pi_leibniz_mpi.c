#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[]) {

    int rank, size;
    long numeroIteraciones;

    double sumaLocal = 0.0;
    double sumaGlobal = 0.0;

    clock_t inicio, fin;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Ejecucion con %d procesos\n", size);
        printf("Ingresar numero de iteraciones: ");
        scanf("%ld", &numeroIteraciones);

        inicio = clock();
    }

    // Enviar iteraciones a todos los procesos
    MPI_Bcast(&numeroIteraciones, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Cada proceso calcula una parte
    for (long i = rank; i < numeroIteraciones; i += size) {

        if (i % 2 == 0) {
            sumaLocal += 4.0 / (2.0 * i + 1.0);
        }
        else {
            sumaLocal -= 4.0 / (2.0 * i + 1.0);
        }
    }

    // Sumar todas las sumas locales
    MPI_Reduce(&sumaLocal, &sumaGlobal,
        1, MPI_DOUBLE, MPI_SUM,
        0, MPI_COMM_WORLD);

    if (rank == 0) {

        fin = clock();

        printf("Valor de PI = %.10f\n", sumaGlobal);

        printf("Tiempo de ejecucion = %f segundos\n",
            (double)(fin - inicio) / CLOCKS_PER_SEC);
    }

    MPI_Finalize();

    return 0;
}