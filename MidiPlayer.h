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
#ifndef MIDPLAYER_H
#define MIDPLAYER_H

#include <set>

#include "jack/JackDevice.h"

#include "midi/MidiFile.h"
#include "midi/MidiEventQueue.h"

#include "Session.h"

namespace Dawe {

class MidiPlayer : public JackDevice
{
public:
	MidiPlayer(Session *sess, const char * name = "DaweMidiPlayer");
	void play();
	int process_callback(uint32_t nframes);
	int old_process_callback(uint32_t nframes);

	MidiFile f;
	uint32_t last;

	MidiFile::MidiTrack theTrack;



	miditempo_T tempo;
	std::set<MidiEvent>::iterator it;
	uint32_t frame_offset;
	uint32_t tick_offset;

	void setQeueue(MidiEventQueue *q){
		queue=q;
	}

	void start(){
		tempo = 500000; // default 120bpm
		frame_offset=0;
		tick_offset=0;

		activate();
	}

private:
	MidiEventQueue * queue;

	Session * sess;

};

}

#endif // MIDPLAYER_H
