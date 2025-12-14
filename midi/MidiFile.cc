/*
 * Copyright (C) 2005-2022, rncbc aka Rui Nuno Capela. All rights reserved.
 * Copyright (C) 2023,  7u83 <7u83@mail.ru>
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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "MidiFile.h"
#include "../Exception.h"


namespace Dawe {

static const char MIDI_FILE_HEADER[] = "MThd";
static const char MIDI_TRACK_HEADER[] = "MTrk";

#define qstrtostr
#define tr

/*
#define RG_WARNING std::cout
#define RG_DEBUG std::cout
*/

class NullBuffer : public std::streambuf {
public:
    int overflow(int c) {
	return c;
    }
};

class NullStream : public std::ostream {
public:
    NullStream() : std::ostream(&m_sb) {}
private:
    NullBuffer m_sb;
};

NullStream null_stream;

#define RG_WARNING std::cout
#define RG_DEBUG null_stream


MidiFile::MidiFile() :
	m_format(MIDI_FILE_NOT_LOADED),
	m_numberOfTracks(0),
	m_timingFormat(MIDI_TIMING_PPQ_TIMEBASE),
	m_time_division(0),
	m_fps(0),
	m_subframes(0)
//	m_fileSize(0),
//	m_trackByteCount(0),
//	m_decrementCount(false),
//	m_bytesRead(0)
{
}


/**
 * Return the (approximate) number of quarters per minute for a
 * given tempo.
 */
//static double getTempoQpm(tempoT tempo) { return double(tempo) / 100000.0; }
//static tempoT getTempoForQpm(double qpm) { return tempoT(qpm * 100000 + 0.01); }

MidiFile::~MidiFile()
{
	// Delete all the event objects.
//	clearMidiComposition();
}

long
MidiFile::midiBytesToLong(const std::string &bytes)
{
	if (bytes.length() != 4) {
		RG_WARNING << "midiBytesToLong(): WARNING: Wrong length for long data (" << bytes.length() << ", should be 4)";

		// TRANSLATOR: "long" is a C++ data type
		throw Exception("Wrong length for long data in MIDI stream");
	}

	long longRet = static_cast<long>(static_cast<MidiByte>(bytes[0]))
			<< 24 | static_cast<long>(static_cast<MidiByte>(bytes[1]))
			<< 16 | static_cast<long>(static_cast<MidiByte>(bytes[2]))
			<< 8 | static_cast<long>(static_cast<MidiByte>(bytes[3]));

	RG_DEBUG << "midiBytesToLong(" << static_cast<long>(static_cast<MidiByte>(bytes[0]))
			<< "," << static_cast<long>(static_cast<MidiByte>(bytes[1]))
			<< "," << static_cast<long>(static_cast<MidiByte>(bytes[2]))
			<< "," << static_cast<long>(static_cast<MidiByte>(bytes[3]))
			<< ") -> " << longRet;

	return longRet;
}

uint32_t MidiFile::readLong(std::ifstream *file){
	uint32_t val;
	val = static_cast<uint32_t>(MidiFile::getByte(file))<<24;
	val += static_cast<uint32_t>(MidiFile::getByte(file))<<16;
	val += static_cast<uint32_t>(MidiFile::getByte(file))<<8;
	val += MidiFile::getByte(file);
	return val;
}

int
MidiFile::midiBytesToInt(const std::string &bytes)
{
	if (bytes.length() != 2) {
		RG_WARNING << "midiBytesToInt(): WARNING: Wrong length for int data (" << bytes.length() << ", should be 2)";

		// TRANSLATOR: "int" is a C++ data type
		throw Exception("Wrong length for int data in MIDI stream");
	}

	return static_cast<int>(static_cast<MidiByte>(bytes[0]))
			<< 8 |static_cast<int>(static_cast<MidiByte>(bytes[1]));
}

/*
MidiByte
MidiFile::read(std::ifstream *midiFile)
{
	return static_cast<MidiByte>(read(midiFile, 1)[0]);
}
*/

