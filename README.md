# 5311 Project Assignment

## Description

This is the implementation of a multi-threaded merge sort algorithm in C for the needs of the course 5311 Design & Analysis of Algorithms.

In the scope of this project, two versions of the merge sort algorithm are implemented and benchmarked. The first version is a single-threaded implementation of the merge sort algorithm, while the second version is a multi-threaded implementation of the merge sort algorithm.
<br />For benchmarking purposes the single-threaded version is essentially the same as the multi-threaded version, but with the multi-threading part removed.

## Requirements
* **gcc**
* **pthreads**
* **cmake** (optional)

## Build
1. Using **cmake**
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
2. Using **gcc**
  * For the parallel version
    ```bash
    gcc -o p_merge_sort src/p_merge_sort.c src/multithreading.c src/verbosity.c -Isrc -lpthread -lm
    ```
  * For the traditional version
    ```bash
    gcc -o trad_merge_sort src/trad_merge_sort.c src/verbosity.c -Isrc -lpthread -lm
    ```

## Run
Executables should be created in the root directory of the project.
You can optionally provide the size of the array to be sorted. If no size is provided, the default size is $10^6$.
* For the parallel version
  ```bash
  ./p_merge_sort [array_size]
  ```
* For the traditional version
  ```bash
  ./trad_merge_sort [array_size]
  ```

## Notes
* The array to be sorted is generated randomly.
* The array is sorted in ascending order.
* The MAX_THREADS should always be greater or equal to MAX_TASKS_IN_QUEUE. That is because this is a recursive algorithm and tasks will only be consumed if their subtasks are consumed. So the queue cannot have more tasks than the active threads. This will result in tasks never getting picked up by any thread
* The default configuration for MAX_THREADS and MAX_TASKS_IN_QUEUE is 3 and 3 respectively and was set after some experimentation. This configuration was found to be the most efficient for the given problem. However, you can change these values in the `p_merge_sort.c` file.
* The VERBOSITY is set to SILENT by default. You can change this value in either `p_merge_sort.c` or `trad_merge_sort.c` files.
* The benchmarking is done by utilizing both wall time and CPU time. The wall time is the time that has passed in the real world, while the CPU time is the time that the CPU has spent on the process.
