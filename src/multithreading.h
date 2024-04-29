/*
 * Created by Nikos Ntokos on 24/4/24.
 * Copyright (c) 2024, Nikos Ntokos
 * All rights reserved.
 */

#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    void* (*function)(void*);
    void* args;
    bool is_done;
    void* output;
    int priority; // The priority of the task. The lower the number, the higher the priority
    pthread_mutex_t mutex; // Mutex to protect the task
    pthread_cond_t cond; // Condition variable to signal the completion of the task
} Task;

typedef struct {
    Task** tasks;
    int max_tasks;
    int front;
    int rear;
    int no_active_tasks;
    pthread_mutex_t mutex; // Mutex to protect the queue
    pthread_cond_t cond; // Condition variable to signal the availability of tasks
} TaskQueue;

typedef struct {
    pthread_t* threads;
    int max_threads;
    TaskQueue queue;
    bool terminated;
} ThreadPool;

bool isTaskInQueue(TaskQueue* queue, Task* task);

void* worker(void* args);
ThreadPool* createThreadPool(int max_threads, int max_tasks);
void destroyThreadPool(ThreadPool* pool);
int addTaskFront(ThreadPool* pool, Task* task);
void waitForTask(ThreadPool* pool, Task* task);


#endif //MULTITHREADING_H
