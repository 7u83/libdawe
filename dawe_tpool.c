#include <stdlib.h>
#include <stdio.h>

#include "dawe_tpool.h"


static void * worker(void * param)
{
	dawe_tpool_t * p = (dawe_tpool_t*)param;
	while(1){
		void * arg;
		void (*fun)(void *);
		sem_wait(&p->job_avail);
		arg = p->job_arg;
		fun =p->job_fun;
		sem_post(&p->job_taken);
		fun(arg);
	}
}

dawe_tpool_t * dawe_tpool_create(int size)
{
	dawe_tpool_t *pool;
	int i;

	pool = malloc(sizeof(dawe_tpool_t));
	if (pool==NULL)
		return NULL;
	pool->threads = malloc(sizeof(pthread_t)*size);

	if (sem_init(&pool->job_avail,0,0)){
		printf("can't open semaphore\n");
		return 0;
	}

	if (sem_init(&pool->job_taken,0,0)){
		printf("can't open semaphore\n");
		return 0;
	}

	pool->size=0;
	for (i=0;i<size;i++){
		int fc;
		if (pthread_create(&(pool->threads[i]), NULL, worker, pool))
			continue;
		pool->size++;
	}

	return pool;
}

int dawe_tpool_add_job(dawe_tpool_t *pool,void (*fun)(void*), void *arg)
{
	pool->job_fun=fun;
	pool->job_arg=arg;
	sem_post(&pool->job_avail);
	sem_wait(&pool->job_taken);
	return 0;
}

