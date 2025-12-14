/*
 * Copyright (C) 2023 7u83 <7u83@mail.ru>
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
#include "MidiList.h"

namespace Dawe {

void MidiList::dump()
{

	accessor a = getAccessor(); //(*this);


	//printf("IS AT END=: %d\n",a.isAtEnd());
	//a=createAcessor();

	MidiEvent *e0;
	for ( ; !a.isAtEnd(); a.moveForward()){
		e0 = a.getValPtr();
		printf("MidiList DUMP:"); e0->dump();
	}
	//delete a;
	return;

	iterator  it(begin());

	for (it=begin(); it!=end(); ++it){
		MidiEvent e = *it;
		printf("DUMP [%d]",e.getTime());
		e.dump();

		/*
		fprintf(stderr,"DUMP: T:%d, %02x,%02x %d\n",e.getTime(),
			e.getMessageType(),e.getMetaEventCode(),
			e.getData1());*/
	}
}

void XMidiList::dump()
{

//	accessor a = getAccessor(); //(*this);


	//printf("IS AT END=: %d\n",a.isAtEnd());
	//a=createAcessor();

/*	MidiEvent *e0;
	for ( ; !a.isAtEnd(); a.moveForward()){
		e0 = a.getValPtr();
		printf("MidiList DUMP:"); e0->dump();
	}
	//delete a;
	return;
*/

	iterator  it(begin());

	for (it=begin(); it!=end(); ++it){
		MidiEvent e = *it;
		printf("DUMP [%d]",e.getTime());
		e.dump();

		/*
		fprintf(stderr,"DUMP: T:%d, %02x,%02x %d\n",e.getTime(),
			e.getMessageType(),e.getMetaEventCode(),
			e.getData1());*/
	}
}



} // namespace Dawe

