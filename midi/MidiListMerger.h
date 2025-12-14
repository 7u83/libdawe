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
#ifndef MIDILISTMERGER_H
#define MIDILISTMERGER_H

#include <vector>

//#include "MidiList.h"
//#include "MidiListAccessor.h"

namespace Dawe {

class MidiListMerger
{
public:
/*	MidiListMerger();

	MidiEvent * getNext(){
		MidiEvent *e;

		MidiListAccessor *a=lists[0];
		e = a->getValue();

		for (size_t i=1; i< lists.size(); i++){
			MidiEvent *e1 = lists[i]->getValue();
			if (e->getTime() > e1->getTime()){
				e = e1;
				a=lists[i];
			}
		}
		a->moveForward();
		return e;
	}

private:
	std::vector <MidiListAccessor *> lists;

*/

};

} // namespace Dawe

#endif // MIDILISTMERGER_H
