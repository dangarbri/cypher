#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include "threading/pool.h"

int sleep_in_thread(void* arg) {
    (void)arg;
    // 1.25 seconds
    const struct timespec duration = {
        .tv_sec = 2,
        .tv_nsec = 0
    };
    puts("Going to sleep in thread");
    int status = thrd_sleep(&duration, NULL);
    if (status < 0) {
        puts("Failed to sleep in thread!");
    } else {
        puts("Woke up in thread");
    }
    return 0;
}

void test_Pool() {
    // Create a pool which manages 5 active threads
    struct ThreadPool* pool = ThreadPool_New(5);
    // Create a queue with 5 tasks
    struct ThreadQueue* queue = ThreadQueue_New(5);
    // Assign each queue item to execute 'sleep_in_thread'
    for (int i = 0; i < 5; i++) {
        queue->info[i].Function = sleep_in_thread;
    }
    // Submit the queue to the pool
    // Since it's 5 threads and 5 tasks, they should all complete in around 1 second.
    thrd_t primary_thread = ThreadPool_SubmitQueue(pool, queue);
    assert(ThrdGood(primary_thread));
    time_t start = time(NULL);
    // Wait for the pool to finish processing the queue
    int status = thrd_join(primary_thread, NULL);
    assert(status == thrd_success);
    ThreadQueue_Free(queue);
    ThreadPool_Free(pool);
    time_t end = time(NULL);
    time_t elapsed_seconds = end - start;
    assert(elapsed_seconds > 1);
    assert(elapsed_seconds < 4);
}

void test_FullQueue() {
    // Create a pool which manages 2 workers
    struct ThreadPool* pool = ThreadPool_New(2);
    // Create a queue with 4 tasks
    struct ThreadQueue* queue = ThreadQueue_New(4);
    // Assign each queue item to execute 'sleep_in_thread'
    for (int i = 0; i < 4; i++) {
        queue->info[i].Function = sleep_in_thread;
    }
    // Submit the queue to the pool
    // Since it's 2 threads and 4 tasks, it should take roughly double the time to complete
    thrd_t primary_thread = ThreadPool_SubmitQueue(pool, queue);
    assert(ThrdGood(primary_thread));
    time_t start = time(NULL);
    // Wait for the pool to finish processing the queue
    int status = thrd_join(primary_thread, NULL);
    assert(status == thrd_success);
    ThreadQueue_Free(queue);
    ThreadPool_Free(pool);
    time_t end = time(NULL);
    time_t elapsed_seconds = end - start;
    assert(elapsed_seconds > 3);
    assert(elapsed_seconds < 8);
}

int main() {
    test_Pool();
    test_FullQueue();
}