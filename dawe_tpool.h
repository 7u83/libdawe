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
  */

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
};

/**
 * @defgroup API
 * @{
 */

/**
 * @brief dawe_tpool_t
 */
typedef struct dawe_tpool dawe_tpool_t;
dawe_tpool_t * dawe_tpool_create(int size);
int dawe_tpool_add_job(dawe_tpool_t *pool,void (*fun)(void*), void *arg);
void dawe_tpool_destroy(dawe_tpool_t *p);

/** @} */

#endif