std::string
MidiFile::readString(std::ifstream *midiFile, unsigned long numberOfBytes)
{
	std::string str;

	while(str.length()<numberOfBytes){
		str+=MidiFile::getByte(midiFile);
	}
	return str;
}



uint32_t
MidiFile::findNextTrack(std::ifstream *file)
{
	// Conforms to recommendation in the MIDI spec, section 4, page 3:
	// "Your programs should /expect/ alien chunks and treat them as if
	// they weren't there."  (Emphasis theirs.)

	// Assume not found.
//	m_decrementCount = false;
//	m_trackByteCount = -1;

	// For each chunk
	while (!file->eof()) {
		// Read the chunk type and size.
		std::string chunkType = readString(file, 4);
		long chunkSize = readLong(file); // midiBytesToLong(read(file, 4));

		// If we've found a track chunk
		if (chunkType.compare(0, 4, MIDI_TRACK_HEADER) == 0) {
//			m_trackByteCount = chunkSize;
//			m_decrementCount = true;
			return chunkSize;
		}

//		RG_DEBUG << "findNextTrack(): skipping alien chunk.  Type:" << chunkType;

		// Alien chunk encountered, initiate evasive maneuvers (skip it).
		file->seekg(chunkSize, std::ios::cur);
	}

	// Track not found.
	RG_WARNING << "findNextTrack(): Couldn't find Track";
	throw Exception(qstrtostr(tr("File corrupted or in non-standard format")));
}

bool
MidiFile::read(const std::string &filename)
{
	RG_DEBUG << "read(): filename = " << filename;

//	clearMidiComposition();


	// Open the file
	std::ifstream *midiFile =
			new std::ifstream(filename.c_str(),
					  std::ios::in | std::ios::binary);


	if (!(*midiFile)) {
		throw Exception ( std::strerror( errno ));
		return false;
	}


	// The parsing process throws string exceptions back up here if we
	// run into trouble which we can then pass back out to whomever
	// called us using m_error and a nice bool.
	try {
		// Parse the MIDI header first.
		readHeader(midiFile);

		// For each track chunk in the MIDI file.
		for (unsigned track = 0; track < m_numberOfTracks; ++track) {

			// Skip any alien chunks.
			uint32_t len = findNextTrack(midiFile);
			// Read the track into m_midiComposition.

			XMidiList * l = new XMidiList();
			tracks.push_back(l);

			readTrack(midiFile,len,tracks[track]);

		/*	printf("=== dump track ===\n");
			for(size_t i=0; i<tracks[track]->size(); i++){
				printf("%d: ", tracks[track]->getAtIndex(i)->getTime());
				tracks[track]->getAtIndex(i)->dump();
			}*/
		}

	} catch (const Exception &e) {
		RG_WARNING << "read() - caught exception - " << e.what();

		//	m_error = e.getMessage();
//		m_error = e.what();
		m_format = MIDI_FILE_NOT_LOADED;
		return false;
	}

	midiFile->close();

	return true;
}

void
MidiFile::readHeader(std::ifstream *midiFile)
{
	// The basic MIDI header is 14 bytes.
	std::string midiHeader = readString(midiFile, 14);

	if (midiHeader.size() < 14) {
		throw Exception("Not a MIDI file");
	}

	if (midiHeader.compare(0, 4, MIDI_FILE_HEADER) != 0) {
		throw Exception("Not a MIDI file");
	}

	long chunkSize = midiBytesToLong(midiHeader.substr(4, 4));
	m_format = static_cast<FileType>(
				midiBytesToInt(midiHeader.substr(8, 2)));

	m_numberOfTracks = midiBytesToInt(midiHeader.substr(10, 2));
	m_time_division = midiBytesToInt(midiHeader.substr(12, 2));
	m_timingFormat = MIDI_TIMING_PPQ_TIMEBASE;

	if (m_format == MIDI_SEQUENTIAL_TRACK_FILE) {
		RG_WARNING << "parseHeader() - can't load sequential track (Format 2) MIDI file";
		throw Exception(qstrtostr(tr("Unexpected MIDI file format")));
	}

	if (m_time_division > 32767) {
		RG_DEBUG << "parseHeader() - file uses SMPTE timing";

		m_timingFormat = MIDI_TIMING_SMPTE;
		m_fps = 256 - (m_time_division >> 8);
		m_subframes = (m_time_division & 0xff);
	}

	if (chunkSize > 6) {
		// Skip any remaining bytes in the header chunk.
		// MIDI spec section 4, page 5: "[...] more parameters may be
		// added to the MThd chunk in the future: it is important to
		// read and honor the length, even if it is longer than 6."
		midiFile->seekg(chunkSize - 6, std::ios::cur);
	}
}

