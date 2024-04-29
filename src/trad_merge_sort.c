/*
 * Created by Nikos Ntokos on 24/4/24.
 * Copyright (c) 2024, Nikos Ntokos
 * All rights reserved.
 */

#include "trad_merge_sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "verbosity.h"

#define DEFAULT_ARRAY_SIZE 1000000

#define VERBOSITY_LEVEL SILENT

// Binary Search Implementation
int binary_search(int x, const int* arr, int p, int r) {
    // low is the starting index p, and high is the max of p and r+1
    int low = p;
    int high = (p > r + 1) ? p : r + 1;
    while (low < high) {
        int mid = (int)floor((double)(low+high) / 2);
        if (x <= arr[mid])
            high = mid;
        else
            low = mid + 1;
    }
    return high;
}

void swap(int *n1, int *n2) {
    int temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

void* merge(void* args) {
    MergeArgs* mergeArgs = (MergeArgs*) args;
    int* T = mergeArgs->T;
    int p1 = mergeArgs->p1; int r1 = mergeArgs->r1;
    int p2 = mergeArgs->p2; int r2 = mergeArgs->r2;
    int* A = mergeArgs->A;
    int p3 = mergeArgs->p3;
    int depth = mergeArgs->depth;

    int n1 = r1 - p1 + 1;
    int n2 = r2 - p2 + 1;

    if (n1 < n2) {
        swap(&p1, &p2);
        swap(&r1, &r2);
        swap(&n1, &n2);
    }

    if (n1 == 0) {
        return NULL;
    } else {
        int q1 = floor((p1 + r1) / 2);
        int q2 = binary_search(T[q1], T, p2, r2);
        int q3 = p3 + (q1 - p1) + (q2 - p2);

        A[q3] = T[q1];

        MergeArgs left_args = {T, p1, q1 - 1, p2, q2 - 1, A, p3, depth+1};
        MergeArgs right_args = {T, q1 + 1, r1, q2, r2, A, q3 + 1, depth+1};

        merge(&left_args);
        merge(&right_args);

    }
    return NULL;
}

void* merge_sort(void* args) {
    SortArgs* sortArgs = malloc(sizeof(SortArgs)); // Dynamically allocate memory for arguments
    memcpy(sortArgs, args, sizeof(SortArgs)); // Copy the arguments to the new memory location
    int p = sortArgs->p;
    int r = sortArgs->r;
    int s = sortArgs->s;
    int* A = sortArgs->A;
    int* B = sortArgs->B;
    int depth = sortArgs->depth;

    int n = r - p + 1;
    if (n==1) {
        B[s] = A[p];
    } else {
        int q = (int)floor((double)(p + r) / 2);
        int q_prime = q-p+1;
        int* T = (int*)malloc(n * sizeof(int));
        if (!T) {
            fprintf(stderr, "Failed to allocate memory\n");
            exit(EXIT_FAILURE);
        }

        SortArgs left_args = {A, p, q, T, 0, depth+1};
        SortArgs right_args = {A, q + 1, r, T, q_prime, depth+1};

        merge_sort(&left_args);
        merge_sort(&right_args);

        MergeArgs merge_args = {T, 0, q_prime-1, q_prime, n-1, B, s, 0};
        merge(&merge_args);
        free(T);
        T=NULL;

    }
    free(sortArgs); // Free the dynamically allocated memory when done
    sortArgs = NULL;
    return NULL;
}

int main(int argc, char* argv[]) {
    set_verbosity(VERBOSITY_LEVEL);
    int array_size = DEFAULT_ARRAY_SIZE;
    if (argc > 1) {
        array_size = atoi(argv[1]);
        if (array_size <= 0) {
            fprintf(stderr, "{main}: Invalid array size\n");
            exit(EXIT_FAILURE);
        }
    }

    int *A = malloc(array_size * sizeof(int));
    int *B = malloc(array_size * sizeof(int));

    if (!A || !B) { // Check if memory allocation for B failed
        print_verbosity(NORMAL, "{main}: Failed to allocate memory for A and/or B\n");
        exit(EXIT_FAILURE);
    } else {
        print_verbosity(NORMAL, "{main}: Memory allocated for A and B\n");
    }

    // Seed the random number generator
    srand(time(NULL));
    // Populate the A with random numbers
    for (int i = 0; i < array_size; i++) {
        A[i] = rand() % 100000;
    }

    // Arguments for initial merge_sort
    SortArgs args = {A, 0, array_size - 1, B, 0, 0};

    // Initial Benchmark variables
    struct rusage usage; // Memory usage
    long initial_memory, final_memory; // Memory usage variables
    clock_t start_cpu, end_cpu; // CPU time variables
    struct timeval start, end; // Wall time variables

    // Get the initial memory usage
    getrusage(RUSAGE_SELF, &usage);
    initial_memory = usage.ru_maxrss;

    // Start the CPU timer
    start_cpu = clock();

    // Time of day start
    gettimeofday(&start, NULL);

    merge_sort(&args); // Call the initial merge_sort

    // Time of day end
    gettimeofday(&end, NULL);

    // Stop the timer
    end_cpu = clock();

    // Get the final memory usage
    getrusage(RUSAGE_SELF, &usage);
    final_memory = usage.ru_maxrss;

    // Output the sorted B
//    int as = array_size < 100 ? array_size : 100;
//    for (int i = 0; i < as; i++) {
//        printf("%d ", A[i]);
//    }
//    printf("\n");
//    for (int i = 0; i < as; i++) {
//        printf("%d ", B[i]);
//    }
//    printf("\n");
    free(A);
    free(B);

    // Calculate the time taken and memory used
    double wall_time = (double) (end.tv_usec - start.tv_usec) / 1000000 + (double) (end.tv_sec - start.tv_sec);
    double cpu_time = ((double) end_cpu - start_cpu) / CLOCKS_PER_SEC;
    long memory_used = final_memory - initial_memory;

    printf("Wall Time: %f seconds\n", wall_time);
    printf("CPU Time: %f seconds\n", cpu_time);
    printf("Memory used: %ld kilobytes\n", memory_used);

    return 0;
}
