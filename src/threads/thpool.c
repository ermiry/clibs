// /* ********************************
//  * Original Author:       Johan Hanssen Seferidis
//  * License:	     MIT
//  ********************************/

#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#if defined(__linux__)
#include <sys/prctl.h>
#endif

#include "../../include/threads/thpool.h"
#include "../../include/threads/bsem.h"
#include "../../include/threads/jobs.h"

#ifdef THPOOL_DEBUG
#define THPOOL_DEBUG 1
#else
#define THPOOL_DEBUG 0
#endif

#if !defined(DISABLE_PRINT) || defined(THPOOL_DEBUG)
#define err(str) fprintf(stderr, str)
#else
#define err(str)
#endif

static volatile int threads_keepalive;
static volatile int threads_on_hold;

#pragma region thread

struct _PoolThread {

	int id;
	pthread_t thread_id;
	Thpool *thpool;

};

typedef struct _PoolThread PoolThread;

static PoolThread *pool_thread_new (void) {

	PoolThread *thread = (PoolThread *) malloc (sizeof (PoolThread));
	if (thread) {
		thread->id = -1;
		thread->thread_id = 0;
		thread->thpool = NULL;
	}

	return thread;

}

static void pool_thread_delete (void *thread_ptr) {

	if (thread_ptr) free (thread_ptr);

}

static PoolThread *pool_thread_create (int id, Thpool *thpool) {

	PoolThread *thread = pool_thread_new ();
	if (thread) {
		thread->id = id;
		thread->thpool = thpool;
	}

	return thread;

}

#pragma endregion

#pragma region thpool

static Thpool *thpool_new (void) {

	Thpool *thpool = (Thpool *) malloc (sizeof (Thpool));
	if (thpool) {
		thpool->n_threads = 0;
		thpool->threads = NULL;
		thpool->num_threads_alive = 0;
		thpool->num_threads_working = 0;

		thpool->mutex = NULL;
		thpool->threads_all_idle = NULL;

		thpool->job_queue = NULL;
	}

	return thpool;

}

void thpool_delete (void *thpool_ptr) {

	if (thpool_ptr) {
		Thpool *thpool = (Thpool *) thpool_ptr;

		if (thpool->threads) {
			for (unsigned int i = 0; i < thpool->n_threads; i++) {
				pool_thread_delete (thpool->threads[i]);
			}

			free (thpool->threads);
		}

		pthread_mutex_destroy (thpool->mutex);
		free (thpool->mutex);

		pthread_cond_destroy (thpool->threads_all_idle);
		free (thpool->threads_all_idle);

		job_queue_delete (thpool->job_queue);

		free (thpool_ptr);
	}

}

#pragma endregion


/* Thread */
struct thread {
	int       id;                        /* friendly id               */
	pthread_t pthread;                   /* pointer to actual thread  */
	struct thpool_* thpool_p;            /* access to thpool          */
};


/* Threadpool */
struct thpool_{
	struct thread**   threads;                  /* pointer to threads        */
	volatile int num_threads_alive;      /* threads currently alive   */
	volatile int num_threads_working;    /* threads currently working */
	pthread_mutex_t  thcount_lock;       /* used for thread count etc */
	pthread_cond_t  threads_all_idle;    /* signal to thpool_wait     */
	// struct jobqueue  _jobqueue;                  /* job queue                 */
	JobQueue *job_queue;
};

/* ========================== PROTOTYPES ============================ */


static int  thread_init(struct thpool_* thpool_p, struct thread** thread_p, int id);
static void* thread_do(struct thread* thread_p);
static void  thread_hold(int sig_id);
static void  thread_destroy(struct thread* thread_p);

/* ========================== THREADPOOL ============================ */


/* Initialise thread pool */
struct thpool_* thpool_init(int num_threads){

	threads_on_hold   = 0;
	threads_keepalive = 1;

	if (num_threads < 0){
		num_threads = 0;
	}

