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
#ifndef TRACK_H
#define TRACK_H

#include <map>
#include <set>
#include <list>

#include "../dawe.h"

#include "../Device.h"
#include "Clip.h"
//#include "MidiClip.h"
//#include "Track.h"
#include "../algo/List.h"
//#include "../midi/MidiMapAccessor.h"
#include "../algo/Accessor.h"
#include "../midi/MidiEvent.h"
#include "MidiClipList.h"

namespace Dawe {

class Track : Device
{
public:
	Track();
	/*	void insert(miditick_T pos, Clip *clip);
	void print();
*/

	void insert(miditick_T pos, Clip *clip);
	void dump();
	void remove(Clip *clip);

	bool hasClip(Clip *clip){
		return reverse.find(clip)!=reverse.end();
	}


private:
	struct ClipMapElem{
		ClipMapElem(int refs=0):refs(refs){}
		std::set<Clip*> clips;
		int refs;
		void insert(Clip *clip){
			clips.insert(clip);
			refs++;
		}
		void erase(Clip *clip){
			clips.erase(clip);
		}
	};

	//typedef std::map<miditick_T,std::set<Clip*> > clipmap_T;
	typedef std::map<miditick_T,ClipMapElem> clipmap_T;

	clipmap_T clips;
	std::map <Clip *,nmiditicks_T> reverse;


public:


	/*
	class accessor : public Accessor<nmiditicks_T,MidiEvent>
	{
		// MidiListAccessor interface
	public:
		accessor(Track & track):track(track){
			seekBegin();
		}
		void moveForward();
		void moveBackward(){}

		MidiEvent *getValPtr() const;
		nmiditicks_T getKey() const { return 0;}

		bool isAtEnd() const;
		bool isAtBegin() const {return true;}
		void seek(nmiditicks_T){return ;}
		void seekBegin(){
			current_accessor=0;
			clipmap_it=track.clips.begin();
			if (track.clips.empty())
				return;

			copyAccessors();
		}

		//Accessor<nmiditicks_T,MidiEvent> * getFirstAccessor(Clip **key=0) const;

		void updateCurrent();
		MidiEvent *getwat();

		// MidiMapAccessor interface
	public:


	private:
		Track & track;
		//std::set<Accessor*>current_clips;

//		nmiditicks_T current;

		std::map<miditick_T,ClipMapElem>::iterator clipmap_it;


		struct CurrentClipsElem {
			nmiditicks_T	offset;
			Accessor<nmiditicks_T,MidiEvent> * accessor;
		};
		const CurrentClipsElem * getFirstAccessor(Clip **key=0) const;
		std::map<Clip *,CurrentClipsElem> current_clips;

		void copyAccessors();


		Accessor<nmiditicks_T,MidiEvent> * current_accessor;

		// Accessor interface
	public:

	};*/



	clipmap_T::iterator clipmap_iterator;

	MidiEvent * getNext();
	void init();


//	List <nmiditicks_T,MidiClip> midi_clips_list;



/*
	class MidiClipList : List <nmiditicks_T,MidiClip >{
	public:

	};
	*/
//	ClipList clip_list;

	MidiClipList * getMidiClipList(){
		return &clip_list;
	}



	const std::string &getName() const;
	void setName(const std::string &newName);

private:
	std::string name;
	MidiClipList clip_list;
};

} // namespace Dawe

#endif // TRACK_H
