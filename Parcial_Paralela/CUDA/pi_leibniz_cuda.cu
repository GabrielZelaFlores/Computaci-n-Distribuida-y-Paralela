#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <time.h>

#define THREADS 256
#define BLOCKS 256

__global__ void calcularPi(double* resultados, long long iteraciones) {

    int idx = threadIdx.x + blockIdx.x * blockDim.x;

    double suma = 0.0;

    long long totalThreads = blockDim.x * gridDim.x;

    for (long long i = idx; i < iteraciones; i += totalThreads) {

        if (i % 2 == 0)
            suma += 4.0 / (2.0 * i + 1.0);
        else
            suma -= 4.0 / (2.0 * i + 1.0);
    }

    resultados[idx] = suma;
}

int main() {

    long long iteraciones;

    printf("Ingrese numero de iteraciones: ");
    scanf("%lld", &iteraciones);

    int totalThreads = THREADS * BLOCKS;

    // CPU
    double* resultadosCPU =
        (double*)malloc(totalThreads * sizeof(double));

    // GPU
    double* resultadosGPU;

    cudaError_t error;

    error = cudaMalloc((void**)&resultadosGPU,
                       totalThreads * sizeof(double));

    if (error != cudaSuccess) {
        printf("Error cudaMalloc: %s\n",
               cudaGetErrorString(error));
        return 1;
    }

    clock_t inicio = clock();

    // Ejecutar kernel
    calcularPi<<<BLOCKS, THREADS>>>(resultadosGPU, iteraciones);

    // Verificar errores del kernel
    error = cudaGetLastError();

    if (error != cudaSuccess) {
        printf("Error kernel: %s\n",
               cudaGetErrorString(error));
        return 1;
    }

    // Esperar GPU
    cudaDeviceSynchronize();

    // Copiar GPU -> CPU
    error = cudaMemcpy(resultadosCPU,
                       resultadosGPU,
                       totalThreads * sizeof(double),
                       cudaMemcpyDeviceToHost);

    if (error != cudaSuccess) {
        printf("Error cudaMemcpy: %s\n",
               cudaGetErrorString(error));
        return 1;
    }

    // Sumar resultados
    double pi = 0.0;

    for (int i = 0; i < totalThreads; i++) {
        pi += resultadosCPU[i];
    }

    clock_t fin = clock();

    printf("PI = %.10f\n", pi);

    printf("Tiempo de ejecucion = %f segundos\n",
           (double)(fin - inicio) / CLOCKS_PER_SEC);

    cudaFree(resultadosGPU);

    free(resultadosCPU);

    return 0;
}