	/* Make new thread pool */
	struct thpool_* thpool_p;
	thpool_p = (struct thpool_*)malloc(sizeof(struct thpool_));
	if (thpool_p == NULL){
		err("thpool_init(): Could not allocate memory for thread pool\n");
		return NULL;
	}
	thpool_p->num_threads_alive   = 0;
	thpool_p->num_threads_working = 0;

	/* Initialise the job queue */
	// if (jobqueue_init(&thpool_p->_jobqueue) == -1){
	// 	err("thpool_init(): Could not allocate memory for job queue\n");
	// 	free(thpool_p);
	// 	return NULL;
	// }

	thpool_p->job_queue = job_queue_create ();


	/* Make threads in pool */
	thpool_p->threads = (struct thread**)malloc(num_threads * sizeof(struct thread *));
	if (thpool_p->threads == NULL){
		err("thpool_init(): Could not allocate memory for threads\n");
		// jobqueue_destroy(&thpool_p->_jobqueue);
		free(thpool_p);
		return NULL;
	}

	pthread_mutex_init(&(thpool_p->thcount_lock), NULL);
	pthread_cond_init(&thpool_p->threads_all_idle, NULL);

	/* Thread init */
	int n;
	for (n=0; n<num_threads; n++){
		thread_init(thpool_p, &thpool_p->threads[n], n);
#if THPOOL_DEBUG
			printf("THPOOL_DEBUG: Created thread %d in pool \n", n);
#endif
	}

	/* Wait for threads to initialize */
	while (thpool_p->num_threads_alive != num_threads) {}

	return thpool_p;
}


/* Add work to the thread pool */
int thpool_add_work(struct thpool_* thpool_p, void (*function_p)(void*), void* arg_p){
	struct job* newjob;

	// newjob=(struct job*)malloc(sizeof(struct job));
	// if (newjob==NULL){
	// 	err("thpool_add_work(): Could not allocate memory for new job\n");
	// 	return -1;
	// }

	// /* add function and argument */
	// newjob->function=function_p;
	// newjob->arg=arg_p;

	/* add job to queue */
	// jobqueue_push(&thpool_p->_jobqueue, newjob);
	Job *job = job_create (function_p, arg_p);
	job_queue_push (thpool_p->job_queue, job);

	return 0;
}


/* Wait until all jobs have finished */
void thpool_wait(struct thpool_* thpool_p){
	// FIXME:
	// pthread_mutex_lock(&thpool_p->thcount_lock);
	// while (thpool_p->_jobqueue.len || thpool_p->num_threads_working) {
	// 	pthread_cond_wait(&thpool_p->threads_all_idle, &thpool_p->thcount_lock);
	// }
	// pthread_mutex_unlock(&thpool_p->thcount_lock);
}


/* Destroy the threadpool */
void thpool_destroy(struct thpool_* thpool_p){
	/* No need to destory if it's NULL */
	if (thpool_p == NULL) return ;

	volatile int threads_total = thpool_p->num_threads_alive;

	/* End each thread 's infinite loop */
	threads_keepalive = 0;

	/* Give one second to kill idle threads */
	double TIMEOUT = 1.0;
	time_t start, end;
	double tpassed = 0.0;
	time (&start);
	while (tpassed < TIMEOUT && thpool_p->num_threads_alive){
		bsem_post_all(thpool_p->job_queue->has_jobs);
		time (&end);
		tpassed = difftime(end,start);
	}

	/* Poll remaining threads */
	while (thpool_p->num_threads_alive){
		bsem_post_all(thpool_p->job_queue->has_jobs);
		sleep(1);
	}

	/* Job queue cleanup */
	// jobqueue_destroy(&thpool_p->_jobqueue);
	job_queue_delete (thpool_p->job_queue);
	/* Deallocs */
	int n;
	for (n=0; n < threads_total; n++){
		thread_destroy(thpool_p->threads[n]);
	}
	free(thpool_p->threads);
	free(thpool_p);
}


/* Pause all threads in threadpool */
void thpool_pause(struct thpool_* thpool_p) {
	int n;
	for (n=0; n < thpool_p->num_threads_alive; n++){
		pthread_kill(thpool_p->threads[n]->pthread, SIGUSR1);
	}
}


