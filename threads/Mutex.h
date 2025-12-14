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
#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <string.h>

#include "../dawe.h"
#include "../Exception.h"

namespace Dawe {

/**
 * @brief The Mutex class
 */
class Mutex
{
public:

	inline Mutex()
	{
		int rc = pthread_mutex_init(&mutex,NULL);
		if (rc){
			throw Exception("thread_mutex_init failed %d - %s",rc, strerror(rc));
		}
	}

	inline ~Mutex(){
		pthread_mutex_destroy(&mutex);
	}

	inline int lock(){
		return pthread_mutex_lock(&mutex);
	}

	inline int unlock(){
		return pthread_mutex_unlock(&mutex);
	}

	inline int trylock(){
		return pthread_mutex_trylock(&mutex);
	}

private:
	pthread_mutex_t mutex;

};

} // namespace

#endif // MUTEX_H
