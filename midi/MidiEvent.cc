/*
 * Copyright 2000-2022 the Rosegarden development team.
 * Copyright (C) 2023  7u83 <7u83@mail.ru>
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

#include <cstdio>

#include "Midi.h"
#include "MidiEvent.h"
#include "MidiFile.h"

namespace Dawe {


const char *midi_note_names[12]={"C","C#","D","D#","E","F",
				 "F#","G","G#","A","A#","B"};

static struct
{
	uint8_t id;
	const char *name;
} midi_event_names[] = {

{ MidiEvent::NOTE_OFF, "Note Off" },
{ MidiEvent::NOTE_ON, "Note On" },
{ MidiEvent::POLY_AFTERTOUCH, "Polyphonic Aftertouch" },
{ MidiEvent::CTRL_CHANGE, "Controll Change"} ,
{ MidiEvent::PROG_CHANGE, "Program Change"} ,
{ MidiEvent::PROG_CHANGE, "Channel Aftertouch" },
{ MidiEvent::PITCH_BEND, "Pitch Bend" },
{ MidiEvent::SYSEX, "SysEx" },
{ MidiEvent::META, "Meta"},
{ 0x00, 0}

};

static struct
{
	uint8_t id;
	const char *name;
} midi_meta_event_names[] = {

{ MidiEvent::SEQUENCE, "Sequence" },
{ MidiEvent::TEXT, "Text" },
{ MidiEvent::COPYRIGHT, "Copyright" },
{ MidiEvent::TRACKNAME, "Track Name"} ,
{ MidiEvent::INSTRUMENT, "Instrument"} ,
{ MidiEvent::LYRIC, "Lyric" },
{ MidiEvent::MARKER, "Marker" },
{ MidiEvent::CUE, "Cue" },
{ MidiEvent::CHANNEL, "Channel"},
{ MidiEvent::EOT, "EOT"},
{ MidiEvent::PORT, "Port"},

{ MidiEvent::TEMPO, "Tempo"},
{ MidiEvent::TIMESIG, "Time Signature"},
{ MidiEvent::KEYSIG, "Key Signature"},
{ MidiEvent::CHANNEL, "Key Signature"},
{ 0x00, 0}

};

const char *MidiEvent::getName() const
{
	uint8_t id = getType();
	if (id!=META){
		return getName(getType());

	return getName(getMetaType());

	/*	for (int i=0; midi_event_names[i].name;i++){
			if (midi_event_names[i].id==id)
				return midi_event_names[i].name;
		}*/
		return 0;
	}
	id = getMetaType();
	for (int i=0; midi_meta_event_names[i].name;i++){
		if (midi_meta_event_names[i].id==id)
			return midi_meta_event_names[i].name;
	}
	return 0;

}

const char *MidiEvent::getName(Type id){
	for (int i=0; midi_event_names[i].name;i++){
		if (midi_event_names[i].id==id)
			return midi_event_names[i].name;
	}
	return 0;
}

const char *MidiEvent::getName(MetaType id)
{
	for (int i=0; midi_meta_event_names[i].name;i++){
		if (midi_meta_event_names[i].id==id)
			return midi_meta_event_names[i].name;
	}
	return 0;
}

void MidiEvent::dump() const{
	for (size_t i=0; i<midi_data.size(); i++){
		printf("%02x ",midi_data[i]);
	}
	printf("\n");
}

std::string toprint(std::string str)
{
	std::stringstream ss;
	bool isPrintableAscii = true;
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		char c = *it;
		if (c < 32 || c > 126) {
			isPrintableAscii = false;
			break;
		}
	}

	if (isPrintableAscii) {
		return str;
	} else {
		const char * separator = "";
		for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
			unsigned char c = static_cast<unsigned char>(*it);
			char buf[3];
			sprintf(buf, "%02X", c);
			ss << separator << buf ;
			separator = ",";
		}
	}
	return ss.str();
}







long MidiEvent::fromStream(std::ifstream *file, uint8_t &running_status, nmiditicks_T &time){

	long pos = file->tellg();

	uint32_t delta_time = MidiFile::readVarLengthQuantity(file);
	time+=delta_time;
	setTime(time);

	int statusbyte = MidiFile::getByte(file);

	if (!(statusbyte&0x80)){
		file->unget();
		statusbyte=running_status;
	}


	midi_data.push_back(statusbyte);

	if (statusbyte==META){
		midi_data.push_back(MidiFile::getByte(file));
		uint32_t len = MidiFile::readVarLengthQuantity(file);
		for (uint32_t i=0; i< len; i++){
			midi_data.push_back(MidiFile::getByte(file));
		}
		return file->tellg()-pos;
	}

	running_status=statusbyte;

	Type t = (Type)(statusbyte & 0xf0);

	// set length first
	switch(t){
	case NOTE_ON:
	case NOTE_OFF:
	case PITCH_BEND:
	case CTRL_CHANGE:
	case POLY_AFTERTOUCH:
		midi_data.push_back(MidiFile::getByte(file));
		midi_data.push_back(MidiFile::getByte(file));
		break;
	case PROG_CHANGE:
	case CHNL_AFTERTOUCH:
		midi_data.push_back(MidiFile::getByte(file));
		break;
	case SYSEX:{
		uint32_t len = MidiFile::readVarLengthQuantity(file);
		for(uint32_t i=0; i<len; i++){
			midi_data.push_back(MidiFile::getByte(file));
		}
		break;
	}
	default:
		//			midi_data_len=3;
		break;
	}
	return file->tellg()-pos;
}







