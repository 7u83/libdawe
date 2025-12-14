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
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <iostream>


#include "../dawe.h"

#include "../Exception.h"

namespace Dawe{

class Semaphore
{
public:
	Semaphore(int val = 0){
		int rc = sem_init(&sem,0,val);
		if (rc)
			throw Exception("Can't init semaphore");
	}
	~Semaphore(){
		sem_destroy(&sem);
	}

	void wait(){
		sem_wait(&sem);
	}
	void post(){
		int rc = sem_post(&sem);
		if (rc!=0){
			std::cout << "RC:" << rc << errno << std::endl;
			throw Exception("Can't post semahore %d: %s",rc,strerror(errno));
		}
	}

	int val(){
		int val;
		sem_getvalue((sem_t*)&sem, &val);
		return val;
	}
private:
	sem_t sem;
};

}

#endif // SEMAPHORE_H
