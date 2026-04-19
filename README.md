# Computación Distribuida y Paralela

## Evidencia 1: Instalación de un Cluster con MPI, NFS y SSH

**Estudiante:** Gabriel Frank Krisna Zela Flores
**Curso:** Computación Distribuida y Paralela
**Docente:** Victor Eduardo Martinez Abaunza
**Fecha:** 19 de abril de 2026

---

# Objetivo

Construir un cluster utilizando máquinas virtuales bajo las siguientes condiciones:

* Un nodo de administración.
* Un nodo de computación.
* Comunicación segura mediante SSH.
* Sistema de archivos compartido con NFS.
* Ejecución del programa `hello_mpi.c` para verificar el funcionamiento del cluster.

---

# Configuración del entorno

Se utilizaron dos máquinas virtuales con Ubuntu 24.04:

| Nodo                   | Nombre  | Usuario | IP interna     |
| ---------------------- | ------- | ------- | -------------- |
| Nodo de administración | master  | gabriel | 192.168.100.10 |
| Nodo de computación    | worker1 | gabo    | 192.168.100.11 |

Configuración de red en ambas máquinas virtuales:

* Adaptador 1: NAT
* Adaptador 2: Red interna `cluster-net`

---

# 1. Configuración de red

## En el nodo master

```bash
sudo ip addr add 192.168.100.10/24 dev enp0s8
sudo ip link set enp0s8 up
```

## En el nodo worker1

```bash
sudo ip addr add 192.168.100.11/24 dev enp0s8
sudo ip link set enp0s8 up
```

## Verificación de conectividad

Desde `worker1`:

```bash
ping 192.168.100.10
```

Resultado esperado:

```text
64 bytes from 192.168.100.10: icmp_seq=1 ttl=64 time=...
```

---

# 2. Instalación y configuración de SSH

## En ambas máquinas

```bash
sudo apt update
sudo apt install openssh-server -y
sudo systemctl enable ssh
sudo systemctl start ssh
```

Verificación:

```bash
sudo systemctl status ssh
```

Debe aparecer:

```text
Active: active (running)
```

## Generación de llave SSH en el master

```bash
ssh-keygen -t rsa
```

Se aceptó la ruta por defecto y se dejó la contraseña vacía.

## Copia de la llave al nodo worker1

```bash
ssh-copy-id gabo@192.168.100.11
```

## Verificación

```bash
ssh gabo@192.168.100.11
```

Si la conexión se realiza sin pedir contraseña, SSH quedó correctamente configurado.

---

# 3. Configuración de NFS

## En el nodo master

Instalación del servidor NFS:

```bash
sudo apt install nfs-kernel-server -y
mkdir ~/shared
chmod 777 ~/shared
```

Edición del archivo `/etc/exports`:

```bash
sudo nano /etc/exports
```

Se agregó la siguiente línea:

```text
/home/gabriel/shared 192.168.100.0/24(rw,sync,no_subtree_check)
```

Aplicación de los cambios:

```bash
sudo exportfs -a
sudo systemctl restart nfs-kernel-server
```

## En el nodo worker1

Instalación del cliente NFS:

```bash
sudo apt install nfs-common -y
mkdir ~/shared
sudo mount 192.168.100.10:/home/gabriel/shared ~/shared
```

## Verificación del sistema de archivos compartido

En `worker1`:

```bash
echo "hola cluster" > ~/shared/prueba.txt
```

En `master`:

```bash
cat ~/shared/prueba.txt
```

Resultado:

```text
hola cluster
```

---

# 4. Instalación de MPI

## En ambas máquinas

```bash
sudo apt install openmpi-bin openmpi-common libopenmpi-dev -y
```

Verificación:

```bash
mpicc --version
mpirun --version
```

---

# 5. Programa de prueba `hello_mpi.c`

En el nodo `master`, dentro de la carpeta compartida:

```bash
nano ~/shared/hello_mpi.c
```

Contenido del archivo:

