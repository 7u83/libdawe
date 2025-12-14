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
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <cstddef>
#include <iostream>

#include "dawe.h"

#include "threads/Mutex.h"

namespace Dawe{

template <class T>
class RingBuffer
{
public:
	RingBuffer(size_t s, int n=1)
	{
		this->num=n;
		this->size=s;
		buffer = new T[size*num];
		reset();
	}

	~RingBuffer()
	{
		delete[] buffer;
	}

	inline void reset(){
		read = 0;
		write = 0;
		written=0;
	}

	inline size_t read_avail(){
		if (read + written > size)
			return size-read;
		return written;
	}

	inline size_t write_avail(){
		size_t a = size - written;
		if (write+a > size)
			return size-write;
		return a;

	}

	inline T * get_read_buf (){
		return buffer+read*num;
	}
	inline T * get_write_buf (){
		return buffer+write*num;
	}

	inline void update_read(size_t r){
		mutex.lock();
		written -=r;
		mutex.unlock();
		read+=r;
		if (read>=size)
			read=0;
	}

	inline void update_write(size_t w){
		mutex.lock();
		written+=w;
		mutex.unlock();
		write+=w;
		if(write>=size)
			write=0;
	}

private:
	Mutex mutex;
	int num;
	T *buffer;
	volatile size_t read;
	volatile size_t write;
	volatile size_t size;
	volatile size_t written;
};


}

#endif // RINGBUFFER_H
