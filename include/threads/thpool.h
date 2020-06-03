#ifndef _THREADS_THPOOL_H_
#define _THREADS_THPOOL_H_

#include <stdbool.h>
#include <pthread.h>

#include "jobs.h"

struct _PoolThread;

typedef struct Thpool {

    const char *name;

    unsigned int n_threads;
    struct _PoolThread **threads;

    volatile bool keep_alive;
    volatile unsigned int num_threads_alive;
    volatile unsigned int num_threads_working;

    pthread_mutex_t *mutex;
    pthread_cond_t *threads_all_idle;

    JobQueue *job_queue;

} Thpool;

extern Thpool *thpool_create (unsigned int n_threads);

extern unsigned int thpool_init (Thpool *thpool);

extern void thpool_set_name (Thpool *thpool, const char *name);

extern int thpool_add_work (Thpool *thpool, void (*work) (void *), void *args);

extern void thpool_destroy (Thpool *thpool);

#endif