#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <time.h>

#define THREADS 256
#define BLOCKS 256

// Generador pseudoaleatorio mejorado
__device__ unsigned int lcg(unsigned int seed) {
    return seed * 1664525 + 1013904223;
}

__device__ double randomGPU(unsigned int seed) {

    seed = lcg(seed);

    return (double)(seed & 0xFFFFFF) / 16777215.0;
}

__global__ void monteCarlo(long long* resultados,
                           long long muestras) {

    int idx = threadIdx.x +
              blockIdx.x * blockDim.x;

    long long contador = 0;

    long long totalThreads =
        blockDim.x * gridDim.x;

    for (long long i = idx;
         i < muestras;
         i += totalThreads) {

        double x =
            randomGPU(i + idx);

        double y =
            randomGPU(i + idx + 999999);

        if ((x * x + y * y) <= 1.0) {
            contador++;
        }
    }

    resultados[idx] = contador;
}

int main() {

    long long muestras;

    printf("Ingrese numero de muestras: ");
    scanf("%lld", &muestras);

    int totalThreads =
        THREADS * BLOCKS;

    long long* resultadosCPU =
        (long long*)malloc(
            totalThreads *
            sizeof(long long));

    long long* resultadosGPU;

    cudaMalloc((void**)&resultadosGPU,
               totalThreads *
               sizeof(long long));

    clock_t inicio = clock();

    monteCarlo<<<BLOCKS, THREADS>>>(
        resultadosGPU,
        muestras);

    cudaDeviceSynchronize();

    cudaMemcpy(resultadosCPU,
               resultadosGPU,
               totalThreads *
               sizeof(long long),
               cudaMemcpyDeviceToHost);

    long long total = 0;

    for (int i = 0;
         i < totalThreads;
         i++) {

        total += resultadosCPU[i];
    }

    double pi =
        4.0 *
        (double)total /
        (double)muestras;

    clock_t fin = clock();

    printf("PI estimado = %.10f\n", pi);

    printf("Tiempo de ejecucion = %f segundos\n",
           (double)(fin - inicio) /
           CLOCKS_PER_SEC);

    cudaFree(resultadosGPU);

    free(resultadosCPU);

    return 0;
}