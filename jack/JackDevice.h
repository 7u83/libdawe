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
#ifndef JACKDEVICE_H
#define JACKDEVICE_H


#include <iostream>

#include <jack/jack.h>
#include <jack/transport.h>

#include "../dawe.h"
#include "../Device.h"

#include "JackPort.h"

namespace Dawe {

class JackDevice : public Device
{
public:
	void setup();

	JackDevice(const char *name = "daweJackDevice",
		   const char *server_name="default")
		: Device::Device(name){

		this->server_name = server_name;
		setup_client();
	}

	~JackDevice();

	virtual int get_transport();
	virtual nframes_T get_current_frame();

	int activate(){
		return jack_activate(client);
	}

	JackPort * register_port(const char *name, Port::Type type);

protected:
	void setup_client();

	jack_client_t *client;
	const char *server_name;

	static int j_callback(jack_nframes_t nframes, void *arg){
		return ((JackDevice *)arg)->process_callback(nframes);
	}

	static int j_sync_callback(jack_transport_state_t state, jack_position_t *pos, void *arg){
		Transport::Sate dawe_state = convert_transport(state);
		return ((Device*)arg)->sync_callback(dawe_state,pos->frame);
	}
	static Transport::Sate convert_transport(jack_transport_state_t state);

	void TransportStart()
	{
		jack_transport_start(client);
	}

	void TransportStop()
	{
		jack_transport_stop(client);
	}
};

}

#endif // JACKDEVICE_H


