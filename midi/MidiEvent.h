/*
 * Copyright 2000-2022 the Rosegarden development team.
 * Copyright (C) 2005-2013, rncbc aka Rui Nuno Capela. All rights reserved.
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
#ifndef MIDIEVENT_H
#define MIDIEVENT_H

/**
 * @file
 *
 * Definition of MidiEvent class
 */


#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <cstdio>


#include "../dawe.h"
#include "../VObj.h"

namespace Dawe
{


/**
 * @defgroup MIDI MIDI Classes and functions
 *
 * @addtogroup MIDI
 * @{
 */


extern const char *midi_note_names[12];

extern std::string toprint(std::string str);


///  For standard MIDI file I/O.
/*
 * MidiEvent is a representation of MIDI which we use
 * for the import/export of MIDI files.  It uses std::string for
 * meta event messages which makes them nice and easy to handle.
 *
 * We don't use this class at all for playback or recording of MIDI -
 * for that look at MappedEvent and MappedEventList.
 *
 * Rosegarden doesn't have any internal concept of MIDI events, only
 * the Event class which offers a superset of MIDI functionality.
 */


/**
 * @brief The MidiEvent class
 */
class MidiEvent : public VObj
{

public:

	/**
	 * @brief Standard MIDI events types
	 *
	 * These are the MIDI event types as defined in the
	 * MIDI 1.0 sepecification v.96.1 3rd Ed.
	 * Event type is also often reffered as "message type".
	 */
	enum Type{
		NOTE_OFF	= 0x80,	/**< The Note Off message is sent, when
					     a key on a MIDI keyboad is released
					     which means, the note is turned
					     off. */
		NOTE_ON		= 0x90, /**< The Note On message is sent, when
					     a key is pressed on a MIDI keyboard
					     instrument or MIDI keyboard
					     controller. */
		POLY_AFTERTOUCH	= 0xa0, /**< Some MIDI keyboard instruments have
					     the ability to sense the amount of
					     pressure which is being applied to
					     the keys while they are depressed.
					     This pressure information, commonly
					     called "aftertouch", may be used to
					     control some aspects of the sound
					     produced by the synthesizer
					     (vibrato, for example).
					     The Polyphonic AfterTouch event
					     indicates an aftertouch event for a
					     specific note in contrast to the
					     #CHNL_AFTERTOUCH event which applies
					     to all notes currently on. */
		CTRL_CHANGE	= 0xb0,	/**< MIDI Control Change messages are
					     used to control a wide variety of
					     functions in a synthesizer. */
		PROG_CHANGE	= 0xc0, /**< The Program Change message is used
					     to specify the type of instrument
					     which should be used to
					     play sounds on a given Channel. */
		CHNL_AFTERTOUCH	= 0xd0, /**< Aftertoch for all notes currently
					     playing on the addressed channel.
					     @see #POLY_AFTERTOUCH */
		PITCH_BEND	= 0xe0,	/**< The Pitch Bend Change message is
					     normally sent from a keyboard
					     instrument in response to changes
					     in position of the pitch bend
					     wheel. */
		SYSEX		= 0xf0, /**< System Exclusive messages include
					     a Manufacturer's Identification (ID)
					     code, and are used to transfer any
					     number of data bytes in a format
					     specified by the referenced
					     manufacturer. */

		//RESET		= 0xff,
		META		= 0xff, /**< Meta events are never sent to a MIDI
					     device. Meta events are used to
					     describe tempo information, lyrics
					     or time signatures. There are
					     several meta event types defined,
					     known by LIBDAWE.
					     ^@see MetaType */
		// Extra-ordinary event types...
		REGPARAM	= 0x10,
		NONREGPARAM	= 0x20,
		CONTROL14	= 0x30
	};


	/**
	 * @brief Meta event types
	 *
	 * These are some MIDI meta event types, known by LIBDAWE.
	 */
	enum MetaType {
		SEQUENCE	= 0x00,	/**< Sequence Text. */
		TEXT		= 0x01,	/**< Contains text that the user or
					     the program must interpret. */
		COPYRIGHT	= 0x02,	/**< Text used to hold a copyright notice
					     typically found in a MIDI file.
					     @see MidiFile*/
		TRACKNAME	= 0x03, /**< Text to name a track in a MIDI file
					     @see MidiFile */
		INSTRUMENT	= 0x04, /**< Text to name the instrument */
		LYRIC		= 0x05,
		MARKER		= 0x06,
		CUE		= 0x07,
		CHANNEL		= 0x20,
		PORT		= 0x21,
		EOT		= 0x2f,
		TEMPO		= 0x51,
		SMPTE		= 0x54,
		TIMESIG		= 0x58,
		KEYSIG		= 0x59,
		PROPRIETARY	= 0x7f
	};

