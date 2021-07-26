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

#include <stdlib.h>
#include <string.h>

#include "dawe.h"

/**
 * @brief dawe_sess_create Create a dawe session object
 * @param sampling_rate the desired sample rate
 * @param bits_per_sample the number of bits per sample
 * @param format the desired format the created session will use
 * @param max_threads the maximum number of threads this session will use.
 * @return a handle to the creared session or NULL if an error has occcured.
 */
dawe_sess_t *
dawe_sess_create(
		uint32_t sampling_rate,
		uint16_t bits_per_sample,
		int format,
		uint32_t max_threads
		)
{
	dawe_sess_t * s;
	s = malloc(sizeof(dawe_sess_t));
	if (s==NULL){
		return NULL;
	}
	memset(s,0,sizeof(dawe_sess_t));

	s->sampling_rate=sampling_rate;
	s->bits_per_sample=bits_per_sample;
	s->format=format;

	if (max_threads<=0)
		max_threads=DAWE_SESS_MAX_THREADS;

	s->tpool = dawe_tpool_create(max_threads);
	if (!s->tpool){
		dawe_sess_destroy(s);
		return NULL;
	}

	return s;
}

/**
 * @brief Destroy a dawe session which was previously created
 *	  by #dawe_sess_create.
 * @param s The session handle to be destroyed
 */
void
dawe_sess_destroy(dawe_sess_t * s)
{
	if (!s)
		return;
	if (s->tpool){
		dawe_tpool_destroy(s->tpool);
	}
	free(s);
}
