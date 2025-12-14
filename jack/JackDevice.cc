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


#include <iostream>
#include <sstream>
#include <string>


#include "../Exception.h"
#include "JackDevice.h"
#include "JackPort.h"

namespace Dawe {


void JackDevice::setup_client()
{
	jack_status_t status = (jack_status_t)0;

	client = jack_client_open (name.c_str(), (jack_options_t)(JackServerName | JackNoStartServer), &status, server_name);
	if (!client){
		throw Exception("Can't connect to jack server '%s': %02X",server_name, status);
	}
	int rc;
	rc = jack_set_process_callback (client, this->j_callback, this);

	if (rc)
		throw Exception("Can't set process callback for jack client '%s'",name.c_str());

	rc = jack_set_sync_callback(client,j_sync_callback,this);
	if (rc)
		throw Exception("Can't set sync callback for jack client '%s'",name.c_str());
}



JackDevice::~JackDevice()
{
	if (client)
		jack_client_close(client);
}


void JackDevice::setup()
{
	std::cout << "Jack Device stup" << std::endl;
}

Device::Transport::Sate JackDevice::convert_transport(jack_transport_state_t state)
{
	switch (state) {
	case JackTransportStopped:
		return Transport::STOPPED;
	case JackTransportRolling:
		return Transport::ROLLING;
	case JackTransportStarting:
		return Transport::STARTING;
	case JackTransportLooping:
		return Transport::LOOPING;
	default:
		return Transport::UNDEF;
	}
}

int JackDevice::get_transport()
{
	jack_position_t current;
	jack_transport_state_t transport_state;
	transport_state = jack_transport_query (client, &current);

	return convert_transport(transport_state);

}

nframes_T JackDevice::get_current_frame()
{
	jack_position_t current;
//	jack_transport_state_t transport_state;
//	transport_state =
	jack_transport_query (client, &current);

	return current.frame;

}


JackPort * JackDevice::register_port(const char *name, Port::Type type)
{
	JackPort * p;
	try {
		p = new JackPort(client,name,type);
	}catch ( Exception& e) {
		std::cout << e.what() << std::endl;
		return 0;
	}
	ports.push_back(p);
	return p;
}



}
