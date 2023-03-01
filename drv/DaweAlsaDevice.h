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
#ifndef DAWEALSADEVICE_H
#define DAWEALSADEVICE_H

extern "C" {
#include <alsa/asoundlib.h>
}

#include "../DaweDevice.h"

class DaweAlsaDevice : public DaweDevice
{
public:
	DaweAlsaDevice(const char * name);
	~DaweAlsaDevice();
	void print_info();
private:
	snd_pcm_t *pcm_handle;
	snd_pcm_sw_params_t *swparams;

	unsigned int pcm_channles;

	void setup();
	const char * device_name;
	unsigned int pcm_max_channels;
	unsigned int pcm_min_channels;
	unsigned int pcm_min_rate, pcm_max_rate;
	int direction;

	bool pcm_mmap_interleaved,pcm_mmap_noninterleaved;
	bool pcm_mmap_complex;
	bool pcm_interleaved,pcm_noninterleaved;

};

#endif // DAWEALSADEVICE_H
