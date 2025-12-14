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
#ifndef DAWESESS_H
#define DAWESESS_H

#include "dawe.h"

#include "Device.h"

namespace Dawe {

class Transport {
public:
	typedef enum {
		ROLLING,
		STOPPED,
		STARTING,
		LOOPING,
		UNDEF
	} Sate;
};

class Session
{
public:
	Session(const char *name,nmiditicks_T ticks_per_beat=960);
	virtual ~Session();
	typedef enum {
		FLOAT,
		S32
	} dawe_format_T;



	dawe_format_T format;

//	Session(dawe_format_T format, int sampling_rate);

	void setup();
	virtual void print_info();



	/**
	 * @brief transportStart
	 * Start the transport rolling.
	 *
	 * It takes effect no sooner than the next process cycle,
	 * perhaps later if there are slow-sync clients.
	 * This function is realtime-safe.
	 */
	void transportStart();

	/**
	 * @brief transportStop
	 * Stop the transport.
	 *
	 * It takes effect on the next process cycle.
	 * This function is realtime-safe.
	 */
	void transportStop();

	/**
	 * @brief Get the number of midi ticks per beat.
	 *
	 * This is the so called time division of a midi track/file - the
	 * resolution of a midi file.
	 * Sometimes also called pulse per quarter (PPQ). This value defaults in
	 * libdave to 960 and should be sufficient for most applications.
	 * You can set this value only in the constructor of a session object.
	 * @return number of midi ticks per beat
	 * @see #setTicksPerBeat
	 */
	inline nmiditicks_T getTicksPerBeat(){ return ticks_per_beat; }

	/**
	 * @brief Set the number of ticks per beat
	 *
	 * For explaination see #getTicksPerBeat.
	 * @param n Number of ticks per beat
	 */
	inline void setTicksPerBeat(nmiditicks_T n){ ticks_per_beat=n; }

	/**
	 * @brief Get the sampling rate
	 *
	 * The sampling rate defaults in libdawe to 48000 frames per second,
	 * but could be dictated by the selected master sound card to a a
	 * different value.
	 * @return number of frames per second
	 */
	inline nframes_T getSamplingRate(){ return sampling_rate; }

public:

//private:


protected:
	const char * name;
	nmiditicks_T ticks_per_beat;
	Device * masterDevice;
	nframes_T sampling_rate;


};

}

#endif // DAWESESS_H
