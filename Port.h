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
#ifndef DAWEPORT_H
#define DAWEPORT_H

#include "dawe.h"
#include "sysdefs.h"

namespace Dawe {

class Port
{
public:
	enum flags{
		TERMINAL = 1,
		HARDWARE = 2,
		INPUT = 4,
		OUTPUT = 8
	};

	typedef enum {
		AUDIO,
		MIDI
	} Type;

	Port();
	virtual ~Port();

	virtual nframes_T get_latency(){ return 0; }
	virtual void * get_buffer(nframes_T nframes){
		(void)nframes;
		return 0;
	}
	uint32_t flags;




};

}

#endif // DAWEPORT_H
