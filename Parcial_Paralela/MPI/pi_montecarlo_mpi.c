#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]) {

    int rank, size;

    long muestras;

    long contadorLocal = 0;
    long contadorGlobal = 0;

    double x, y, pi;

    clock_t inicio, fin;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {

        printf("Ejecucion con %d procesos\n", size);

        printf("Ingrese numero de muestras: ");
        scanf("%ld", &muestras);

        inicio = clock();
    }

    // Compartir muestras
    MPI_Bcast(&muestras, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Semilla distinta por proceso
    srand(time(NULL) + rank);

    // Cada proceso genera puntos
    for (long i = rank; i < muestras; i += size) {

        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        if ((x * x + y * y) <= 1.0) {
            contadorLocal++;
        }
    }

    // Reducir resultados
    MPI_Reduce(&contadorLocal,
               &contadorGlobal,
               1,
               MPI_LONG,
               MPI_SUM,
               0,
               MPI_COMM_WORLD);

    if (rank == 0) {

        pi = 4.0 * contadorGlobal / muestras;

        fin = clock();

        printf("PI estimado = %.10f\n", pi);

        printf("Tiempo de ejecucion = %f segundos\n",
            (double)(fin - inicio) / CLOCKS_PER_SEC);
    }

    MPI_Finalize();

    return 0;
}