	///
	/// \brief Create a MIDI event.
	///
	/// All data in this event are uninitializied, so the object is
	/// useless until it gets initialized.
	///
	MidiEvent(){}


	///
	/// \brief Create a MIDI of a specfific type.
	///
	/// The created event will be initialized with some useful default
	/// values.
	///
	/// \param type Type of the event.
	/// \param time The time for the created event, defaults to 0.
	///
	MidiEvent(Type type, nmiditicks_T time=0):
		m_time(time){


		init(type);
		return;

		midi_data.push_back(type);

		// set some defaults if nececerry
		switch(type){
		case NOTE_ON:
		case NOTE_OFF:
		case POLY_AFTERTOUCH:
			midi_data.resize(3);
			setNote(0+5*12);
			setVelocity(100);
			break;

		default:
			//		memset(midi_data+1,0,midi_data_len-1);
			break;
		}
	}

	///
	/// \brief Create a meta MIDI meta event of specfici type
	///
	/// The event data will be initialized with some useful
	/// default values. For example `MidiEvent *e = new MidiEvent(NOTE_ON);`
	/// @code
	///   MidiEvent *e = new MidiEvent(NOTE_ON);
	/// @endcode
	/// creates a new MIDI event at time 0 with note C3.
	///
	/// \param type Meta type of the event.
	/// \param time Time for this event, defaults to 0
	///
	MidiEvent(MetaType type, nmiditicks_T time=0):
		m_time(time)
	{
		init(type);
		return;

		midi_data.push_back(META);
		midi_data.push_back(type);
		// set len first
		switch (type){
		case TEMPO:
			midi_data.resize(5);
			setTempo(MIDI_DEFAULT_TEMPO);
			break;
		case TIMESIG:
			midi_data.resize(6);
			setTimeSig(4,4);
			break;
		default:
			midi_data.resize(8,0);
			//		memset(midi_data+3,0,midi_data_len-3);
			break;
		}

	}

	/**
	 * @brief The destructor
	 *
	 * Does nothing
	 */
	~MidiEvent(){}


	void setType(Type type){
		midi_data.clear();
		init(type);
	}

	void init(Type type){
		midi_data.push_back(type);

		// set some defaults if nececerry
		switch(type){
		case NOTE_ON:
		case NOTE_OFF:
		case POLY_AFTERTOUCH:
			midi_data.resize(3);
			setNote(0+5*12);
			setVelocity(100);
			break;

		default:
			//		memset(midi_data+1,0,midi_data_len-1);
			break;
		}
	}

	void setType(MetaType type) {
		midi_data.clear();
		init(type);
	}

	inline void init(MetaType type){
		midi_data.push_back(META);
		midi_data.push_back(type);
		// set len first
		switch (type){
		case TEMPO:
			midi_data.resize(5);
			setTempo(MIDI_DEFAULT_TEMPO);
			break;
		case TIMESIG:
			midi_data.resize(6);
			setTimeSig(4,4);
			break;
		default:
			midi_data.resize(8,0);
			//		memset(midi_data+3,0,midi_data_len-3);
			break;
		}
	}

	/**
	 * @brief fromStream
	 * @param file
	 * @param running_status
	 * @param time
	 * @return
	 */
	long fromStream(std::ifstream *file, uint8_t & running_status, nmiditicks_T & time);


	inline uint8_t getNumerator() const {return midi_data[2]; }
	inline void setNumerator(uint8_t n) {midi_data[2]=n; }

	inline uint8_t getDenomintaor() const {
		assert(getMetaType()==TIMESIG);
		return 1 << midi_data[3];
	}
	inline void setDenominator(uint8_t d){
		assert(getMetaType()==TIMESIG);
		uint8_t dl=0;
		while(d>>=1)dl++;
		midi_data[3]=dl;
	}
	void setTimeSig(uint8_t n, uint8_t d){
		setNumerator(n);
		setDenominator(d);
	}


