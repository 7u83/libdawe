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


#include <stdio.h>

#include "drv/DaweAlsaDevice.h"

int main(int argc, char * argv[])
{
	const char * name;
	if (argc==2)
		name = argv[1];
	else
		name = "default";

	DaweAlsaDevice * d;
	try{
		d = new DaweAlsaDevice(name);
		d->print_info();
	}
	catch (std::exception &e){
		printf("ERROR: %s\n",e.what());
	}

	delete d;
}