/* Resume all threads in threadpool */
void thpool_resume(struct thpool_* thpool_p) {
    // resuming a single threadpool hasn't been
    // implemented yet, meanwhile this supresses
    // the warnings
    (void)thpool_p;

	threads_on_hold = 0;
}


int thpool_num_threads_working(struct thpool_* thpool_p){
	return thpool_p->num_threads_working;
}





/* ============================ THREAD ============================== */


/* Initialize a thread in the thread pool
 *
 * @param thread        address to the pointer of the thread to be created
 * @param id            id to be given to the thread
 * @return 0 on LOG_SUCCESS, -1 otherwise.
 */
static int thread_init (struct thpool_* thpool_p, struct thread** thread_p, int id){

	*thread_p = (struct thread*)malloc(sizeof(struct thread));
	if (thread_p == NULL){
		err("thread_init(): Could not allocate memory for thread\n");
		return -1;
	}

	(*thread_p)->thpool_p = thpool_p;
	(*thread_p)->id       = id;

	pthread_create(&(*thread_p)->pthread, NULL, (void *(*)(void *)) thread_do, (*thread_p));
	pthread_detach((*thread_p)->pthread);
	return 0;
}


/* Sets the calling thread on hold */
static void thread_hold(int sig_id) {
    (void)sig_id;
	threads_on_hold = 1;
	while (threads_on_hold){
		sleep(1);
	}
}


/* What each thread is doing
*
* In principle this is an endless loop. The only time this loop gets interuppted is once
* thpool_destroy() is invoked or the program exits.
*
* @param  thread        thread that will run this function
* @return nothing
*/
static void* thread_do(struct thread* thread_p){

	/* Set thread name for profiling and debuging */
	char thread_name[128] = {0};
	sprintf(thread_name, "thread-pool-%d", thread_p->id);

#if defined(__linux__)
	/* Use prctl instead to prevent using _GNU_SOURCE flag and implicit declaration */
	prctl(PR_SET_NAME, thread_name);
#elif defined(__APPLE__) && defined(__MACH__)
	pthread_setname_np(thread_name);
#else
	err("thread_do(): pthread_setname_np is not supported on this system");
#endif

	/* Assure all threads have been created before starting serving */
	struct thpool_* thpool_p = thread_p->thpool_p;

	/* Register signal handler */
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = thread_hold;
	if (sigaction(SIGUSR1, &act, NULL) == -1) {
		err("thread_do(): cannot handle SIGUSR1");
	}

	/* Mark thread as alive (initialized) */
	pthread_mutex_lock(&thpool_p->thcount_lock);
	thpool_p->num_threads_alive += 1;
	pthread_mutex_unlock(&thpool_p->thcount_lock);

	while(threads_keepalive){

		bsem_wait(thpool_p->job_queue->has_jobs);

		if (threads_keepalive){

			pthread_mutex_lock(&thpool_p->thcount_lock);
			thpool_p->num_threads_working++;
			pthread_mutex_unlock(&thpool_p->thcount_lock);

			/* Read job from queue and execute it */
			void (*func_buff)(void*);
			void*  arg_buff;
			// struct job* job_p = jobqueue_pull(&thpool_p->_jobqueue);
			Job *job_p = job_queue_pull (thpool_p->job_queue);
			if (job_p) {
				func_buff = job_p->method;
				arg_buff  = job_p->args;
				func_buff(arg_buff);
				// free(job_p);
				job_delete (job_p);
			}

			pthread_mutex_lock(&thpool_p->thcount_lock);

			thpool_p->num_threads_working--;

			if (!thpool_p->num_threads_working) 
				pthread_cond_signal(&thpool_p->threads_all_idle);

			pthread_mutex_unlock(&thpool_p->thcount_lock);
		}
	}

	pthread_mutex_lock(&thpool_p->thcount_lock);
	thpool_p->num_threads_alive --;
	pthread_mutex_unlock(&thpool_p->thcount_lock);

	return NULL;
}


/* Frees a thread  */
static void thread_destroy (struct thread* thread_p){
	free(thread_p);
}