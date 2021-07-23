#ifndef _DAWE_TPOOL_H
#define _DAWE_TPOOL_H

#include <pthread.h>
#include <semaphore.h>

struct dawe_tpool {
	int size;
	sem_t job_avail;
	sem_t job_taken;
	void (*job_fun)(void*);
	void *job_arg;

	pthread_t * threads;
};
typedef struct dawe_tpool dawe_tpool_t;

dawe_tpool_t * dawe_tpool_create(int size);
int dawe_tpool_add_job(dawe_tpool_t *pool,void (*fun)(void*), void *arg);

#endif
