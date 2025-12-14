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
#ifndef MIDICLIPLINK_H
#define MIDICLIPLINK_H

#include <iostream>
#include <set>

#include "../algo/Accessor.h"
#include "../midi/MidiEvent.h"
#include "Clip.h"

namespace Dawe {

class MidiClip;

class MidiClipLink : public Clip
{
public:
	MidiClipLink(MidiClip * data):
		data(data){
		transpose=0;
	}

public:
	class accessor : public Accessor<nmiditicks_T,MidiEvent>{
	public:
		accessor(Accessor<nmiditicks_T,MidiEvent> * a,
			const MidiClipLink & clip):
			clip(clip){
			this->a = a;
		}

		MidiEvent *getValPtr() const{return a->getValPtr();}
		nmiditicks_T getKey() const{
			return a->getKey()+clip.getTime();
		}
		MidiEvent getVal() const{
			MidiEvent e = *a->getValPtr();
			e.setTime(e.getTime()+clip.getTime());
			return e;
		}


		void moveForward(){a->moveForward();}
		void moveBackward(){a->moveBackward();}
		bool isAtEnd() const{
			if (a->isAtEnd())
				return true;
			return a->getKey()>clip.getDuration();
		}
		bool isAtBegin() const { return a->isAtBegin();}


	private:
		const MidiClipLink & clip;
		Accessor<nmiditicks_T,MidiEvent> * a;
	};
public:
	accessor * createAccessor();

	MidiClip * getMidiData() const{
		return data;
	}

	MidiClipLink * createCopy(){
		MidiClipLink *c = new MidiClipLink(data);
		*c=*this;
		return c;
	}

private:
	MidiClip * data;
	uint8_t transpose;


};

} // namespace Dawe

#endif // MIDICLIP_H