	/**
	 * @brief Get text from a meta message that contains text.
	 *
	 * A lot of MIDI meta messages contain a text string. For
	 * those messages the text can be retrieved with this method.
	 * If the message does not contain a text string the behavior is
	 * undefined. To be sure there is a text string available check
	 * with #hasText.
	 *
	 * @see hasText
	 *
	 * @return A text string.
	 */
	std::string getText() const {
		assert(hasText());
		std::string s;
		for (size_t i=2; i<midi_data.size(); i++){
			s+=midi_data[i];
		}
		return s;
	}

	void setText(const char *text,int len){
		midi_data.resize(len+2);
		for(int i=0; i<len; i++){
			midi_data[i+2]=text[i];
		}
	}

	void setText(const std::string & s) {
		assert(hasText());
		setText(s.c_str(),s.size());
	}
	void setText(const char *s){
		assert(hasText());
		setText(s,strlen(s));
	}

	/**
	 * @brief Check whether this event has text.
	 *
	 * If this message has text available you cann call
	 * #getText to obtain the text.
	 *
	 * @see #getText
	 * @return True if text is available.
	 */
	bool hasText() const {
		if (!isMeta())
			return false;
		switch (getMetaType()){
		case COPYRIGHT:
		case MARKER:
		case TRACKNAME:
		case LYRIC:
		case TEXT:
		case PORT:
			return true;
		default:
			return false;
		}
	}


	/**
	 * @brief Set Tempo on a tempo meta message
	 *
	 * If the message is not of #MetaType #TEMPO the behavior is
	 * undefined and the program may crash.
	 *
	 * @param tempo
	 */
	void setTempo(miditempo_T tempo){
		midi_data[0+2]=(tempo >> 16)  & 0xff;
		midi_data[1+2]=(tempo >> 8)  & 0xff;
		midi_data[2+2]=tempo & 0xff;
	}

	/**
	 * @brief Get the tempo value
	 *
	 * If the event is not of #MetaType #TEMPO the returned value
	 * is undefined.
	 *
	 * @return Teh tempo
	 */
	miditempo_T getTempo() const{
		return	(midi_data[0+2] << 16)+
				(midi_data[1+2] << 8) +
				(midi_data[2+2]);
	}


	int controller() const{ return getData1();}
	int param() const {return getData1();}
	int value() const {return getData1();}

	//	const unsigned char * sysex() const {return "sx";}
	int sysex_len()const{return 0;}



	void setTime(nmiditicks_T time)  { m_time = time; }
	nmiditicks_T getTime() const  { return m_time; }

	///
	/// \brief get the frame this event is positioned
	///
	/// A frame is only available for tempo events. And only the, if
	/// the frame was calculated end set before by an appropriate function.
	/// So the frame is set, when the event was stored in a #MidiTempoIdx.
	/// In all other cases the return value of this function is undefined.
	///
	/// \return the  frame
	///
	/*	nframes_T getFrame() const {
		return xmidi_data.tempo.frames;
	}
	void setFrame(nframes_T frames){xmidi_data.tempo.frames=frames;}
*/

	// nframes_T m_frames;

	void setDuration(const nmiditicks_T& duration) { m_duration = duration; }
	nmiditicks_T getDuration() const { return m_duration; }

	//	MidiByte getEventCode() const { return m_eventCode; }
	//	MidiByte getMessageType() const
	//	{ return m_eventCode & MIDI_MESSAGE_TYPE_MASK; }
	//	MidiByte getChannelNumber() const
	//	{ return m_eventCode & MIDI_CHANNEL_NUM_MASK; }

	/**
	 * @brief Get the message type.
	 *
	 * @return Type of message (event).
	 */
	inline Type getType() const{
		if (midi_data[0]==META)
			return META;
		// return status byte zeroing channel info
		return (Type)(midi_data[0] & 0xf0);
	}

	/**
	 * @brief getCombinedType
	 * @return The combined type
	 */
	uint16_t getCombinedType() const {
		if(isMeta()){
			return ((uint16_t)(META<<8)) + getMetaType();
		}
		return getType();
	}

