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
#ifndef JACKPORT_H
#define JACKPORT_H

#include <jack/jack.h>

#include "../dawe.h"
#include "../Port.h"


namespace Dawe {

class JackPort : public Port
{
public:
	JackPort(jack_client_t *client, const char * name, Port::Type type);
	~JackPort();

	void * get_buffer(uint32_t nframe){
		return jack_port_get_buffer (port, nframe) ;
	}

	nframes_T get_latency(){
		jack_latency_range_t range;
		jack_port_get_latency_range(port, JackPlaybackLatency,
					    &range);
		return range.max;
	}

private:
	jack_port_t * port;
};

} // namespace

#endif // JACKPORT_H
