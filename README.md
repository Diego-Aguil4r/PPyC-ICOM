# Parallel and Concurrent Programming Final Project (PPyC-ICOM)

This repository contains the source code, version history, and performance documentation for the final project of the Parallel and Concurrent Programming course (Section D02) at the Centro Universitario de Ciencias Exactas e Ingenierías (CUCEI).

The project demonstrates empirical performance acceleration of computationally expensive algorithms by applying parallel computing patterns with OpenMP in C++.

## 🚀 Project Architecture

The application executes three consecutive processing tasks on an ultra-high resolution 8K matrix (7680 x 4320 pixels):
1. **Mandelbrot Set Generation (Task A):** Computation of a complex fractal exhibiting massive computational load imbalance.
2. **Gaussian Blur Filter (Task B):** Application of a 5x5 spatial convolution matrix to blur the generated fractal image.
3. **Color Histogram Extraction:** Frequency distribution tracking of iteration values across the finalized dataset.

## ⚙️ Implemented OpenMP Optimizations

The implementation systematically evolved from a baseline sequential codebase to a fully optimized concurrent structure, addressing critical performance bottlenecks:

* **Dynamic Load Balancing:** Mitigated the severe spatial load asymmetry of the Mandelbrot set by shifting from default static scheduling to `schedule(dynamic, 100)`. This minimized thread starvation and reduced execution time significantly.
* **SPMD Structure & Loop Vectorization:** Forced vectorization within the innermost convolution loops of the Gaussian filter using the `#pragma omp simd` directive. Compilation diagnostics confirm successful data packaging into 16-byte SIMD vector registers.
* **Cache Contention & False Sharing Elimination:** Resolved severe L1/L2 cache line invalidations in the histogram calculation. The initial implementation relied on shared global arrays protected via mutual exclusion (`#pragma omp atomic`), which induced false sharing. This was replaced with an optimized localized reduction strategy using strictly local thread-private arrays.

## 💻 Hardware Specifications (Testbed)
* **CPU:** AMD Ryzen 3 3200G with Radeon Vega Graphics (4 Physical Cores, 4 Logical Threads)
* **Base Frequency:** 3.60 GHz
* **Cache Hierarchy:** L1: 384 KB, L2: 2.0 MB, L3: 4.0 MB
* **Memory:** 16.0 GB RAM @ 2933 MT/s
* **OS Environment:** Linux Ubuntu Virtual Machine (hosted via Hypervisor)