static const std::string defaultTrackName = "Imported MIDI";

void
MidiFile::readTrack(std::ifstream *midiFile, uint32_t len, XMidiList * track)
{
	uint8_t running_status=MidiEvent::NOTE_OFF;
	uint32_t time=0;
	while (len){
		MidiEvent e,e1;
		len-=e.fromStream(midiFile,running_status,time);
		//track->push_back(e);
		track->insert(e);
	//	e1=e;

	//	e1.dump();
	}
	//track->rebuild();
	printf("Track Loaded %ld\n",track->size());
	fflush(stdout);
}




void
MidiFile::writeInt(std::ofstream *midiFile, int number)
{
	*midiFile << static_cast<MidiByte>((number & 0xFF00) >> 8);
	*midiFile << static_cast<MidiByte>(number & 0x00FF);
}

void
MidiFile::writeLong(std::ofstream *midiFile, unsigned long number)
{
	*midiFile << static_cast<MidiByte>((number & 0xFF000000) >> 24);
	*midiFile << static_cast<MidiByte>((number & 0x00FF0000) >> 16);
	*midiFile << static_cast<MidiByte>((number & 0x0000FF00) >> 8);
	*midiFile << static_cast<MidiByte>(number & 0x000000FF);
}

void MidiFile::dump_info()
{
	printf("Format: %s\n",this->getFormatName());
	printf("Tracks: %d\n", this->getNumberOfTracks());
	printf("Time Division: %d\n",this->getTimeDivision());
}
/*
std::string
MidiFile::longToVarBuffer(unsigned long value)
{
	// See WriteVarLen() in the MIDI Spec section 4, page 11.

	// Convert value into a "variable-length quantity" in buffer.

	// Start with the lowest 7 bits of the number
	long buffer = value & 0x7f;

	while ((value >>= 7 ) > 0) {
		buffer <<= 8;
		buffer |= 0x80;
		buffer += (value & 0x7f);
	}

	// Copy buffer to returnString and return it.

	std::string returnString;

	while (true) {
		returnString += (MidiByte)(buffer & 0xff);
		if (buffer & 0x80)
			buffer >>= 8;
		else
			break;
	}

	return returnString;
}
*/
void
MidiFile::writeHeader(std::ofstream *midiFile)
{
	// Our identifying Header string
	*midiFile << MIDI_FILE_HEADER;

	// Write number of Bytes to follow
	*midiFile << static_cast<MidiByte>(0x00);
	*midiFile << static_cast<MidiByte>(0x00);
	*midiFile << static_cast<MidiByte>(0x00);
	*midiFile << static_cast<MidiByte>(0x06);

	writeInt(midiFile, static_cast<int>(m_format));
	writeInt(midiFile, m_numberOfTracks);
	writeInt(midiFile, m_time_division);
}

