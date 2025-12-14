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
#ifndef MIDIINDEX_H
#define MIDIINDEX_H

#include <map>
//#include "../old_Iterator.h"

#include "MidiList.h"
//#include "MidiMapAccessor.h"

namespace Dawe {

/**
 * @brief An Index on certain events in a #MidiList object
 */
class MidiIndex
{
public:

	MidiIndex(MidiList & list);

	virtual void insert(MidiList::iterator li);
	virtual bool filter(MidiEvent &e ) const {
		if (filter_type==0xff)
			return true;
		return e.getCombinedType() == filter_type;
	}
	virtual unsigned long index_key(MidiEvent &e) const {
		return e.getTime();
	}
	virtual void erase(MidiList::iterator it);

	void setFilter(MidiEvent::Type type){
		filter_type=type;
	}
	void setFilter(MidiEvent::MetaType type){
		filter_type=(0xff<<8)+type;
	}

	MidiEvent * getLatest(MidiList::iterator li);

	MidiEvent *getLatest(long index);


	class accessor : public Accessor<nmiditicks_T,MidiEvent> {
	public:
		accessor(MidiIndex & index) : index(index) {
			mit = index.map.begin();
		}

		void moveForward(){++mit;}
		void moveBackward(){--mit;}
		MidiEvent *getValPtr() const{
			return index.getLatest(mit->second);
		}

		bool isAtEnd() const {
			return mit == index.map.end();
		}

		bool isAtBegin() const{
			return mit == index.map.begin();
		}

		void seek(nmiditicks_T pos) {
			mit = index.map.lower_bound(pos);
			if (mit!=index.map.begin())
				mit --;
		}
		void seekBegin(){
			mit = index.map.begin();
		}

		nmiditicks_T getKey() const{
			return mit->first;
		}
	private:
		MidiIndex & index;
		std::map<unsigned long,MidiList::iterator>::iterator mit;

	public:

	};

	accessor  getAccessor(){
		return accessor(*this);
	}

	accessor * createAccessor(){
		return new accessor(*this);
	}

	void dump(){
		accessor a = getAccessor(); //*this);
		for(a.seekBegin();!a.isAtEnd();a.moveForward()){
			MidiEvent *e = a.getValPtr();
			std::cout << "INDEX DUMP: ";
			e->dump();
		}
	}
	typedef std::map<unsigned long,MidiList::iterator>::iterator iterator;

	iterator begin() { return map.begin(); }
	iterator end() { return map.end(); }
	bool empty() const {return map.empty();}

private:
	MidiList & list;
	std::map<unsigned long,MidiList::iterator> map;
	uint16_t filter_type;
};
}

#endif // MIDIINDEX_H