	static inline uint16_t getCombinedType(Type t){
		return t;
	}
	static inline uint16_t getCombinedType(MetaType t){
		return ((uint16_t)(META<<8)) + t;
	}

	/**
	 * @brief getMetaType
	 * @return the meta type
	 * @see #MetaType
	 */
	inline MetaType getMetaType() const { return (MetaType)midi_data[1]; }

	///
	/// \brief Get MIDI channel
	///
	/// This functions works only for events, which provide
	/// a channel, like NOTE_ON, NOTE_OFF, POLY_AFTERTOUCH, ...
	/// For other messages the return value is undefined.
	///
	/// \return The channel number, a value between 0 and 15.
	///
	inline uint8_t getChannel() const {
		assert(hasChannel());
		return midi_data[0] & 0xf;
	}


	/**
	 * @brief Set MIDI channel
	 *
	 * Set the MIDI channel for this event. If the event doesn't have
	 * a channel this method does nothing.
	 *
	 * @param ch
	 *	The channel number to be set, a number between 0 and 15.
	 *	Values above 15 will be cut.
	 */
	void setChannel(uint8_t ch) {
		assert(hasChannel());
		if (!hasChannel())
			return;
		midi_data[0]=(midi_data[0]&0xf0) | (ch &0xf);
	}

	/**
	 * @brief Check if the event has a channel.
	 * @return True if a channel is preset. Otherwise false.
	 */
	bool hasChannel() const {
		switch(getType()){
		case NOTE_ON:
		case NOTE_OFF:
		case PROG_CHANGE:
		case CTRL_CHANGE:
		case POLY_AFTERTOUCH:
		case CHNL_AFTERTOUCH:
			return true;
		default:
			return false;
		}
	}

	inline uint8_t getStatusByte() const { return midi_data[0]; }

	uint8_t getPitch() const { return getData1(); }

	uint8_t getData1() const { return midi_data[1]; }
	uint8_t getData2() const { return midi_data[2]; }


	inline uint8_t getVelocity() const {
		assert(hasVelocity());
		return midi_data[2];
	}
	inline void setVelocity(uint8_t v){
		assert(hasVelocity());
		midi_data[2]=v;
	}

	/**
	 * @brief hasVelocity
	 * @return
	 */
	inline bool hasVelocity() const {
		switch(getType()){
		case NOTE_OFF:
		case NOTE_ON:
		case POLY_AFTERTOUCH:
			return true;
		default:
			return false;
		}
	}

	/**
	 * @brief compare operator <
	 * @param other
	 * @return
	 */
	bool operator<(const MidiEvent& other) const {
		return getTime() < other.getTime();
	}

	/**
	 * @brief operator <
	 * @param t
	 * @return
	 */
	bool operator<(const nmiditicks_T t ) const {
		return getTime() < t;
	}

	operator nmiditicks_T(){
		return getTime();
	}


	inline uint8_t getNote() const{
		assert(hasNote());
		return midi_data[1];
	}
	inline void setNote(uint8_t n){
		assert(hasNote());
		midi_data[1]=n;
	}

	inline bool hasNote() const {
		switch (getType()){
		case NOTE_OFF:
		case NOTE_ON:
		case POLY_AFTERTOUCH:
			return true;
		default:
			return false;
		}
	}

	std::string getValueStr(){
		uint16_t t = getCombinedType();
		switch(t){
		case META<<8 | TRACKNAME:
		case META<<8 | LYRIC:
			return getText();
		}

		return std::string();
	}

	std::string getChannelStr(){
		if (!hasChannel())
			return std::string();
		char str[6];
		sprintf(str,"%d",getChannel());
		return std::string(str);
	}

	/**
	 * @brief get a human readable name for this event.
	 * @return A pointer to the name
	 */
	const char * getName() const;

	static const char * getName(Type id);
	static const char * getName(MetaType id);



	/**
	 * @brief Check wether this event is a meta event.
	 * @return true if event is a meta event, otherwise false
	 */
	inline bool isMeta() const {
		return (midi_data[0] == META);
	}

	//	MidiByte getMetaEventCode() const { return m_metaEventCode; }
	//	void setMetaMessage(const std::string &meta) { m_metaMessage = meta; }
	//	std::string getMetaMessage() const { return m_metaMessage; }

