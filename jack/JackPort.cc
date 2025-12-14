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

#include "../dawe.h"
#include "../Exception.h"
#include "JackPort.h"


namespace Dawe {

JackPort::~JackPort()
{

}

JackPort::JackPort(jack_client_t *client, const char *name, Port::Type type)
{
	const char *jtype;
	switch (type){
	case Port::MIDI:
		jtype = JACK_DEFAULT_MIDI_TYPE;
		break;
	case Port::AUDIO:
	default:
		jtype = JACK_DEFAULT_AUDIO_TYPE;
	}

	port = jack_port_register (client, name, jtype,
				   JackPortIsOutput, 0);
	if (port == NULL)
		throw Exception("Can't register port %s",name);

}


}
