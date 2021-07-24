/*
 * Copyright (C) 2021  7u83 <7u83@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 */
/**
 * @file
 * @brief Implementation of dawe_tool functions
 */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "dawe_tpool.h"

/**
 * @brief worker
 * @param param
 * @return
 */
static void * worker(void * param)
{
	void * arg;
	void (*fun)(void *);
	dawe_tpool_t * p = (dawe_tpool_t*)param;

	while(1){
		sem_wait(&p->job_avail);
		arg = p->job_arg;
		fun = p->job_fun;
		sem_post(&p->job_taken);
		fun(arg);
	}
	return NULL;
}

/**
 * @brief Create a thread poll
 * @param size Number of threads to be kept ready for use.
 * @return Null on error
 */
dawe_tpool_t * dawe_tpool_create(int size)
{
	dawe_tpool_t *pool;
	int i;
	pthread_attr_t attr;

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

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	pool->size=0;
	for (i=0;i<size;i++){

		if (pthread_create(&pool->threads[i], &attr, worker, pool))
			continue;
		pool->size++;
	}

	pthread_attr_destroy(&attr);
	return pool;
}

/**
 * @brief dawe_tpool_add_job
 * @param pool A pointer to a #dawe_tpool_t object, initialized
 *	  by #dawe_tpool_create.
 * @param fun The function to be called by this job
 * @param arg
 * @return
 */
int dawe_tpool_add_job(dawe_tpool_t *pool,void (*fun)(void*), void *arg)
{
	pool->job_fun=fun;
	pool->job_arg=arg;
	sem_post(&pool->job_avail);
	sem_wait(&pool->job_taken);
	return 0;
}