	//  friend QDebug operator<<(QDebug, const MidiEvent &);

	//	int getId(){ return m_id; }

	/*
	inline std::string get(uint8_t &prev ) {
		std::string buf;
		if (isMeta()) {
			buf += META;
			buf += getMetaEventCode();

			buf += longToVarBuffer(getMetaMessage().length());
			buf += getMetaMessage();

			// Meta events cannot use running status.
			prev = 0;

		} else {  // non-meta event
			// If the event code has changed, or this is a SYSEX event, we
			// can't use running status.
			// Running status is "[f]or Voice and Mode messages only."
			// Sysex is a system message.  See the MIDI spec, Section 2,
			// page 5.
			if ((getEventCode() != prev) ||	(getEventCode() == MIDI_SYSTEM_EXCLUSIVE)) {

				// Send the normal event code (with encoded channel information)
				buf += getEventCode();
				prev = getEventCode();
			}

			switch (getMessageType()) {
			case MIDI_NOTE_ON:  // These have two data bytes.
			case NOTE_OFF:
			case MIDI_PITCH_BEND:
			case MIDI_CTRL_CHANGE:
			case MIDI_POLY_AFTERTOUCH:
				buf += getData1();
				buf += getData2();
				break;

			case MIDI_PROG_CHANGE:  // These have one data byte.
			case MIDI_CHNL_AFTERTOUCH:
				buf += getData1();
				break;

			case MIDI_SYSTEM_EXCLUSIVE:
				buf += longToVarBuffer(getMetaMessage().length());
				buf += getMetaMessage();
				break;

			default:
				std::cout << "XUnsupported element" << std::endl;
				/  *		RG_WARNING << "writeTrack() - cannot write unsupported MIDI event: "
					   << std::hex << midiEvent.getMessageType();
		* /		break;
			}
		}
		return buf;
	} */

	void dump() const;

	inline nmiditicks_T getKey() { return getTime();}

	/*	inline const char * get_name() const{
		return midi_event_names[(getEventCode()>>4)&0x07];
	}
*/	/*		std::string n;
		std::stringstream ss;
		int octave;
		std::string note;
		if (this->isMeta())
			return n;
		switch (getMessageType()){
		case MIDI_NOTE_ON:
			get_noten_name(this->getData1(),octave, note);
			ss << "Note on :" << note << octave<< " , " <<(unsigned int)getData2();
			return ss.str();
		case MIDI_NOTE_OFF:
			get_noten_name(this->getData1(),octave, note);
			ss << "Note off:" << note << octave << " , " << (unsigned int)getData2();
			return ss.str();
		default:
			return "unknown";
		}
	}
*/

	/*	friend std::ostream& operator<<(std::ostream& os, const MidiEvent& ev)  {
		//return os;

		os << "[" << std::dec << ev.m_time << "]";

		if (ev.isMeta()) {
			uint8_t code = ev.getMetaEventCode();
			std::string msg = toprint (ev.getMetaMessage());

			return os << "Meta: " << std::hex << (unsigned int)code
				  << msg << std::dec;
		}

		os << "Event(" << (unsigned int)ev.getChannelNumber() <<"):"
		   << std::hex << (unsigned int)ev.getEventCode();

		if (ev.getMessageType()==NOTE_OFF || ev.getMessageType()==MIDI_NOTE_ON){
			os << " - " << "ev.get_noten_name()" << " " << (unsigned int)ev.getVelocity()
			   << " - " << ev.getName();
			return os;
		}

		return os << " - " << ev.getName();
	}
*/

	/*	bool operator < (const MidiEvent & other) const  {

		//std::cout << "The compare operator is here !!!" << std::endl;

		if (m_time < other.m_time)
			return true;



		if (m_time > other.m_time)
			return false;
		if (m_id<other.m_id)
			return true;
		return false;
	}
*/
	// Special accessors for pitch-bend event types.
	//	int pitchBend() const
	//	{ return int(pitch_value) - 0x2000; }
	//	void setPitchBend(int iPitchBend)
	//	{ pitch_value = (unsigned short) (0x2000 + iPitchBend); }

private:
	nmiditicks_T m_time;
	nmiditicks_T m_duration;
	std::vector<uint8_t> midi_data;

};

/**
 *@}
 */

}





#endif // MIDIEVENT_H
