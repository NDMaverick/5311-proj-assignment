/*
 * Created by Nikos Ntokos on 24/4/24.
 * Copyright (c) 2024, Nikos Ntokos
 * All rights reserved.
 */

#ifndef TRAD_MERGE_SORT_H
#define TRAD_MERGE_SORT_H

typedef struct {
    int* A;
    int p;
    int r;
    int* B;
    int s;
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
    int depth;
} MergeArgs;

int binary_search(int x, const int* arr, int p, int r);
void swap(int *n1, int *n2);
void* merge(void* args);
void* merge_sort(void* args);

#endif //TRAD_MERGE_SORT_H
