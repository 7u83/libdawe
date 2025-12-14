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
#ifndef CLIP_H
#define CLIP_H

#include <string>

#include "../dawe.h"



namespace Dawe {

class Song;

class Clip
{
public:

	enum Type {
		TYPE_NONE,
		TYPE_WAV,
		TYPE_WAV_LINK,
		TYPE_MIDI,
		TYPE_MIDI_LINK
	};




	Clip( std::string name = "Clip", nmiditicks_T duration=0):
		name(name),
		duration(duration){}

	virtual ~Clip(){

	}

	miditick_T getTime() const {return time;}
	void setTime(nmiditicks_T time){
		this->time=time;
	}

	virtual miditick_T getDuration() const {return duration; }
	void setDuration(miditick_T d) {duration=d;}
	const std::string & getName () const {return name;}
	void setName( const std::string & name) {this->name=name;}

	virtual Type getType(){
		return TYPE_NONE;
	}

	inline nmiditicks_T getKey() const{
		return time;
	}

	bool operator<(const Clip& other) const {
		return getTime() < other.getTime();
	}
protected:
//	Song * song;
	std::string name;
	nmiditicks_T duration;


	nmiditicks_T time; //!< starting time of this clip
};

} // namespace Dawe

#endif // CLIP_H
