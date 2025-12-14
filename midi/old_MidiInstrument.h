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
#ifndef MIDIINSTRUMENT_H
#define MIDIINSTRUMENT_H

#include <string>
#include <map>

#include "../dawe.h"

namespace Dawe {

class MidiInstrument
{

public:
	typedef std::map<uint32_t,std::string> NameMap_T;
	typedef std::map<uint32_t,NameMap_T> NameMapMap_T;


	MidiInstrument(const char *name="Default",int octave_shift=0):
		instrument_name(name),
		octave_shift(octave_shift)
	{
		initDefaultNoteNames();
	}

	std::string getNoteName(uint8_t pitch, uint16_t bank=0,uint8_t patch=0) const {
		NameMapMap_T::const_iterator it;
		const NameMap_T *m;
		it=note_names.find((bank<<8)+patch);
		if(it==note_names.end()){
			m=&default_note_names;
		}
		else{
			m=&it->second;
		}
		NameMap_T::const_iterator nit = m->find(pitch);
		if (nit==m->end())
			return std::string();
		return nit->second;
	}
private:
	std::string instrument_name;
	int octave_shift;

	void initDefaultNoteNames();

	NameMap_T default_note_names;
	NameMapMap_T note_names;

};

} // namespace Dawe

#endif // MIDIINSTRUMENT_H
