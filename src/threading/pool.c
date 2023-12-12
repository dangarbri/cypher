#include <stdio.h>
#include <stdlib.h>

#include "pool.h"

struct ThreadPool* ThreadPool_New(size_t count) {
    struct ThreadPool* pool = malloc(sizeof(struct ThreadPool));
    if (pool != NULL) {
        pool->threads = malloc(count * sizeof(thrd_t));
        if (pool->threads != NULL) {
            pool->count = 0;
            pool->size = count;
            pool->head = 0;
            pool->tail = 0;
        } else {
            free(pool);
            pool = NULL;
        }
    }

    if (pool == NULL) {
        perror("Failed to allocate thread pool");
    }
    return pool;
}

void ThreadPool_Free(struct ThreadPool* pool) {
    if (pool != NULL) {
        if (pool->threads != NULL) {
            free(pool->threads);
        }
        free(pool);
    }
}

struct ThreadQueue* ThreadQueue_New(size_t count) {
    struct ThreadQueue* queue = malloc(sizeof(struct ThreadQueue));
    if (queue != NULL) {
        queue->info = malloc(count * sizeof(struct ThreadInfo));
        if (queue->info != NULL) {
            queue->length = count;
            return queue;
        }
        free(queue);
    }
    perror("ThreadQueue_New");
    return NULL;
}

void ThreadQueue_Free(struct ThreadQueue* queue) {
    if (queue != NULL) {
        if (queue->info != NULL) {
            free(queue->info);
        }
        free(queue);
    }
}


struct ThreadPoolManagerArgs {
    struct ThreadPool* pool;
    struct ThreadQueue* queue;
};

int ThreadPool_QueueManager(void* ptr2arg) {
    struct ThreadPoolManagerArgs* args = (struct ThreadPoolManagerArgs*) ptr2arg;
    puts("Started the queue manager!");
    for (size_t i = 0; i < args->queue->length; i++) {
        if (ThreadPool_SubmitJob(args->pool, &args->queue->info[i]) != thrd_success) {
            puts("Failed to submit job to the pool");
            break;
        }
    }

    int status = 0;
    if (ThreadPool_Join(args->pool) == thrd_error) {
        puts("Failed to wait for the thread pool to finish work");
        status = -1;
    }
    free(args);
    puts("Exiting the queue manager!");
    return status;
}

thrd_t ThreadPool_SubmitQueue(struct ThreadPool* pool, struct ThreadQueue* queue) {
    struct ThreadPoolManagerArgs* args = malloc(sizeof(struct ThreadPoolManagerArgs));
    if (args == NULL) {
        perror("ThreadPool_SubmitQueue");
        return 0;
    }

    args->pool = pool;
    args->queue = queue;
    thrd_t thread;
    int result = thrd_create(&thread, ThreadPool_QueueManager, args);
    if (result != thrd_success) {
        fputs("Failed to start thread pool manager\n", stderr);
        free(args);
        return 0;
    }

    return thread;
}

int ThreadPool_Work(void* arg) {
    struct ThreadInfo* job = (struct ThreadInfo*) arg;
    job->res = job->Function(job->arg);
    return 0;
}

int ThreadPool_SubmitJob(struct ThreadPool* pool, struct ThreadInfo* job) {
    // If there is space in the pool, start the thread.
    if (pool->count < pool->size) {
        thrd_t thread;
        int result = thrd_create(&thread, ThreadPool_Work, job);
        // If thread creation fails, return the error result
        if (result != thrd_success) {
            return result;
        }

        pool->threads[pool->head] = thread;
        pool->head = (pool->head + 1) % pool->size;
        pool->count += 1;
        return thrd_success;
    } else {
        // If there's no space in the queue, wait for a thread to complete
        // TODO: This just waits for the oldest thread, but it would be nice
        //       if there was a mechanism to see if any other threads have completed.
        int result = thrd_join(pool->threads[pool->tail], NULL);
        if (result != thrd_success) {
            return result;
        }
        pool->tail = (pool->tail + 1) % pool->size;
        pool->count -= 1;
        // Then resubmit the request to start the new thread
        return ThreadPool_SubmitJob(pool, job);
    }
}

int ThreadPool_Join(struct ThreadPool* pool) {
    while (pool->count > 0) {
        int result = thrd_join(pool->threads[pool->tail], NULL);
        if (result != thrd_success) {
            return result;
        }
        pool->tail = (pool->tail + 1) % pool->size;
        pool->count -= 1;
    }
    return thrd_success;
}