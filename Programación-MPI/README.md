#  Integración Numérica con MPI

##  Descripción

Se transformó un programa en C que calculaba una integral usando **OpenMP (memoria compartida)** a una versión usando **MPI (memoria distribuida)**.

El método utilizado es la **regla del trapecio** para aproximar la integral de una función.

---

## Cambio realizado

### 🔹 OpenMP

* Usa hilos dentro de una sola máquina.
* Paralelismo con `#pragma omp`.

### 🔹 MPI

* Usa múltiples procesos.
* Comunicación mediante:

  * `MPI_Init`
  * `MPI_Bcast`
  * `MPI_Reduce`

Cada proceso calcula una parte de la integral y luego se combinan los resultados.

---

##  Compilación

```bash
cl integral_mpi.c ^
/I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" ^
/link ^
/LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" msmpi.lib
```

---

##  Ejecución

```bash
mpiexec -n 4 integral_mpi.exe 0 10 1000
```

 

```
![Ejecución del programa](img/ejecucion.png)
```

   

---

##  Resultado

```
Con n = 1000 trapezoides, la estimacion
de la integral de 0.000000 hasta 10.000000 = 1033.344238
```

---

##  Ventajas de MPI

*  Mayor escalabilidad (puede usarse en múltiples máquinas)
*  Evita problemas de memoria compartida
*  Mejor control del paralelismo

---

##  Desventajas de MPI

*  Más complejo de programar
*  Mayor costo de comunicación entre procesos
*  Requiere configuración adicional

---

##  Conclusión

MPI permite mayor potencia y escalabilidad que OpenMP, pero a costa de mayor complejidad. Es ideal para sistemas distribuidos y problemas grandes.