/*
QDebug operator<<(QDebug dbg, const MidiEvent &midiEvent)
{
    timeT tempo;
    int tonality;
    std::string sharpflat;

    if (midiEvent.m_metaEventCode) {
	switch (midiEvent.m_metaEventCode) {
	case MIDI_SEQUENCE_NUMBER:
	    dbg << "MIDI SEQUENCE NUMBER";
	    break;

	case MIDI_TEXT_EVENT:
	    dbg << "MIDI TEXT:\t\"" << midiEvent.m_metaMessage << '"';
	    break;

	case MIDI_COPYRIGHT_NOTICE:
	    dbg << "COPYRIGHT:\t\"" << midiEvent.m_metaMessage << '"';
	    break;

	case MIDI_TRACK_NAME:
	    dbg << "TRACK NAME:\t\"" << midiEvent.m_metaMessage << '"';
	    break;

	case MIDI_INSTRUMENT_NAME:
	    dbg << "INSTRUMENT NAME:\t\"" << midiEvent.m_metaMessage << '"';
	    break;

	case MIDI_LYRIC:
	    dbg << "LYRIC:\t\"" << midiEvent.m_metaMessage << '"';
	    break;

	case MIDI_TEXT_MARKER:
	    dbg << "MARKER:\t\"" << midiEvent.m_metaMessage << '"';
	    break;

	case MIDI_CUE_POINT:
	    dbg << "CUE POINT:\t\"" << midiEvent.m_metaMessage << '"';
	    break;

	    // Sets a Channel number for a TRACK before it starts
	case MIDI_CHANNEL_PREFIX:
	    dbg << "CHANNEL PREFIX:\t"
		<< (timeT)midiEvent.m_metaMessage[0];
	    break;

	    // These are actually the same case but this is not an
	    // official META event - it just crops up a lot.  We
	    // assume it's a MIDI_CHANNEL_PREFIX though
	    //
	case MIDI_CHANNEL_PREFIX_OR_PORT:
	    dbg << "FIXED CHANNEL PREFIX:\t"
		<< (timeT)midiEvent.m_metaMessage[0];
	    break;

	case MIDI_END_OF_TRACK:
	    dbg << "END OF TRACK";
	    break;

	case MIDI_SET_TEMPO:
	    tempo =
		((timeT)(((MidiByte)midiEvent.m_metaMessage[0]) << 16)) +
		((timeT)(((MidiByte)midiEvent.m_metaMessage[1]) << 8)) +
		(short)(MidiByte)midiEvent.m_metaMessage[2];

	    tempo = 60000000 / tempo;
	    dbg << "SET TEMPO:\t" << tempo;
	    break;

	case MIDI_SMPTE_OFFSET:
	    dbg << "SMPTE TIME CODE:\t"
		<< (timeT)midiEvent.m_metaMessage[0]
		<< ":" << (timeT)midiEvent.m_metaMessage[1]
		<< ":" << (timeT)midiEvent.m_metaMessage[2]
		<< "  -  fps = " << (timeT)midiEvent.m_metaMessage[3]
		<< "  - subdivsperframe = "
		<< (timeT)midiEvent.m_metaMessage[4];
	    break;

	case MIDI_TIME_SIGNATURE:
	    dbg << "TIME SIGNATURE:\t"
		<< (timeT)midiEvent.m_metaMessage[0]
		<< "/"
		<< (1 << (timeT)midiEvent.m_metaMessage[1]);
	    break;

	case MIDI_KEY_SIGNATURE:
	    tonality = (int)midiEvent.m_metaMessage[0];

	    if (tonality < 0) {
		sharpflat = std::to_string(-tonality) + " flat";
	    } else {
		sharpflat = std::to_string(tonality);
		sharpflat += " sharp";
	    }

	    dbg << "KEY SIGNATURE:\t" << sharpflat << " "
		<< (((int)midiEvent.m_metaMessage[1]) == 0 ? "major" : "minor");

	    break;

	case MIDI_SEQUENCER_SPECIFIC:
	    dbg << "SEQUENCER SPECIFIC:\t\"" << midiEvent.m_metaMessage;
	    break;


	default:
	    dbg << "Undefined MIDI META event - "
		<< (timeT)midiEvent.m_metaEventCode;
	    break;
	}
    } else {
	switch (midiEvent.m_eventCode & MIDI_MESSAGE_TYPE_MASK) {
	case MIDI_NOTE_ON:
	    dbg << "NOTE ON:\t" << (int)midiEvent.m_data1 << " - "
		<< (int)midiEvent.m_data2;
	    break;

	case MIDI_NOTE_OFF:
	    dbg << "NOTE OFF:\t" << (int)midiEvent.m_data1 << " - "
		<< (int)midiEvent.m_data2;
	    break;

	case MIDI_POLY_AFTERTOUCH:
	    dbg << "POLY AFTERTOUCH:\t" << (int)midiEvent.m_data1
		<< " - " << (int)midiEvent.m_data2;
	    break;

	case MIDI_CTRL_CHANGE:
	    dbg << "CTRL CHANGE:\t" << (int)midiEvent.m_data1
		<< " - " << (int)midiEvent.m_data2;
	    break;

	case MIDI_PITCH_BEND:
	    dbg << "PITCH BEND:\t" << (int)midiEvent.m_data1
		<< " - " << (int)midiEvent.m_data2;
	    break;

	case MIDI_PROG_CHANGE:
	    dbg << "PROG CHANGE:\t" << (int)midiEvent.m_data1;
	    break;

	case MIDI_CHNL_AFTERTOUCH:
	    dbg << "CHNL AFTERTOUCH\t" << (int)midiEvent.m_data1;
	    break;

	default:
	    dbg << "Undefined MIDI event";
	    break;
	}
    }

    return dbg;
}
*/

}  // namesapce
