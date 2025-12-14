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
#ifndef MIDICLIP_H
#define MIDICLIP_H

#include <vector>

#include "../midi/MidiEventArrayList.h"
#include "../midi/MidiIndex.h"
#include "../algo/Accessor.h"

#include "Clip.h"



namespace Dawe {

class MidiClipLink;

class MidiClip : public Clip
{
public:
	MidiClip(const char * name ="Clip"):
		Clip(name),
		tempo_index(event_list),
		timesig_index(event_list)
	{
		tempo_index.setFilter(MidiEvent::TEMPO);
		timesig_index.setFilter(MidiEvent::TIMESIG);
	}

	void insert(MidiEvent & e){
		MidiList::iterator it;
		it = event_list.insert(e);
		tempo_index.insert(it);
		timesig_index.insert(it);

		if (!tempo_index.empty()){
			//song->
		}
	}

	void dump();


/*	const std::string & getName() const {
		return name;
	}
*/

	typedef Accessor<nmiditicks_T,MidiEvent> accessor;

	Accessor<nmiditicks_T,MidiEvent> * createAaccessor() {
		return event_list.createAccessor();
	}

	Accessor<nmiditicks_T,MidiEvent> * createTimeSigAaccessor() {
		return timesig_index.createAccessor();
	}

	MidiIndex * getTimeSigIndex(){
		return &timesig_index;
	}

    MidiClipLink * createMidiClipLink();

	typedef std::vector<MidiClipLink*> midicliplinklist_T;

	const midicliplinklist_T & getLinks() const{
		return link_list;
	}
private:
//	std::string name;

	MidiEventArrayList event_list;
	MidiIndex tempo_index;
	MidiIndex timesig_index;

	midicliplinklist_T link_list;
};

}

#endif // MIDIDATA_H
