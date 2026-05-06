#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

float f(float x) {
    return (x*x*x/3) + 4*x;
}

int main(int argc, char *argv[]) {
    int rank, size;
    float a, b;
    int n;

    float local_a, local_b;
    int local_n;

    float h;
    float x;
    float local_integral = 0.0;
    float total = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Solo el proceso 0 lee los datos
    if (rank == 0) {
        a = atof(argv[1]);
        b = atof(argv[2]);
        n = atoi(argv[3]);
    }

    // Compartir datos con todos los procesos
    MPI_Bcast(&a, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    h = (b - a) / n;

    local_n = n / size;
    local_a = a + rank * local_n * h;
    local_b = local_a + local_n * h;

    // Regla del trapecio local
    local_integral = (f(local_a) + f(local_b)) / 2.0;

    x = local_a;
    for (int i = 1; i < local_n; i++) {
        x += h;
        local_integral += f(x);
    }

    local_integral *= h;

    // Reducir resultados al proceso 0
    MPI_Reduce(&local_integral, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (total < 0) total *= -1;
        printf("Con n = %d trapezoides, la estimacion\n", n);
        printf("de la integral de %f hasta %f = %f\n", a, b, total);
    }

    MPI_Finalize();
    return 0;
}
