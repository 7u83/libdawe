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
#ifndef MIDITEMPOIDX_H
#define MIDITEMPOIDX_H

#include "MidiEventListIdx.h"
#include "MidiList.h"

namespace Dawe{

class MidiTempoIdx
{
public:
	MidiTempoIdx(MidiList::midieventlist_T & list):
		list(list),
		tempo_by_tick(list),
		tempo_by_frame(list,MidiEventListIdx::FRAMEIDX)
	{
		tempo_by_frame.setFilter(MidiEvent::TEMPO);
		tempo_by_tick.setFilter(MidiEvent::TEMPO);
	}

	void insert(MidiList::midieventlist_T::iterator li){






		int lsr = 48000;
		int ltpb = 960;

		if (tempo_by_frame.empty()){
			int ltempo = 500000;

			nframes_T frames = ((double)ltpb *(double)lsr
					    * (double)(*li).getTime())/ltempo;

			printf("SET FRAMES: %d\n",frames);

			(*li).setFrame(frames);

			printf("VERIFY FRAMES: %d\n",(*li).getFrame());
			tempo_by_frame.insert(li);
			tempo_by_tick.insert(li);

			return;
		}

		const MidiEvent *e = tempo_by_tick.getLatest((*li).getTime());
		if (!e){
			printf("E IS NULL\n");
			return;
		}

		printf("Found E with: %d, %d\n",e->getTime(),e->getFrame());
		nframes_T frames = e->getFrame()+
				((double)ltpb *(double)lsr
				    * ((double)(*li).getTime()-(double)e->getTime()))/e->getTempo();

		printf("New Frames: %d\n", frames);
		(*li).setFrame(frames);


		tempo_by_frame.insert(li);
		tempo_by_tick.insert(li);

		MidiEventListIdx::xmap_T::iterator it;
		it = tempo_by_frame.find(frames);
		e=tempo_by_frame.getLatest(it->second);

		++it;
		while(it!=tempo_by_frame.end()){
			MidiEvent * en=tempo_by_frame.getLatest(it->second);
			frames = e->getFrame()+
					((double)ltpb *(double)lsr *
					 ((double)e->getTime()-(double)en->getTime()))/
					e->getTempo();
			en->setFrame(frames);

			//MidiEventList::midieventlist_T::iterator next_li = it->second;
		}

	}
private:
	MidiList::midieventlist_T & list;
	MidiEventListIdx tempo_by_tick;
	MidiEventListIdx tempo_by_frame;
};

} // namespace Dawe

#endif // MIDITEMPOIDX_H
