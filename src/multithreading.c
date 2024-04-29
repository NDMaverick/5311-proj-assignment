/*
 * Created by Nikos Ntokos on 24/4/24.
 * Copyright (c) 2024, Nikos Ntokos
 * All rights reserved.
 */

#include "multithreading.h"
#include "verbosity.h"

void* worker(void* args) {
    ThreadPool* pool = (ThreadPool*)args;

    while (1) {
//        print_verbosity(DEBUG, "{worker - thread %ld}: Acquiring lock for queue: %p", pthread_self(), &pool->queue);
        pthread_mutex_lock(&(pool->queue.mutex));
//        print_verbosity(DEBUG, "{worker - thread %ld}: Acquired lock for queue: %p", pthread_self(), &pool->queue);
        print_verbosity(DEBUG, "{worker - thread %ld}: number of active tasks: %d", pthread_self(), pool->queue.no_active_tasks);
        while (pool->queue.front==pool->queue.rear && !pool->terminated) { // Needs to be changed, because active tasks are not the same as taken up tasks
            print_verbosity(DEBUG, "{worker - thread %ld}: Waiting for new tasks in queue: %p",  pthread_self(), pool->queue.no_active_tasks, &pool->queue);
            pthread_cond_wait(&(pool->queue.cond), &(pool->queue.mutex));
        }
        if (pool->terminated) {
            print_verbosity(DEBUG, "{worker - thread %ld}: Terminating queue: %p", pthread_self(), &pool->queue);
            pthread_mutex_unlock(&(pool->queue.mutex));
            break;
        }

        Task* task = (pool->queue.tasks[pool->queue.front]); // Assign the task at the front of the queue to the worker

        pool->queue.front = (pool->queue.front + 1) % pool->queue.max_tasks; // Update the front of the queue

        print_verbosity(DEBUG, "{worker - thread %ld}: Starting task %p in queue: %p thread %ld", task, &pool->queue, pthread_self());
//        print_verbosity(DEBUG, "{worker - thread %ld}: Releasing lock for queue: %p", pthread_self(), &pool->queue);
        pthread_mutex_unlock(&(pool->queue.mutex));
//        print_verbosity(DEBUG, "{worker - thread %ld}: Released lock for queue: %p", pthread_self(), &pool->queue);


//        print_verbosity(DEBUG, "{worker - thread %ld}: Acquiring lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));
        pthread_mutex_lock(&(task->mutex));
//        print_verbosity(DEBUG, "{worker - thread %ld}: Acquired lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));

        task->output = task->function(task->args);
        task->is_done = true;
        pool->queue.no_active_tasks--;
        pthread_cond_broadcast(&(task->cond));
        print_verbosity(DEBUG, "{worker - thread %ld}: Task %p is done in queue: %p thread %ld", pthread_self(), task, &pool->queue, pthread_self());

//        print_verbosity(DEBUG, "{worker - thread %ld}: Releasing lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));
        pthread_mutex_unlock(&(task->mutex));
//        print_verbosity(DEBUG, "{worker - thread %ld}: Released lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));
    }
    return NULL;
}

ThreadPool* createThreadPool(int max_threads, int max_tasks) {
    print_verbosity(NORMAL, "Creating thread pool with %d threads and %d tasks", max_threads, max_tasks);
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));

    if (pool == NULL) {
        fprintf(stderr, "Failed to allocate memory for thread pool\n");
        return NULL;
    }

    pool->threads = (pthread_t*)malloc(max_threads * sizeof(pthread_t));
    if (pool->threads == NULL) {
        fprintf(stderr, "Failed to allocate memory for threads\n");
        free(pool);
        return NULL;
    }
    pool->terminated = false;
    pool->queue.max_tasks = max_tasks;
    pool->queue.tasks = (Task**)malloc(pool->queue.max_tasks * sizeof(Task));
    if (pool->queue.tasks == NULL) {
        fprintf(stderr, "Failed to allocate memory for tasks\n");
        free(pool->threads);
        free(pool);
        return NULL;
    }

    // Initialize the tasks in the queue to NULL, to avoid mispointers
    for (int i = 0; i < pool->queue.max_tasks; i++) {
        pool->queue.tasks[i] = NULL;
    }

    pool->max_threads = max_threads;
    pool->queue.front = 0;
    pool->queue.rear = 0;
    pthread_mutex_init(&(pool->queue.mutex), NULL);
    pthread_cond_init(&(pool->queue.cond), NULL);

    pthread_mutex_lock(&(pool->queue.mutex));
    print_verbosity(NORMAL, "Thread pool created", max_threads, max_tasks);
    // print tasks
    for (int i = 0; i < pool->queue.max_tasks; i++) {
        if (pool->queue.tasks[i] != NULL) {
            print_verbosity(DEBUG, "Task %d: %p - Done: %d", i, pool->queue.tasks[i], pool->queue.tasks[i]->is_done);
        } else {
            print_verbosity(DEBUG, "Task %d: %p - NULL", i, pool->queue.tasks[i]);
        }
    }
    pthread_mutex_unlock(&(pool->queue.mutex));

    for (int i = 0; i < max_threads; i++) {
        pthread_create(&(pool->threads[i]), NULL, worker, pool);
    }

    return pool;
}

