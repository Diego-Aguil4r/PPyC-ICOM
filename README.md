# Proyecto Final: Programación Paralela y Concurrente (PPyC)

Este repositorio contiene el código y el historial de versiones del proyecto final para la materia de Programación Paralela y Concurrente (D02) del Centro Universitario de Ciencias Exactas e Ingenierías (CUCEI).

El proyecto demuestra la aceleración de algoritmos computacionalmente costosos mediante la paralelización con OpenMP en C++.

## 🚀 Características del Proyecto

El programa principal realiza tres tareas secuenciales sobre una matriz de resolución 8K (7680x4320):
1. **Generación del Conjunto de Mandelbrot:** Cálculo de un fractal con alta irregularidad computacional.
2. **Filtro Gaussiano (Blur):** Aplicación de una matriz de convolución 5x5 para desenfocar la imagen resultante.
3. **Histograma de Colores:** Conteo de la frecuencia de iteraciones/valores en la imagen final.

## ⚙️ Optimizaciones Implementadas con OpenMP

El código evolucionó desde una versión puramente secuencial hasta una versión paralela optimizada, solucionando diferentes cuellos de botella:
* **Balance de Carga Eficiente:** Se implementó `schedule(dynamic, 100)` en el fractal de Mandelbrot para mitigar la asimetría de la carga de trabajo, reduciendo el tiempo drásticamente frente al planificador estático por defecto.
* **Estructura SPMD y Vectorización:** Se forzó la vectorización de los bucles internos de la convolución utilizando la directiva `#pragma omp simd`. El compilador empaquetó exitosamente las instrucciones en vectores de 16 bytes.
* **Eliminación de False Sharing:** Se sustituyó la exclusión mutua (`#pragma omp atomic`) en el cálculo del histograma por el uso de arreglos estrictamente locales por hilo, evitando la contención de memoria y la invalidación constante de la memoria caché L1/L2.

## 💻 Especificaciones de Hardware de Prueba
* **Procesador:** AMD Ryzen 3 3200G (4 núcleos físicos, 4 hilos lógicos)
* **Caché:** L1 (384 KB), L2 (2.0 MB), L3 (4.0 MB)
* **RAM:** 16 GB a 2933 MT/s
* **Entorno:** Máquina Virtual (Ubuntu Linux)
