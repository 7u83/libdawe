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
#include "MidiIndex.h"


namespace Dawe {
MidiIndex::MidiIndex(MidiList &list) :
	list(list)
{
	filter_type=0xff;
}

void MidiIndex::insert(MidiList::iterator li){

	if (!filter(*li))
		return;

	MidiList::iterator ti=li;

	MidiEvent &e = (*li);
	while(li!=list.begin()){
		--li;
		if ( (*li).getTime() == e.getTime()){
			if (filter(*li))
				ti=li;
			continue;
		}
	}
	map[index_key(e)]=ti;
}



void MidiIndex::erase(MidiList::iterator it){
	if (!filter(*it))
		return;

	MidiList::iterator it0 = map[index_key(*it)];
	printf("First Erase Index found: ");
	(*it0).dump();

	while (it0!=list.end()){
		printf("Check it0: "); (*it0).dump();

		if (index_key(*it0) != index_key(*it) ){
			printf("Break\n");
			break;
		}
		if ( filter(*it0) && (it0!=it)){
			printf("YES\n");
			map[index_key(*it0)]=it0;
			return;
		}

		++it0;
	}
	map.erase(map.find(index_key(*it)));
	return;
}

MidiEvent * MidiIndex::getLatest(MidiList::iterator li){

	nmiditicks_T t = (*li).getTime();
	MidiEvent *e=0;

	while(li!=list.end()){
		if ((*li).getTime()!=t)
			break;

		if (filter(*li))
			e=&(*li);
		++li;
	}
	return e;
}


MidiEvent *MidiIndex::getLatest(long index){
	if (map.empty())
		return 0;

	std::map<unsigned long,MidiList::iterator>::iterator it =
			map.lower_bound(index);
	if (it!=map.begin())
		--it;
	MidiList::iterator li = it->second;
	return getLatest(li);
}

} // namespace Dawe