void destroyThreadPool(ThreadPool* pool) {
    print_verbosity(NORMAL, "Destroying thread pool");

    pthread_mutex_lock(&(pool->queue.mutex));
    pool->terminated = true;
    pthread_cond_broadcast(&(pool->queue.cond));
    pthread_mutex_unlock(&(pool->queue.mutex));

    for (int i = 0; i < pool->max_threads; i++) {
        pthread_cancel(pool->threads[i]);
        pthread_join(pool->threads[i], NULL);
        print_verbosity(NORMAL, "thread %ld joined and destroyed", i);
    }

    pthread_mutex_destroy(&(pool->queue.mutex));
    pthread_cond_destroy(&(pool->queue.cond));

    free(pool->threads);
    free(pool);
}


int addTaskFront(ThreadPool* pool, Task* task) {

    print_verbosity(DEBUG, "{addTaskFront - thread %ld}: Adding task %p to queue: %p", pthread_self(), task, &pool->queue);
    pthread_mutex_lock(&(pool->queue.mutex));

    if (pool->queue.no_active_tasks >= pool->queue.max_tasks) {
        print_verbosity(DEBUG, "{addTaskFront - thread %ld}: Queue is full", pthread_self());
        pthread_mutex_unlock(&(pool->queue.mutex));
        return -1;
    }
    if (task == NULL || task->function == NULL) {
        print_verbosity(DEBUG, "{addTaskFront - thread %ld}: Trying to add null task: %p. Aborting...", pthread_self(), task);
        pthread_mutex_unlock(&(pool->queue.mutex));
        return -1;
    }

    task->is_done = false;
    task->output = NULL;

    pthread_mutex_init(&(task->mutex), NULL);
    pthread_cond_init(&(task->cond), NULL);

    // Update the front of the queue to point to the new task
    if (pool->queue.front == 0) {
        pool->queue.front = pool->queue.max_tasks - 1;
    } else {
        pool->queue.front--;
    }

    // Add the task at the front of the queue
    pool->queue.tasks[pool->queue.front] = task;
    pool->queue.no_active_tasks++;

    // print tasks
    for (int i = 0; i < pool->queue.max_tasks; i++) {
        if (pool->queue.tasks[i] != NULL) {
            print_verbosity(DEBUG, "{addTaskFront - thread %ld}: Task %d: %p - Done: %d", pthread_self(), i, pool->queue.tasks[i], pool->queue.tasks[i]->is_done);
        } else {
            print_verbosity(DEBUG, "{addTaskFront - thread %ld}: Task %d: %p - NULL", pthread_self(), i, pool->queue.tasks[i]);
        }
    }
    if (pool->queue.tasks[pool->queue.rear] == NULL) {
        print_verbosity(DEBUG, "{addTaskFront - thread %ld}: Rear task %p is null", pthread_self(), pool->queue.tasks[pool->queue.rear]);
    }

    pthread_cond_broadcast(&(pool->queue.cond));
    pthread_mutex_unlock(&(pool->queue.mutex));
    return 0;
}


void waitForTask(ThreadPool* pool, Task* task) {
    print_verbosity(DEBUG, "{waitForTask - thread %ld}: Checking task: %p", pthread_self(), task);
    if (task->function == NULL) {
        print_verbosity(DEBUG, "{waitForTask - thread %ld}: Task %p is null", pthread_self(), task);
        return;
    }
//    print_verbosity(DEBUG, "{waitForTask - thread %ld}: Acquiring lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));
    pthread_mutex_lock(&(task->mutex));
//    print_verbosity(DEBUG, "{waitForTask - thread %ld}: Acquired lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));
    while (!task->is_done) {
        print_verbosity(DEBUG, "{waitForTask - thread %ld}: Waiting for task: %p", pthread_self(), task);
        pthread_cond_wait(&(task->cond), &(task->mutex));
    }
    print_verbosity(DEBUG, "{waitForTask - thread %ld}: Task %p is done with output: %p", pthread_self(), task, task->output);
//    print_verbosity(DEBUG, "{waitForTask - thread %ld}: Releasing lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));
    pthread_mutex_unlock(&(task->mutex));
//    print_verbosity(DEBUG, "{waitForTask - thread %ld}: Released lock for task: %p at address %p", pthread_self(), task, (void*)&(task->mutex));
}
