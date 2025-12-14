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
#include "Song.h"

#include "MidiClip.h"

namespace Dawe {



Song::Song(Session * session) :
	session(session)
{

}

Song::~Song(){
	std::vector<MidiClip*>::iterator it;
	for(it=midiclip_list.begin(); it!=midiclip_list.end(); ++it){
		delete *it;
	}

	std::vector<Track*>::iterator itt;
	for(itt=track_list.begin(); itt!=track_list.end(); ++itt){
		delete *itt;
	}
}


float Song::MidiTempoToBpm(miditempo_T t){
	return 60000000.0 / float(t);
}

MidiClip* Song::createMidiClip(){
	char name[32];
	sprintf(name,"Clip %ld",midiclip_list.size()+1);
	MidiClip *clip = new MidiClip(name);
	midiclip_list.push_back(clip);
	return clip;
}

void Song::initSigMap(){
	//	sigmap = new MidiClip(this);

}

void Dawe::Song::removeMidiClipAtIndex(int index)
{
	MidiClip *clip=midiclip_list[index];
	std::vector<MidiClip *>::iterator it = midiclip_list.begin() + index;
	midiclip_list.erase(it);
	delete clip;
}

Track *Song::insertTrackAtIndex(int index, const char *name){
	Track * track = new Track();
	track->setName(name);
	std::vector<Track *>::iterator it = track_list.begin()+index;
	track_list.insert(it,track);
	return track;
}

void Song::deleteTrackAtIndex(int index){
	std::vector<Track *>::iterator it = track_list.begin()+index;
	delete track_list[index];
	//delete *it;
	track_list.erase(it);

}

MidiClip *Song::insertMidiClipAtIndex(int index, const char *name){
	MidiClip *clip = new MidiClip(name);
	std::vector<MidiClip *>::iterator it = midiclip_list.begin() + index;
	midiclip_list.insert(it, clip);
	clip->setName(name);
	return clip;
}



} // namespace Dawe
