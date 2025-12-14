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

#include <stdlib.h>
#include <string.h>
#include <set>

#include <jack/midiport.h>

#include "midi/Midi.h"
#include "MidiPlayer.h"

#include "midi/MidiFile.h"

namespace Dawe {

MidiPlayer::MidiPlayer(Session *sess, const char *name) : JackDevice(name),sess(sess)
{
	register_port("out", Port::MIDI);
}

void MidiPlayer::play()
{

	float tictime = 5000000/960;
	std::cout << "TICTICTCTIME: "<< tictime << " mySec" << std::endl;

	float frametime = 1.0/48000.0 * 1000000;
	std::cout << "FRAMTIME:" << frametime << " mySec" << std::endl;

	float fac = tictime/frametime;
	std::cout << "FAC:" << fac << std::endl;

	//f.convertToRosegarden("tr0.mid");
	//f.convertToRosegarden("long.mid");
	//	f.convertToRosegarden("thunderstruck.mid");
	//f.convertToRosegarden("ShineOnYouCrazyDiamond-1-5.mid");
	// f.convertToRosegarden("ShineOnYouCrazyDiamond.mid");
	//	f.convertToRosegarden("TheWall.mid");
	//f.convertToRosegarden("BohemianRhapsody-1.mid");
//	f.convertToRosegarden("BohemianRhapsody.mid");
	//f.convertToRosegarden("kraftwerk-modell.mid");
	//f.convertToRosegarden("HungUp.mid");
	// f.convertToRosegarden("More.mid");
	//f.convertToRosegarden("Rasputin.mid");
	//	f.convertToRosegarden("DaddyCool.mid");
	//	f.convertToRosegarden("KillingInTheNameOf.mid");
	//f.convertToRosegarden("WishYouWereHere.mid");

//	MidiFile::MidiTrack t = (f.m_midiComposition)[1];
//	theTrack=t;

/*
	tempo = 5000000; // default 120bpm
	it = f.allEvents.begin();
	frame_offset=0;
	tick_offset=0;

	std::set<MidiEvent>::iterator tit = f.allEvents.begin();

	while (tit != f.allEvents.end()){
		MidiEvent e = *tit;
		std::cout << "all: " << std::dec << e.getTime() << std::endl;
		++tit;
	}

*/
	activate();
}


int MidiPlayer::old_process_callback(uint32_t nframes)
{

/*
	int rc = get_transport();
	//std::cout << "RC: STOPP:" << rc << " " << Transport::STOPPED << std::endl;
	switch (rc){
	case Transport::STOPPED:
		return 0;
	}

	uint32_t cf = get_current_frame();

	void *port_buf = ports[0]->get_buffer(nframes);
	jack_midi_clear_buffer(port_buf);
	uint8_t prev=0;
	do {
//		if (it==f.allEvents.end())
//			break;

		MidiEvent e = *it;
		//std::cout << "clbk: " << e->getTime() << " id: " << e->getId() << std::endl;


		float tlen=tempo/f.m_time_division;
		float flen=1.0/48000.0 * 1000000;
		float fac = tlen/flen;

		// tlen = tempo/tpb flen = 1000000 / 48000  fac = (tempo/tpb) * 48000 / 1000000

		uint32_t tframe = frame_offset + ((uint32_t)(e.getTime()-tick_offset) * fac);
		if (tframe>=cf+nframes){
			/ *	std::cout << std::dec << "tf in ftr:" << tframe
				  << " (" << e.getTime() << ") "
				  << " cur:" << cf+nframes << std::endl;
		* b/	break;
		}
		if (tframe<cf){
			//	std::cout << "fst fwd:" << tframe << "->" << cf << std::endl;
			++it;
			continue;
		}

		if (e.isMeta()){
			if (e.getMetaEventCode() == MIDI_SET_TEMPO) {
				MidiByte m0 = e.getMetaMessage()[0];
				MidiByte m1 = e.getMetaMessage()[1];
				MidiByte m2 = e.getMetaMessage()[2];
				tempo = (((m0 << 8) + m1) << 8) + m2;
				if (tempo != 0) {
					double bpm = 60000000.0 / double(tempo);
					std::cout << "tmp chng: " << std::dec
						  << tempo << " bpm:" << bpm << std::endl;
					frame_offset=tframe;
					tick_offset=e.getTime();
				}
			}
			++it;
			continue;
		}

		std::string evw = e.get(prev);
		uint8_t * eb = jack_midi_event_reserve(port_buf, tframe-cf, evw.size());

		std::cout << "cblx" << e << std::endl;
		memcpy(eb,evw.c_str(),evw.size());

		++it;

	} while(1);

	return 0;*/

	//  float * b = ports[0]->get_buffer(nframes);
	//  jack_midi_data_t * mb = jack_midi_event_reserve(b,12,3);



//	static float tictime = 3200000/960;
	//float frametime = 1.0/48000.0 * 1000000;
//	float fac = tictime/frametime;

	(void)nframes;
	//std::cout << "Midi callback" << std::endl;

/*
	uint32_t last=0;
	for (unsigned int nn=0; nn<f.m_midiComposition.size(); nn++){

		theTrack = f.m_midiComposition[nn];

		for (unsigned int i=0; i<theTrack.size(); i++){





			MidiEvent *e;
			e = theTrack[i];








			uint32_t myf = (uint32_t)(e->getTime() * fac);

			//std::cout << "no?:" << myf << " vs " << cf << std::endl;
			if (myf >= cf && myf < cf+nframes){




				uint8_t prev=0;
				std::string evw = e->get(prev);
				std::stringstream ss;
				ss << *e;
				if (ss.str().size()){
					std::cout << "ELEMENT(" <<nn<<"):" << *e << std::endl ;
				}
				if (e->isMeta()){
					if (e->getMetaEventCode() == MIDI_SET_TEMPO) {
						MidiByte m0 = e->getMetaMessage()[0];
						MidiByte m1 = e->getMetaMessage()[1];
						MidiByte m2 = e->getMetaMessage()[2];
						tempo = (((m0 << 8) + m1) << 8) + m2;
						if (tempo != 0) {
							double bpm = 60000000.0 / double(tempo);

							std::cout << "TEMPO CHANGE: " << std::dec << tempo << "BPM:" << bpm << std::endl;;

							std::cout << "TIME DIVISION: " << f.m_timingDivision << std::endl;
							tictime=tempo/f.m_timingDivision;
						}
					}

					continue;
				}
				if (evw.size()>0){
					if (myf-cf<last)
						continue;
					last = myf-cf;

					uint8_t * eb = jack_midi_event_reserve(port_buf, myf-cf, evw.size());
					memcpy(eb,evw.c_str(),evw.size());
				};



			}

		}
	}*/
	//std::cout << "FRAME: " << cf << std::endl;
	return 0;
}

int MidiPlayer::process_callback(uint32_t nframes)
{
	(void)nframes;

	int rc = get_transport();
	switch (rc){
	case Transport::STOPPED:
		return 0;
	}


	nframes_T current_frame = get_current_frame();

	void *port_buf = ports[0]->get_buffer(nframes);
	jack_midi_clear_buffer(port_buf);
//	uint8_t prev=0;

	while(1) {
		if (queue->empty())
			break;

		MidiEvent *event = queue->getFirst();

		float tlen=tempo/sess->getTicksPerBeat(); //f.m_timingDivision;
		float flen=1.0/48000.0 * 1000000.0;
		float fac = tlen/flen;

		uint32_t tframe = frame_offset + ((uint32_t)(event->getTime()-tick_offset) * fac);

	//	printf("TFRAME: %d - etime: %d\n",tframe, event->getTime());

		if (tframe>=current_frame+nframes){
			break;
		}
		if (tframe<current_frame){
			//	std::cout << "fst fwd:" << tframe << "->" << cf << std::endl;
			queue->eraseFirst();
			continue;
		}

		nframes_T lat = ports[0]->get_latency();
		printf("Latency: %d\n",lat);

		if (event->isMeta()){
			if (event->getMetaType() == MIDI_SET_TEMPO) {
				tempo = event->getTempo();

/*				MidiByte m0 = event->getMetaMessage()[0];
				MidiByte m1 = event->getMetaMessage()[1];
				MidiByte m2 = event->getMetaMessage()[2];
				tempo = (((m0 << 8) + m1) << 8) + m2;
*/				if (tempo != 0) {
					double bpm = 60000000.0 / double(tempo);
					std::cout << "tmp chng: " << std::dec
						  << tempo << " bpm:" << bpm << std::endl;
					frame_offset=tframe;
					tick_offset=event->getTime();
				}
			}
			queue->eraseFirst();
			continue;
		}

//		std::string evw = event->get(prev);



//		uint8_t * eb = jack_midi_event_reserve(port_buf, tframe-current_frame, evw.size());

	//	std::cout << "cblx" << std::dec << "{" << tframe+current_frame << "}" << *event << std::endl;
	//	memcpy(eb,evw.c_str(),evw.size());

		queue->eraseFirst();

	}

	return 0;
}

}