```c
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int lnom;
    char nombrepr[MPI_MAX_PROCESSOR_NAME];
    int pid, npr;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npr);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Get_processor_name(nombrepr, &lnom);

    printf(">> Proceso %2d de %2d activado en %s\n", pid, npr, nombrepr);

    MPI_Finalize();
    return 0;
}
```

Compilación:

```bash
cd ~/shared
mpicc hello_mpi.c -o hello_mpi
```

---

# 6. Configuración de los hosts para MPI

Archivo `hosts`:

```bash
nano ~/shared/hosts
```

Contenido:

```text
localhost slots=2
worker1 slots=2
```

Configuración adicional en el master:

```bash
nano ~/.ssh/config
```

Contenido:

```text
Host worker1
    HostName 192.168.100.11
    User gabo
```

Además, en `worker1` fue necesario crear un enlace simbólico debido a que el usuario del nodo master era distinto:

```bash
sudo mkdir -p /home/gabriel
sudo ln -s /home/gabo/shared /home/gabriel/shared
```

---

# 7. Ejecución del cluster

Desde el nodo `master`:

```bash
cd ~/shared
mpirun --mca plm_rsh_agent ssh --hostfile hosts -np 4 ./hello_mpi
```

Resultado obtenido:

```text
>> Proceso  0 de  4 activado en gabrielzela
>> Proceso  1 de  4 activado en gabrielzela
>> Proceso  2 de  4 activado en gabozela
>> Proceso  3 de  4 activado en gabozela
```

## Captura de pantalla

Inserte aquí la captura de pantalla de la ejecución anterior.

> Recomendación: colocar una imagen mostrando la terminal del nodo master con la salida del comando `mpirun`.

---

# Problemas encontrados y solución

## 1. La red interna no funcionaba

Inicialmente las máquinas solo tenían el adaptador NAT, por lo que no podían comunicarse entre sí.

**Solución:**
Se habilitó un segundo adaptador de tipo “Red interna” con el nombre `cluster-net`.

## 2. Las máquinas no tenían IP en la red interna

La interfaz `enp0s8` no tenía dirección IP.

**Solución:**
Se asignaron manualmente las direcciones:

* Master: `192.168.100.10`
* Worker1: `192.168.100.11`

## 3. SSH no funcionaba sin contraseña

Al inicio, el master pedía contraseña cada vez que intentaba conectarse al worker.

**Solución:**
Se generó una llave RSA con `ssh-keygen` y se copió al worker usando `ssh-copy-id`.

## 4. NFS no permitía montar la carpeta compartida

El comando `mount` devolvía el error:

```text
access denied by server while mounting
```

**Causa:**
La línea del archivo `/etc/exports` estaba comentada con `#`.

**Solución:**
Se eliminó el `#` y se volvió a ejecutar:

```bash
sudo exportfs -a
sudo systemctl restart nfs-kernel-server
```

## 5. MPI no encontraba el usuario correcto

El nodo master utilizaba el usuario `gabriel`, mientras que el worker utilizaba `gabo`.

**Solución:**
Se creó el archivo `~/.ssh/config` para indicarle a MPI qué usuario debía usar en `worker1`.

## 6. MPI no encontraba el ejecutable `hello_mpi`

El worker buscaba el programa en `/home/gabriel/shared`, pero realmente estaba en `/home/gabo/shared`.

**Solución:**
Se creó un enlace simbólico:

```bash
sudo ln -s /home/gabo/shared /home/gabriel/shared
```

---

# Conclusiones

* Se logró implementar correctamente un cluster básico con dos nodos.
* El nodo master administró el sistema de archivos compartido y la ejecución distribuida.
* La comunicación segura mediante SSH permitió que MPI ejecutara procesos remotamente.
* El programa `hello_mpi.c` confirmó que los procesos fueron distribuidos entre ambos nodos.
* La práctica permitió comprender el funcionamiento básico de un entorno de computación distribuida y paralela.
