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

#include <iostream>

#include "../Exception.h"
#include "DaweAlsaDevice.h"

#include "../dawe.h"

namespace Dawe{

DaweAlsaDevice::DaweAlsaDevice ( Session *sess, const char * name )
{
    this->device_name=name;
    this->direction = SND_PCM_STREAM_CAPTURE;
    this->sess=sess;
    setup();
}

void DaweAlsaDevice::setup()
{

    // open the device
    int rc = snd_pcm_open ( &pcm_handle, device_name,
			    SND_PCM_STREAM_PLAYBACK,
			    SND_PCM_NONBLOCK );
    if ( rc )
    {
	pcm_handle=0;
	throw Exception ( "Can't open device '%s': %d - %s\n",
			      device_name,rc, snd_strerror ( rc ) );
    }

    // allocate memory for hw params
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_hw_params_alloca ( &hwparams );

    // read out hw params
    rc = snd_pcm_hw_params_any ( pcm_handle, hwparams );
    if ( rc<0 )
	throw Exception ( "Can't get hw params for device '%s': %d - %s\n",
			      device_name,rc,snd_strerror ( rc ) );


    rc = snd_pcm_hw_params_get_channels_max ( hwparams, &pcm_max_channels );
    if ( rc<0 )
    {
	throw Exception (
	    "Can't get max number of channels for device '%s' %d %s\n",device_name,rc,snd_strerror ( rc )
	);
    }

    rc = snd_pcm_hw_params_get_channels_min ( hwparams, &pcm_min_channels );
    if ( rc<0 )
    {
	throw Exception (
	    "Can't get min number of channels for device '%s' %d %s\n",
	    device_name,rc,  snd_strerror ( rc )
	);
    }

    rc = snd_pcm_hw_params_get_rate_min ( hwparams, &pcm_min_rate, NULL );
    if ( rc<0 )
    {
	throw Exception ( "Can't get min sample rate for device '%s' %d %s\n",
			      device_name,rc,snd_strerror ( rc )
			    );
    }
    rc = snd_pcm_hw_params_get_rate_max ( hwparams, &pcm_max_rate, NULL );
    if ( rc<0 )
    {
	throw Exception ( "Can't get max sample rate for device '%s' %d %s\n",
			      device_name,rc,
			      snd_strerror ( rc ) );
    }



    rc = snd_pcm_hw_params_test_access ( pcm_handle,hwparams,
					 SND_PCM_ACCESS_RW_INTERLEAVED );
    pcm_interleaved	= rc ? false:true;

    rc = snd_pcm_hw_params_test_access ( pcm_handle,hwparams,
					 SND_PCM_ACCESS_RW_NONINTERLEAVED );
    pcm_noninterleaved = rc ? false:true;


    rc = snd_pcm_hw_params_test_access ( pcm_handle,hwparams,
					 SND_PCM_ACCESS_MMAP_INTERLEAVED );
    pcm_mmap_interleaved = rc ? false:true;

    rc = snd_pcm_hw_params_test_access ( pcm_handle,hwparams,
					 SND_PCM_ACCESS_MMAP_NONINTERLEAVED );
    pcm_mmap_noninterleaved = rc ? false:true;

    rc = snd_pcm_hw_params_test_access ( pcm_handle,hwparams,
					 SND_PCM_ACCESS_MMAP_COMPLEX );
    pcm_mmap_complex = rc ? false:true;



    int k;

    for ( k = 0; k < SND_PCM_FORMAT_LAST; ++k )
    {
	const char *s = snd_pcm_format_name ( ( snd_pcm_format_t ) k );
	if ( s )
	{
	    rc = snd_pcm_hw_params_test_format (
		     pcm_handle,
		     hwparams,
		     ( snd_pcm_format_t ) k );

	    printf ( " %s: %s\n", s, rc ? "NO":"TRUE" );
	}
    }


    return;

    snd_ctl_t *ctl_handle;
    rc = snd_ctl_open ( &ctl_handle, device_name, 0 );
    if ( rc )
    {
	pcm_handle=0;
	throw Exception ( "Can't open ctl device: %d %s\n",rc, snd_strerror ( rc ) );
	return;
    }

    snd_ctl_card_info_t *info;
    snd_ctl_card_info_alloca ( &info );

    rc = snd_ctl_card_info ( ctl_handle, info );
    if ( rc )
    {
	pcm_handle=0;
	throw Exception ( "card info: %d %s\n",rc, snd_strerror ( rc ) );
	return;
    }
    return;

}

void DaweAlsaDevice::print_info()
{
    printf ( "Sample Rate, min: %u - max: %u\n",
	     pcm_min_rate,
	     pcm_max_rate );
    printf ( "Number of channels, min: %u - max: %u\n",
	     pcm_min_channels,
	     pcm_max_channels );

    printf ( "Interleaved: %s\n",pcm_interleaved ? "true":"false" );
    printf ( "Non-Interleaved: %s\n",pcm_noninterleaved ? "true":"false" );
    printf ( "MMAP Interleaved: %s\n",pcm_mmap_interleaved ? "true":"false" );
    printf ( "MMAP Non-Interleaved: %s\n",pcm_mmap_noninterleaved ? "true":"false" );
    printf ( "MMAP Complex: %s\n",pcm_mmap_complex ? "true":"false" );
}

DaweAlsaDevice::~DaweAlsaDevice()
{
    std::cout << "Delete DaweAlsaDevice\n";
    if ( pcm_handle )
    {
	snd_pcm_close ( pcm_handle );
    }
}

void DaweAlsaDevice::set_hw_params()
{
    // allocate memory for hw params
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_alloca ( &hw_params );
    int rc = snd_pcm_hw_params_any(this->pcm_handle, hw_params);
    if (rc<0)
	throw Exception(
	    "Can't get hw_params for device '%s' - %s",
	    this->device_name, snd_strerror(rc)
	);
    rc = snd_pcm_hw_params_set_access(pcm_handle, hw_params,
				      SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc<0)
	throw Exception(
	    "Acces type not available '%s' - %s",
	    this->device_name, snd_strerror(rc)
	);

    snd_pcm_format_t format = SND_PCM_FORMAT_S32_LE;

    rc = snd_pcm_format_cpu_endian(format);
    printf("ENDIAN CPU: %d\n",rc);

    rc = snd_pcm_hw_params_set_format(pcm_handle, hw_params, format);
    if (rc<0)
	throw Exception(
	    "Format not supported for device '%s' - %s",
	    this->device_name, snd_strerror(rc)
	);

    unsigned int rate=sess->getSamplingRate();
    rc = snd_pcm_hw_params_set_rate_near(pcm_handle,hw_params,&rate,0);
    if (rc<0)
	throw Exception(
	    "Rate not supported for device '%s' - %s",
	    this->device_name, snd_strerror(rc)
	);

    printf("REAL RATE: %d\n",rate);
}

void DaweAlsaDevice::run()
{
  //  uint8_t buffer[256000];
    this->set_hw_params();

}

}
