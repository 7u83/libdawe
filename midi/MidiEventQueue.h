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
#ifndef MIDIEVENTQUEUE_H
#define MIDIEVENTQUEUE_H

#include <map>

#include "MidiList.h"

namespace Dawe {


class MidiEventQueue : public MidiList
{
public:
	MidiEventQueue()
	{

	}

	void insert(MidiEvent & event){
		std::map<nmiditicks_T,iterator>::iterator it0,it;

		it0=it = map.lower_bound(event.getTime());

		if (it!=map.end()){
			iterator li;
			if ((*it->second).getTime()==event.getTime()){
				li=it->second;
				while(li!=end()){
					if((*li).getTime()!=event.getTime())
						break;
					++li;
				}
				MidiList::insert(li,event);
				return;
			}

			li=MidiList::insert(it->second,event);
			//map.insert(event.getTime(),li);
			map[event.getTime()]=li;
		}
		else{
			iterator li;
			push_back(event);
			li=end();
			--li;
			map[event.getTime()]=li;
		}

	}

	virtual void dump() {
		MidiList::dump();
		std::map<nmiditicks_T,iterator>::iterator it;
		fprintf(stderr,"---\n");

		for(it=map.begin(); it!=map.end(); ++it){
			MidiEvent e = *it->second;
			fprintf(stderr,"MAP: T:%d, %d %d\n",e.getTime(),e.getType(),e.getData1());
		}
	}

	void eraseFirst(){
		MidiList::eraseFirst();
		map.erase(map.begin());
		if (list.empty())
			return;
		map[(*list.begin()).getTime()]=list.begin();
	}

protected:
	std::map<nmiditicks_T,MidiList::iterator> map;
};

} //namespace Dawe

#endif // MIDIEVENTQEUEU_H
