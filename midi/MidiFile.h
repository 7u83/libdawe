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

#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <fstream>
#include <string>
#include <vector>
#include <map>
//#include <set>

#include "../dawe.h"

#include "Midi.h"
#include "MidiEvent.h"
#include "../Exception.h"
#include "MidiEventArrayList.h"

/**
 * @file
 * Implementation of MidiFile class
 * The MidiFile object is used to read and write MIDI files.
 */

namespace Dawe {

/**
 * @addtogroup MIDI
 * @{
 */


/**
 * @brief The MidiFile class
 */
class MidiFile
{
public:


	///
	/// \brief Read one byte out of a MIDI file
	///
	/// Throw an exception if no byte is availabel to read
	///
	/// \param file File to read from
	/// \return the byte read
	///
	static uint8_t getByte(std::ifstream *file){
		if (file->eof()){
			throw Exception("Unexpected end of MIDI file.");
		}
		return file->get();
	}

	///
	/// \brief Read a variable-length quantity value
	///
	/// See Standard MIDI Files 1.0 Specification, page 2.
	///
	/// \param file File to read from
	/// \return value read
	///
	static uint32_t readVarLengthQuantity(std::ifstream *file){
		uint32_t val=0;
		uint8_t b;
		do {
			b  = MidiFile::getByte(file);
			val = (val << 7) + (b & 0x7F);
		} while (b & 0x80);
		return val;
	}


	MidiFile();

	~MidiFile();



	/// Error message when convertToRosegarden() fails.
	//	std::string getError() const  { return m_error; }

	/// Convert a Rosegarden composition to a MIDI file.
	/*
	     * Returns true on success.
	     *
	     * See RosegardenMainWindow::exportMIDIFile().
	     */
	//	bool convertToMidi(RosegardenDocument *doc, const QString &filename);

	//	void setProgressDialog(QPointer<QProgressDialog> progressDialog)
	//	{ m_progressDialog = progressDialog; }

	// convertToMidi() uses MidiInserter.
	// MidiInserter uses:
	// - m_timingDivision
	// - m_format
	// - m_numberOfTracks
	// - m_midiComposition
	// - clearMidiComposition()
	//friend class MidiInserter;

	// *** Standard MIDI File Header

	enum FileType {
		MIDI_SINGLE_TRACK_FILE          = 0x00,  /**< Format 0*/
		MIDI_SIMULTANEOUS_TRACK_FILE    = 0x01,  // Format 1
		MIDI_SEQUENTIAL_TRACK_FILE      = 0x02,  // Format 2
		MIDI_CONVERTED_TO_APPLICATION   = 0xFE,
		MIDI_FILE_NOT_LOADED            = 0xFF
	};
	FileType m_format;

	const char * getFormatName() const{
		switch (m_format){
		case MIDI_SINGLE_TRACK_FILE:
			return "0 - Single Multi-Channel-Track";
		case MIDI_SIMULTANEOUS_TRACK_FILE:
			return "1 - Simultaneous Track(s)";
		case MIDI_SEQUENTIAL_TRACK_FILE:
			return "2 - Sequentially Independent Single-Track(s)";
		default:
			return "Unknown";
		}
	}

	int getNumberOfTracks() const{
		return m_numberOfTracks;
	}

	/**
	 * @brief Get time division of the MIDI file
	 *
	 * Time division is often refereed as ticks per beat or pulse
	 * per quarter (PPQ).
	 *
	 * @return time division
	 */
	int getTimeDivision(){
		return m_time_division;
	}

	unsigned m_numberOfTracks;

	enum TimingFormat {
		MIDI_TIMING_PPQ_TIMEBASE,
		MIDI_TIMING_SMPTE
	};
	TimingFormat m_timingFormat;

	/// Pulses Per Quarter note.  (m_timingFormat == MIDI_TIMING_PPQ_TIMEBASE)
	int m_time_division;

	// For SMPTE.
	int m_fps;
	int m_subframes;
	int m_nextId;




	// *** Internal MIDI Composition

	/**
	     * Our internal MIDI composition is just a vector of MidiEvents.
	     * We use a vector and not a set because we want the order of
	     * the events to be arbitrary until we explicitly sort them
	     * (necessary when converting Composition absolute times to
	     * MIDI delta times).
	     */
	typedef std::vector<MidiEvent *> MidiTrack;
	typedef std::map<TrackId, MidiTrack> MidiComposition;
	//MidiComposition m_midiComposition;
	//void clearMidiComposition();

	// *** Standard MIDI File to Rosegarden

	/// Read a MIDI file into m_midiComposition.
	bool read(const std::string &filename);

	void readHeader(std::ifstream *midiFile);
	/// Convert a track to events in m_midiComposition.
	void readTrack(std::ifstream *midiFile, uint32_t len, XMidiList *track);
	// m_midiComposition track to MIDI channel.
	//	std::map<TrackId, int /*channel*/> m_trackChannelMap;
	// Names for each track.
	// std::vector<std::string> m_trackNames;



	/// Find the next track chunk and set m_trackByteCount.
	uint32_t findNextTrack(std::ifstream *midiFile);



	/// Combine each note-on/note-off pair into a single note event with a duration.

	//void consolidateNoteEvents(TrackId trackId);


	/// Configure the Instrument based on events in Segment at time 0.
	//	static void configureInstrument(
	//			Track *track, Segment *segment, Instrument *instrument);

	// Read
	/// Read a "variable-length quantity" from the MIDI byte stream.
	/**
	     * In case the first byte has already been read, it can be sent
	     * in as firstByte.
	     */
	long readNumber(std::ifstream *midiFile, int firstByte = -1);
	//	MidiByte readString(std::ifstream *midiFile);

	std::string readString(std::ifstream *midiFile,
			       unsigned long numberOfBytes);

	// Conversion
	int midiBytesToInt(const std::string &bytes);
	long midiBytesToLong(const std::string &bytes);

	///
	/// \brief Read a long value (4 bytes) from MIDI file
	///
	/// This is basically nothen else, than reading a long
	/// value in big endia format from a file.
	///
	/// \param file File to read the value from.
	/// \return The long value read.
	///
	static uint32_t readLong(std::ifstream *file);

	//	size_t m_fileSize;
	/// Number of bytes left to read in the current track.
	// long m_trackByteCount;
	/// Allow decrementing of m_trackByteCount while reading.
	/**
	     * Set to true while processing a track.  false while processing other
	     * parts of the file.
	     */
	//bool m_decrementCount;

	//std::string m_error;

	// *** Rosegarden to Standard MIDI File

	/// Write m_midiComposition to a MIDI file.
	bool write(const std::string &filename);
	void writeHeader(std::ofstream *midiFile);
	void writeTrack(std::ofstream *midiFile, TrackId trackNumber);

	// Write
	/// Write an int as 2 bytes.
	void writeInt(std::ofstream *midiFile, int number);
	/// Write a long as 4 bytes.
	void writeLong(std::ofstream *midiFile, unsigned long number);


	void dump_info();


	std::vector<XMidiList *> tracks;

};

/**
 * @}
 */








} //namespace

#endif // MIDIFILE_H
