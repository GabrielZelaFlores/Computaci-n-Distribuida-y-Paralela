// Author: Wes Kendall (MPI) - Modificado a OpenMP por Gabriel Frank Krisna
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <assert.h>
#include <windows.h>     // ← Agregado para caracteres en español

float *create_rand_nums(int num_elements) {
  float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
  assert(rand_nums != NULL);
  for (int i = 0; i < num_elements; i++) {
    rand_nums[i] = (rand() / (float)RAND_MAX);
  }
  return rand_nums;
}

float compute_avg(float *array, int num_elements) {
  float sum = 0.0f;
  for (int i = 0; i < num_elements; i++) {
    sum += array[i];
  }
  return sum / num_elements;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s num_elements\n", argv[0]);
    exit(1);
  }

  // Soluciona los caracteres extraños (ñ, ó, ú, etc.)
  SetConsoleOutputCP(CP_UTF8);

  int num_elements = atoi(argv[1]);
  srand(time(NULL));

  float *rand_nums = create_rand_nums(num_elements);
  assert(rand_nums != NULL);

  double start_time = omp_get_wtime();
  float total_sum = 0.0f;
  int num_threads = 1;

  // === VERSIÓN COMPATIBLE CON VISUAL STUDIO ===
  int i;   // ← Variable declarada fuera (obligatorio en cl)
  #pragma omp parallel
  {
      #pragma omp single
      num_threads = omp_get_num_threads();

      #pragma omp for reduction(+:total_sum)
      for (i = 0; i < num_elements; i++) {
          total_sum += rand_nums[i];
      }
  }

  float avg = total_sum / num_elements;
  double end_time = omp_get_wtime();

  printf("=== OpenMP ===\n");
  printf("Avg of all elements is %f\n", avg);
  printf("Tiempo de ejecución: %f segundos\n", end_time - start_time);
  printf("Número de hilos utilizados: %d\n\n", num_threads);

  // Cálculo secuencial para comparación
  double seq_start = omp_get_wtime();
  float original_avg = compute_avg(rand_nums, num_elements);
  double seq_end = omp_get_wtime();

  printf("Avg computed sequentially is %f\n", original_avg);
  printf("Tiempo secuencial: %f segundos\n", seq_end - seq_start);
  printf("Speedup: %.2fx\n", (seq_end - seq_start) / (end_time - start_time));

  free(rand_nums);
  return 0;
}