void
MidiFile::writeTrack(std::ofstream *midiFile, TrackId trackNumber)
{
	(void)midiFile; (void)trackNumber;

/*
	// For running status.
	MidiByte previousEventCode = 0;

	// First we write into trackBuffer, then we write trackBuffer
	// out to the file with its accompanying length.

	std::string trackBuffer;

	// Used to accumulate time deltas for skipped events.
	nmiditicks_T skippedTime = 0;

	// For each event in the Track
	for (MidiTrack::iterator i = m_midiComposition[trackNumber].begin();
	     i != m_midiComposition[trackNumber].end();
	     ++i) {
		const MidiEvent &midiEvent = **i;

		// Do not write controller reset events to the buffer/file.
		// HACK for #1404.  I gave up trying to find where the events
		// were originating, and decided to try just stripping them.  If
		// you can't do it right, do it badly, and somebody will
		// eventually freak out, then fix it the right way.
		// ??? This is created in ChannelManager::insertControllers().
		// ??? Since we now have the "Allow Reset All Controllers" config
		//     option, we can probably get rid of this and tell people to
		//     use the config option.  If someone complains that 121's
		//     aren't appearing in MIDI files, that's probably the route
		//     we'll have to go.
		if (midiEvent.getData1() == MIDI_CONTROLLER_RESET) {
			RG_WARNING << "writeTrack(): Found controller 121.  Skipping.  This is a HACK to address BUG #1404.";

			// Keep track of the timestamps from skipped events so we can
			// add them to the next event that makes it through.
			skippedTime += midiEvent.getTime();

			continue;
		}

		// Add the time to the buffer in MIDI format
		trackBuffer += longToVarBuffer(midiEvent.getTime() + skippedTime);

		skippedTime = 0;

		RG_DEBUG << "MIDI event for channel "
			 << static_cast<int>(midiEvent.getChannelNumber())
			 << " (track " << trackNumber << ") "
			 << " time" << midiEvent.getTime();
		//FIXTHIS
		// RG_DEBUG << midiEvent;

		if (midiEvent.isMeta()) {
			trackBuffer += MIDI_FILE_META_EVENT;
			trackBuffer += midiEvent.getMetaEventCode();

			trackBuffer += longToVarBuffer(midiEvent.
						       getMetaMessage().length());
			trackBuffer += midiEvent.getMetaMessage();

			// Meta events cannot use running status.
			previousEventCode = 0;

		} else {  // non-meta event
			// If the event code has changed, or this is a SYSEX event, we
			// can't use running status.
			// Running status is "[f]or Voice and Mode messages only."
			// Sysex is a system message.  See the MIDI spec, Section 2,
			// page 5.
			if ((midiEvent.getEventCode() != previousEventCode) ||
					(midiEvent.getEventCode() == MIDI_SYSTEM_EXCLUSIVE)) {

				// Send the normal event code (with encoded channel information)
				trackBuffer += midiEvent.getEventCode();

				previousEventCode = midiEvent.getEventCode();
			}

			switch (midiEvent.getMessageType()) {
			case MIDI_NOTE_ON:  // These have two data bytes.
			case MIDI_NOTE_OFF:
			case MIDI_PITCH_BEND:
			case MIDI_CTRL_CHANGE:
			case MIDI_POLY_AFTERTOUCH:
				trackBuffer += midiEvent.getData1();
				trackBuffer += midiEvent.getData2();
				break;

			case MIDI_PROG_CHANGE:  // These have one data byte.
			case MIDI_CHNL_AFTERTOUCH:
				trackBuffer += midiEvent.getData1();
				break;

			case MIDI_SYSTEM_EXCLUSIVE:
				trackBuffer +=
						longToVarBuffer(midiEvent.getMetaMessage().length());
				trackBuffer += midiEvent.getMetaMessage();
				break;

			default:
				RG_WARNING << "writeTrack() - cannot write unsupported MIDI event: "
					   << std::hex << midiEvent.getMessageType();
				break;
			}
		}


	}

	// Now we write the track to the file.

	*midiFile << MIDI_TRACK_HEADER;
	writeLong(midiFile, trackBuffer.length());
	*midiFile << trackBuffer;
	*/

}

