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
#ifndef MIDIEVENTLISTIDX_H
#define MIDIEVENTLISTIDX_H

#include <map>

#include "MidiList.h"

namespace Dawe {


class MidiEventListIdx
{
public:

	struct Filter{
		virtual bool filter(const MidiEvent *e){
			(void)e;
			printf("Always true\n");
			return true;
		}
	};
	struct Index{
		virtual long index(const MidiEvent *e){
			if (!e){
				//	printf("We are Index\n");
				return 0;
			}
			return e->getTime();
		}
	};
	struct FrameIndex : public Index {
		virtual long index(MidiEvent *e){
			if (!e){
				//	printf("We are Frame Index\n");
				return 0;
			}
			return e->getFrame();
		}
	};

	struct EventFilter : public Filter{

	protected:
		int eid;
	public:
		EventFilter(int eid):eid(eid){}
		bool filter(const MidiEvent *e){
			printf("SimpleFilter: "); e->dump();

			return e->getMessageType()==eid;
		}
	};

	struct MetaEventFilter : public EventFilter{
	public:
		MetaEventFilter(int id):EventFilter(id){}

		bool filter(const MidiEvent *e){

			printf("MeteFilter: "); e->dump();

			if (!e->isMeta())
				return false;
			return e->getMetaEventCode()==eid;
		}
	};

	void setFilter(MidiEvent::Type type){
		filter_pointer = new EventFilter(type);
	}

	void setFilter(MidiEvent::MetaType type){
		filter_pointer = new MetaEventFilter(type);
	}

	typedef enum{
		TICKIDX,
		FRAMEIDX
	} Type;

	typedef std::map<unsigned long,MidiList::midieventlist_T::iterator> xmap_T;
	xmap_T map;
	MidiList::midieventlist_T & list;

	MidiEventListIdx(MidiList::midieventlist_T & l,Type type=TICKIDX):
		list(l){
		switch (type) {
		case FRAMEIDX:
			index=new FrameIndex();
			break;
		case TICKIDX:
		default:
			index=new Index();
		}
		//filter=new Filter();
		filter_pointer=0;
		printf("Set Filter to %p",filter_pointer);
	}

	xmap_T::const_iterator begin() const{
		return map.begin();
	}
	xmap_T::const_iterator end() const{
		return map.end();
	}
	virtual void insert(MidiList::midieventlist_T::iterator li){
		printf("Calling Filter %p\n",filter_pointer);

		if (!this->filter_pointer->filter(&(*li)))
			return;

		printf("Insert: "); (*li).dump();

		MidiList::midieventlist_T::iterator ti=li;

		MidiEvent &e = (*li);
		while(li!=list.begin()){
			--li;
			if ( (*li).getTime() == e.getTime()){
				if (filter_pointer->filter(&(*li)))
					ti=li;
				continue;
			}
		}
		map[index->index(&e)]=ti;
	}

	virtual void erase(MidiList::midieventlist_T::iterator it){
		if (!filter_pointer->filter(&(*it)))
			return;

		MidiList::midieventlist_T::iterator it0 = map[index->index(&(*it))];
		printf("First Erase Index found: ");
		(*it0).dump();

		while (it0!=list.end()){
			printf("Check it0: "); (*it0).dump();

			if (index->index(&(*it0)) != index->index(&(*it)) ){
				printf("Break\n");
				break;
			}
			if ( filter_pointer->filter(&(*it0)) && (it0!=it)){
				printf("YES\n");
				map[index->index(&(*it0))]=it0;
				return;
			}

			++it0;
		}
		map.erase(map.find( index->index(&(*it))));
		return;














		MidiList::midieventlist_T::iterator itp,itn;
		itp=itn=it;
		if (itp!=list.begin()){
			itp--;
			if ((*itp).getTime() == (*it).getTime())
				return;
		}

		itn++;
		if (itn==list.end()){
			map.erase(map.find(/*(*it).getTime()*/ index->index(&(*it)) ));
			return;
		}
		if ((*itn).getTime() != (*it).getTime()) {

			map.find( index->index(&(*it)));

			map.erase(map.find( index->index(&(*it))));
			return;
		}
		map[index->index(&(*it))]=itn;
		return;

	}

	bool empty(){return map.empty();}

	xmap_T::iterator find(long index){
		return map.find(index);
	}

	const MidiEvent * getLatest(MidiList::midieventlist_T::const_iterator li) const{

		nmiditicks_T t = (*li).getTime();
		const MidiEvent *e=0;

		while(li!=list.end()){
			if ((*li).getTime()!=t)
				break;

			if (filter_pointer->filter(&(*li)))
				e=&(*li);
			++li;

		}
		return e;
	}

	const MidiEvent * getLatest(long index) const{
		if (map.empty())
			return 0;

		xmap_T::const_iterator it = map.lower_bound(index);
		if (it!=map.begin())
			--it;
		MidiList::midieventlist_T::const_iterator li = it->second;
		return getLatest(li);
	}

protected:
	Index * index;
	Filter * filter_pointer;


};

} // namespace Dawe

#endif // MIDIEVENTLISTIDX_H
