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
#include "MidiClip.h"
#include "MidiClipLink.h"

namespace Dawe {

void MidiClip::dump(){
	std::cout << "MidiData dump " << "[" << getName()
		  << "]" << " =============" << std::endl;
	std::cout << "MidiData Events:" << std::endl;
	event_list.dump();
	std::cout << "MidiData Tempo Events:" << std::endl;
	tempo_index.dump();
	std::cout << "MidiData Time Signatures:" << std::endl;
	timesig_index.dump();
}

Dawe::MidiClipLink *Dawe::MidiClip::createMidiClipLink(){
    MidiClipLink * link = new MidiClipLink(this);
    link->setName("huhu");
    link_list.push_back(link);
    return link;
}

}