bool
MidiFile::write(const std::string &filename)
{
	std::ofstream midiFile(filename.c_str(),
			       std::ios::out | std::ios::binary);

	if (!midiFile.good()) {
		RG_WARNING << "write() - can't write file";
		m_format = MIDI_FILE_NOT_LOADED;
		return false;
	}

	writeHeader(&midiFile);

	// For each track, write it out.
	for (TrackId i = 0; i < m_numberOfTracks; ++i) {
		writeTrack(&midiFile, i);

		/*	if (m_progressDialog  &&  m_progressDialog->wasCanceled())
	return false;

    if (m_progressDialog)
	m_progressDialog->setValue(i * 100 / m_numberOfTracks);
 */
	}

	midiFile.close();

	return true;
}

/*
void
MidiFile::consolidateNoteEvents(TrackId trackId)
{
	MidiTrack &track = m_midiComposition[trackId];

	// For each MIDI event on the track.
	for (MidiTrack::iterator firstEventIter = track.begin();
	     firstEventIter != track.end();
	     ++firstEventIter) {
		MidiEvent &firstEvent = **firstEventIter;

		// Not a note-on?  Try the next event.
		if (firstEvent.getMessageType() != MIDI_NOTE_ON)
			continue;

		// Note-on with velocity 0 is a note-off.  Try the next event.
		if (firstEvent.getVelocity() == 0)
			continue;

		// At this point, firstEvent is a note-on event.
		// Search for the matching note-off event.

		bool noteOffFound = false;

		MidiTrack::iterator secondEventIter;

		// For each following MIDI event
		for (secondEventIter = firstEventIter + 1;
		     secondEventIter != track.end();
		     ++secondEventIter) {
			const MidiEvent &secondEvent = **secondEventIter;

			bool noteOff = (secondEvent.getMessageType() == MIDI_NOTE_OFF  ||
					(secondEvent.getMessageType() == MIDI_NOTE_ON  &&
					 secondEvent.getVelocity() == 0x00));

			// Not a note-off?  Try the next event.
			if (!noteOff)
				continue;

			// Wrong pitch?  Try the next event.
			if (secondEvent.getPitch() != firstEvent.getPitch())
				continue;

			// Wrong channel?  Try the next event.
			// Note: Since all the events in a track are for a single channel,
			//       this will never be true.
			if (secondEvent.getChannelNumber() != firstEvent.getChannelNumber())
				continue;

			nmiditicks_T noteDuration = secondEvent.getTime() - firstEvent.getTime();

			// Some MIDI files floating around in the real world
			// apparently have note-on followed immediately by note-off
			// on percussion tracks.  Instead of setting the duration to
			// 0 in this case, which has no meaning, set it to 1.
			if (noteDuration == 0) {
				RG_WARNING << "consolidateNoteEvents() - detected MIDI note duration of 0.  Using duration of 1.  Touch wood.";
				noteDuration = 1;
			}

			firstEvent.setDuration(noteDuration);

			// Remove the note-off.
			delete *secondEventIter;
			track.erase(secondEventIter);

			noteOffFound = true;
			break;
		}

		if (!noteOffFound) {
			// avoid crash due to secondEventIter == track.end()
			--secondEventIter;
			// Set Event duration to length of Segment.
			firstEvent.setDuration(
						(*secondEventIter)->getTime() - firstEvent.getTime());
		}
	}
}
*/

/*
void
MidiFile::clearMidiComposition()
{
	// For each track
	for (MidiComposition::iterator trackIter = m_midiComposition.begin();
	     trackIter != m_midiComposition.end();
	     ++trackIter) {

		MidiTrack &midiTrack = trackIter->second;

		// For each event on the track.
		for (MidiTrack::iterator eventIter = midiTrack.begin();
		     eventIter != midiTrack.end();
		     ++eventIter) {

			delete *eventIter;
		}

		midiTrack.clear();
	}

	m_midiComposition.clear();
	m_trackChannelMap.clear();
	m_trackNames.clear();
}
*/

} // namespace Dawe


