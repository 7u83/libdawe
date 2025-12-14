#ifndef MIDILISTINDEX_H
#define MIDILISTINDEX_H

#include <vector>

#include "MidiList.h"
#include "../VList.h"

#include "../algo/SkipList.h"

namespace Dawe
{


class MidiListIndex : public VList<nmiditicks_T,MidiEvent>
{
private:
	struct Cmp{
		bool operator()(XMidiList::iterator a,
				XMidiList::iterator b){

			return *a < *b;
		}
	};

public:
	MidiListIndex(XMidiList & list):list(list)
	{

	}

	void ins(MidiList::iterator it){
		if ((*it).getType()==MidiEvent::NOTE_ON){
//			ilist.insert(it);
		}
	}

	void rebuild(){
		ilist.clear();
		MidiList::iterator it;
		for (it=list.begin(); it!=list.end();++it){
			ins(it);
		}
	}

	SkipList< XMidiList::iterator, nmiditicks_T > index;

public:
	XMidiList & list;

	SkipList<XMidiList::iterator,nmiditicks_T,Cmp> ilist;

	std::vector<MidiList::iterator> array;


	// VList interface
public:
	virtual bool empty() const
	{
		return ilist.empty();
	}
	virtual size_t size() const
	{
		return ilist.size();
	}
	virtual MidiEvent *getValPtrAtIndex(int index) const
	{
		XMidiList::iterator it = *(ilist.getIteratorAtIndex(index));
		return &(*it);
	}
	virtual void deleteAtIndex(int index)
	{
	}
	virtual void insert(MidiEvent e)
	{
	}
};

} // namespace Dawe

#endif // MIDILISTINDEX_H
