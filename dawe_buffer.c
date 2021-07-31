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

#include <malloc.h>

#include "dawe.h"

dawe_buffer_t * dawe_buffer_create(int channels){
	dawe_buffer_t *b;
	b=malloc(sizeof(dawe_buffer_t));
	if(!b)
		return NULL;
	b->data=malloc(sizeof(uint8_t*)*channels);
	if (!b->data){
		free(b);
		return NULL;
	}
	b->data[0]=NULL;
	b->channels=channels;
	return b;
}

uint8_t *
dawe_buffer_init(dawe_buffer_t * b,int frames, int frame_size)
{
	b->data[0]=malloc(frames*frame_size*b->channels);
	if (b->data[0])
		return NULL;
	return b->data[0];
}

