/*
 * Created by Nikos Ntokos on 24/4/24.
 * Copyright (c) 2024, Nikos Ntokos
 * All rights reserved.
 */

#ifndef P_MERGE_SORT_H
#define P_MERGE_SORT_H

#include <pthread.h>
#include "multithreading.h"

typedef struct {
    int* A;
    int p;
    int r;
    int* B;
    int s;
    ThreadPool* pool;
    int depth;
} SortArgs;

typedef struct {
    int* T;
    int p1;
    int r1;
    int p2;
    int r2;
    int* A;
    int p3;
    ThreadPool *pool;
    int depth;
} MergeArgs;

int binary_search(int x, const int* arr, int p, int r);
void swap(int *n1, int *n2);

void* p_merge(void* args);
void* p_merge_sort(void* args);

#endif //P_MERGE_SORT_H
