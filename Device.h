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
#ifndef DAWEDEVICE_H
#define DAWEDEVICE_H

#include <string>
#include <vector>


#include "dawe.h"


#include "Port.h"

namespace Dawe {

class Device
{
public:
	Device(const char * name = "daweDevice")
	{
		this->name = name;
	}

	virtual ~Device(){
		for (unsigned int i=0; i< this->ports.size(); i++){
			delete ports[i];
		}
	}


	class Transport {
	public:
		typedef enum {
			ROLLING,
			STOPPED,
			STARTING,
			LOOPING,
			UNDEF
		} Sate;
	};






	std::vector<Port *> ports;

	virtual Port * register_port(const char * name, Port::Type type){
		(void)name;
		(void)type;
		return 0;
	}



	std::string name;
	uint16_t num_inputs;
	uint16_t num_outputs;

	int type;
	int dir;
	uint32_t latency;

	virtual int get_transport(){
		return Transport::STOPPED;
	}
	virtual nframes_T get_current_frame(){
		return 0;
	}


	inline void run() {
		return;
	}

	virtual int activate(){
		return 0;
	}

	// called by constructor
	void setup(const char * name="daweDevice"){
		(void)name;
	}

	/**
	 * @brief process_callback
	 * @param nframes Number of frames to process
	 * @return zero on success, otherwise error
	 */
	virtual int process_callback(uint32_t nframes){
		(void)nframes;
		return 0;
	}

	virtual int sync_callback(Transport::Sate state, uint32_t frame){
		(void)state;
		(void)frame;
		return 1;
	}

	virtual void TransportStop(){}
	virtual void TransportStart(){}



};

}

#endif // DAWEDEVICE_H
