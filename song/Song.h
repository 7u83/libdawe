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
#ifndef SONG_H
#define SONG_H

#include <map>

#include <iostream>

#include "../Session.h"


#include "Track.h"

#include "MidiClipLink.h"
#include "MidiClip.h"


#include <vector>

namespace Dawe {


class MidiClipLink;

class Song
{
public:


	///
	/// \brief getTicksPerMeasure
	/// \param t
	/// \return
	///
	/*	nmiditicks_T getTicksPerMeasure(TimeSigEvent & t){
		return t.getNumerator()*4*session->getTicksPerBeat()
				/t.getDenomintaor();
	}
*/



	/**
	 * @brief Constructor for Song object
	 * @param session the session this song belongs to
	 */
	Song(Session * session);


	~Song();

	typedef std::map<nmiditicks_T,miditempo_T> tempomap_T;

	/**
	 * @brief Get the default tempo
	 *
	 * The default tempo for midi is 500000, which corresponds to 120 BPM.
	 * @return default tempo
	 */
	static inline miditempo_T getDefaultTempo(){ return 500000; }

	inline miditempo_T getTempoAt(nmiditicks_T pos){
		tempomap_T::iterator i = tempo_map.lower_bound(pos);
		if (i==tempo_map.begin())
			return getDefaultTempo();
		--i;
		return i->second;
	}

	tempomap_T::iterator getTempoMapIterator(nmiditicks_T start=0){
		return tempo_map.lower_bound(start);
	}

	/**
	 * @brief Convert midi tempo to BPM (beats per minute)
	 * @param t midi tempo to convert
	 * @return BPM
	 */
	static float MidiTempoToBpm(miditempo_T t);

	//typedef EventMap<TimeSigEvent> SigMap;
	//SigMap sig_map;
	//	EventMap<Tempo> tm;

	Session * getSession() const{
		return session;
	}
	/*
	Clip * createClip(std::string & type){
		Clip *c=0;
		if (type=="MIDI"){
			c = new MidiClip("MidiClip",0);
		}
		return c;
	}
*/

	MidiClip * createMidiClip();

	void initSigMap();


	Track & getTimeSigTrack(){
		return timesig_track;
	}

	//typedef std::vector<MidiClip *> midicliplist_T;

	const std::vector<MidiClip *> & getMidiClipList() const{
		return midiclip_list;
	}

	MidiClip* insertMidiClipAtIndex(int index, const char * name = "New Clip");
	void removeMidiClipAtIndex(int index);

	/**
	 * @brief getTrackCount
	 * @return
	 */
	int getTrackCount(){
		return track_list.size();
	}

	Track * insertTrackAtIndex(int index, const char *name);
	Track * getTrackAtIndex(int index){
		return track_list[index];
	}
	void deleteTrackAtIndex(int index);

private:
	std::vector<MidiClip *> midiclip_list;
	std::vector<Track *> track_list;
	Track timesig_track;




	Track tempo_track;


	std::vector<Clip *>all_clips;
	std::map<std::string,Clip *> clips;


	// the session this song belongs to
	Session * session;

	tempomap_T tempo_map;
	//	std::map <nmiditicks_T,TimeSigEvent> time_sig_map;


};

} // namespace Dawe

#endif // SONG_H
