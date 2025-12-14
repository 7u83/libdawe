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
#include "old_MidiInstrument.h"

#include <cstdio>

namespace Dawe {

static const char *midi_note_names[12]={"C ","C#","D ","D#","E ","F ",
				 "F#","G ","G#","A ","A#","B "};


void MidiInstrument::initDefaultNoteNames()
{
	for(uint8_t pitch=0; pitch<128; pitch++){
		char nn[16];
		uint8_t octave = ( pitch/12 );
		snprintf(nn,sizeof(nn),"%s%i",midi_note_names[pitch%12],octave-octave_shift);
		std::string note_name(nn);
		default_note_names[pitch]=note_name;
	}
}

} // namespace Dawe
