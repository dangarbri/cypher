#ifndef _POOL_H_
#define _POOL_H_

#include <stddef.h>
#include <threads.h>

struct ThreadPool {
    /** Array of thread identifiers */
    thrd_t* threads;
    /** Number of active threads */
    size_t count;
    /** Maximum number of threads for this pool */
    size_t size;
    /** Index of the next available thread */
    size_t head;
    /** Index of the oldest thread */
    size_t tail;
};

/**
 * Contains information about a thread
 */
struct ThreadInfo {
    /** Function this thread is executing */
    thrd_start_t Function;
    /** Argument passed to Function */
    void* arg;
    /** Thread's return status */
    int res;
};

/**
 * Stores a list of thread functions to be worked on by a thread pool
 */
struct ThreadQueue {
    struct ThreadInfo* info;
    size_t length;
};

/**
 * Creates a new thread pool
 * @param count number of active worker threads.
 * @return New pool or NULL on failure
 */
struct ThreadPool* ThreadPool_New(size_t count);

/**
 * Allocate memory for a thread queue
 * @param count size of the queue
 */
struct ThreadQueue* ThreadQueue_New(size_t count);

/**
 * Submits a queue to be worked on by this thread pool.
 *
 * To wait for the queue to be fully processed, call thrd_join on the result.
 * At this point, the res field of the ThreadInfo structs will contain
 * the thread's return statuses.
 *
 * If the result of this thread is -1, then there was an error processing the queue.
 * If the result is 0, then queue processing completed successfully.
 *
 * @param pool Thread pool to schedule the queue on.
 * @param queue Queue of functions to be worked on by the pool
 * @return 0 on failure, otherwise the Primary thread that will complete when all pool items are processed
 */
thrd_t ThreadPool_SubmitQueue(struct ThreadPool* pool, struct ThreadQueue* queue);

/**
 * Submits a job to the pool
 * If there are no slots available in the pool, this function will block until a slot is received.
 * @param pool Pool to submit the job to
 * @param job Parameters required to execute the thread
 * @returns thrd_success, thrd_nomem, or thrd_error
 */
int ThreadPool_SubmitJob(struct ThreadPool* pool, struct ThreadInfo* job);

/**
 * Waits for all active threads in the pool to complete.
 * @param pool Thread pool to wait on
 * @returns thrd_success or thrd_error on failure
 */
int ThreadPool_Join(struct ThreadPool* pool);

/**
 * Frees the given threadpool
 */
void ThreadPool_Free(struct ThreadPool* pool);

void ThreadQueue_Free(struct ThreadQueue* queue);

#endif