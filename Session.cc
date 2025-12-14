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

#include "Session.h"

#include "jack/JackDevice.h"

namespace Dawe {

/*
Session::Session(dawe_format_T format, int sampling_rate)
{
	this->format=format;
	this->sampling_rate=sampling_rate;
}
*/

Session::Session(const char *name, nmiditicks_T ticks_per_beat) :
	name(name),
	ticks_per_beat(ticks_per_beat),
	masterDevice(0)
{
	sampling_rate=48000;
}

Session::~Session()
{
	if (masterDevice)
		delete masterDevice;
}

void  Session::setup()
{
	masterDevice = new JackDevice(name,"default");
	masterDevice->register_port("MasterOut",Port::MIDI);
	masterDevice->activate();
}

void Session::print_info(){
    std::cout << "=== Dawe Session Info ===\n";
    std::cout << "Rate: " << this->sampling_rate << " Hz" << std::endl;

}

void Session::transportStart()
{
	std::cout << "Start Transport" << std::endl;
	masterDevice->TransportStart();
}

void Session::transportStop()
{
	std::cout << "Stop Transport" << std::endl;
	masterDevice->TransportStop();
}


}
