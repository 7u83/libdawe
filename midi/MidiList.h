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
#ifndef MIDILIST_H
#define MIDILIST_H

#include <list>

#include "MidiEvent.h"
//#include "MidiListAccessor.h"

#include "../algo/Iterator.h"
#include "../algo/List.h"

#include "../algo/SkipList.h"

namespace Dawe {



/**
 * @brief The MidiList class
 */
class MidiList : public List<nmiditicks_T,MidiEvent>
{
public:



	MidiList() {
		list = std::list<MidiEvent>();
	}

	virtual MidiEvent * getFirst() const {
		if (list.empty())
			return 0;
		return &(*list.begin());
	}

	virtual void eraseFirst(){
		list.erase(list.begin());
	}

	virtual void dump();

	virtual iterator begin() const { return list.begin(); }
	virtual iterator end() const { return list.end(); }
	bool empty() const {return list.empty();}

protected:
	//	mutable std::list<MidiEvent> lost;

	iterator insert(iterator pos, const MidiEvent& value) {
		printf("Inerting one element here");
		value.dump();
		return list.insert(pos, value);
	}

	iterator erase(iterator it){
		return list.erase(it);

	}


public:
	void push_back(const MidiEvent& value) { list.push_back(value); }
	void push_front(const MidiEvent& value) { list.push_front(value); }


};

struct LTimeComperator {

	bool operator()(std::list<MidiEvent>::iterator & a, nmiditicks_T time ) const {
		// compare absolute values
		return (*a).getTime() < time;
	}

};

class YMidiList : public XList<nmiditicks_T,MidiEvent,LTimeComperator>
{
public:
	virtual void dump();
	class MidiListIndex : public XList<nmiditicks_T,MidiEvent,LTimeComperator>::Index{


		// Index interface
	public:
		MidiListIndex(YMidiList * list): XList<nmiditicks_T,MidiEvent,LTimeComperator>::Index(list)
		{

		}

		virtual bool filter(const MidiEvent &e ) const {
			if (filter_type==0xff)
				return true;
			return e.getCombinedType() == filter_type;
		}


		void setFilter(MidiEvent::Type type){
			filter_type=type;
			rebuild();
		}
		void setFilter(MidiEvent::MetaType type){
			filter_type=(0xff<<8)+type;
			rebuild();
		}
	private:
		uint16_t filter_type;
	};

	YMidiList(){
/*		MidiListIndex * index = new MidiListIndex(this);
		addIndex(index);
		index->setFilter(MidiEvent::TIMESIG);
*/	}


	// VList interface

};



class XMidiList : public SkipList<MidiEvent,nmiditicks_T>, public VList<nmiditicks_T, MidiEvent>
{
public:
	virtual void dump();
	class MidiListIndex : public XList<nmiditicks_T,MidiEvent,LTimeComperator>::Index{


		// Index interface
	public:
/*		MidiListIndex(XMidiList * list): XList<nmiditicks_T,MidiEvent,LTimeComperator>::Index(list)
		{

		}
*/
		virtual bool filter(const MidiEvent &e ) const {
			if (filter_type==0xff)
				return true;
			return e.getCombinedType() == filter_type;
		}


		void setFilter(MidiEvent::Type type){
			filter_type=type;
			rebuild();
		}
		void setFilter(MidiEvent::MetaType type){
			filter_type=(0xff<<8)+type;
			rebuild();
		}
	private:
		uint16_t filter_type;
	};

	XMidiList(){
//		MidiListIndex * index = new MidiListIndex(this);
//		addIndex(index);
//		index->setFilter(MidiEvent::TIMESIG);
	}


	// VList interface


	// VList interface
public:
	virtual bool empty() const
	{
		return SkipList<MidiEvent,nmiditicks_T>::empty();
	}
	virtual size_t size() const
	{
		return SkipList<MidiEvent,nmiditicks_T>::size();
	}
	virtual MidiEvent *getValPtrAtIndex(int index) const
	{
		return SkipList<MidiEvent,nmiditicks_T>::getValPtrAtIndex(index);
	}

	// VList interface
public:

	virtual void deleteAtIndex(int index)
	{
		SkipList<MidiEvent,nmiditicks_T>::deleteAtIndex(index);
	}

	// VList interface
public:
	virtual void insert(MidiEvent e)
	{
		SkipList<MidiEvent,nmiditicks_T>::insert(e);
	}
};


} // namespace Dawe


#endif // MIDIEVENTLIST_H
