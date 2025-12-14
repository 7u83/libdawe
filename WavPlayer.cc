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
#include <iostream>

#include <jack/transport.h>

#include "WavPlayer.h"
#include "Exception.h"


namespace Dawe {

int WavPlayer::process_callback(uint32_t nframes)
{
	float * bd0,*bd1;
	bd0 = (float*)ports[0]->get_buffer(nframes);
	bd1 = (float*)ports[1]->get_buffer(nframes);

	switch (get_transport()) {
	case Transport::STOPPED:
	case Transport::UNDEF:
		memset(bd0,0,sizeof(float)*nframes);
		memset(bd1,0,sizeof(float)*nframes);
		return 0;
	case Transport::STARTING:
	default:
		break;
	}

	size_t avail;

	do {
		avail = buf->read_avail();
		if (avail > nframes)
			avail=nframes;

		float *bs =  buf->get_read_buf();

		//std::cout << "AVAIL" << avail << std::endl;

		for (size_t i =0; i<avail; i++){
			*bd0=bs[i*2];
			bd0++;
			*bd1=bs[i*2+1];
			bd1++;
		}

		buf->update_read(avail);
		nframes -= avail;

		if (avail==0){
			memset(bd0,0,sizeof(float)*nframes);
			memset(bd1,0,sizeof(float)*nframes);
			break;
		}


	} while (nframes>0);
	sem.post();
	return 0;
}


WavPlayer::WavPlayer(const char *name, const char *server_name) : JackDevice(name,server_name)
{
	buf=0;

	//	sem = Semaphore();

	//const char * filename = "fisher_z.wav";
	//const char * filename = "midnight_man.wav";
	// const char * filename = "karat_albatros.wav";
	const char * filename = "flirts_passion.wav";
	// const char * filename = "gwen_tictac.wav";
	//const char * filename = "prayer_in_c.wav";
	// const char * filename = "leonel.wav";

	//sfinfo;
	memset (&sfinfo,0,sizeof(sfinfo));

	//	jack_status_t status = (jack_status_t)0 ;
	sndfile = sf_open (filename, SFM_READ, &sfinfo) ;
	if (sndfile == NULL)
		throw Exception("Can't open file: %s",filename);

	std::cout << "FORMAT: " << sfinfo.format << std::endl;
	std::cout << "CHANNELS: " << sfinfo.channels << std::endl;
	std::cout << "RATE: " << sfinfo.samplerate << std::endl;
	std::cout << "SEEKABLE: " << sfinfo.seekable << std::endl;

	/*	client = jack_client_open ("daweWavPlayer",  JackNoStartServer, &status);
	if (!client){
		throw Exception("Can' open client");
	}
*/

	for (int i = 0 ; i < sfinfo.channels ; i++)
	{	char name [16] ;

		snprintf (name, sizeof (name), "out_%d", i + 1) ;
		//ports [i] = jack_port_register (client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0) ;
		register_port(name, Port::AUDIO);
	};

	buf = new Dawe::RingBuffer<float>(1024*200+370,2);

	//	jack_set_process_callback (client, jcallback, this) ;


}

WavPlayer::~WavPlayer()
{
	if (sndfile)
		sf_close(sndfile);
	if (buf)
		delete buf;
}

int WavPlayer::sync_callback(Device::Transport::Sate state, uint32_t frame)
{
	(void)state;
	int rc;
	std::cout << "SYNC CALLBACK" << std::endl;
	rc = sf_seek(sndfile,frame,SEEK_SET);
	std::cout << "FRAME:" << frame << std::endl;
	std::cout << "RC: " << rc << std::endl;
	if (rc==-1){
		sf_seek(sndfile,0,SEEK_END);
	}
	buf->reset();
	fill_buf();
	return 1;
}

int WavPlayer::fill_buf()
{
	size_t avail;
	sf_count_t r;
	while ((avail = buf->write_avail()) !=0){
		r = sf_readf_float(this->sndfile,buf->get_write_buf(),avail);
		buf->update_write(r);
		if(r<(long)avail)
			return 0;
	}
	return 1;
}

void WavPlayer::play()
{
	fill_buf();
	activate ();
	do {
		this->sem.wait();
	}
	while(this->fill_buf());

}




}
