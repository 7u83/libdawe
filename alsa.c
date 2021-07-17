
#include "dawe.h"
#include "dawe_wav.h"

#include <alsa/asoundlib.h>

static int get_alsa_fmt(uint16_t bits,uint16_t encoding)
{
	int i;
	static struct fmt {
		uint16_t bits;
		uint16_t encoding;
		int alsa_fmt;
	} fmts[] = {
	{16,WAV_FMT_PCM,SND_PCM_FORMAT_S16_LE},
	{32,WAV_FMT_PCM,SND_PCM_FORMAT_S32_LE},
	{32,WAV_FMT_IEEE_FLOAT,SND_PCM_FORMAT_FLOAT_LE},
	{0,0,SND_PCM_FORMAT_UNKNOWN}
	};
	struct fmt *p = fmts;
	do if (p->bits==bits && p->encoding==encoding)
			return p->alsa_fmt;
	while((++p)->bits);

	return SND_PCM_FORMAT_UNKNOWN;
}

struct dawe_device * dawe_alsa_create_device(
		const char *name, dawe_wav_t *w)
{
	int rc;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	snd_output_t *log;
	unsigned int rrate;

	snd_pcm_hw_params_malloc(&params);

	printf("open pcm\n");
	rc = snd_pcm_open(&handle, name,
					  SND_PCM_STREAM_PLAYBACK | SND_PCM_STREAM_CAPTURE, 0);
	if (rc){
		fprintf(stderr,"Can't open device: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	printf("Get HW Params\n");
	rc = snd_pcm_hw_params_any(handle, params);
	if (rc){
		fprintf(stderr,"Can't open device: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	printf("Attach stdio \n");
	rc = snd_output_stdio_attach(&log, stdout, 0);
	if (rc){
		fprintf(stderr,"Can't open device: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	fprintf(stderr, "FULLL HW Params of device \"%s\":\n",
			snd_pcm_name(handle));
	fprintf(stderr, "--------------------\n");
	snd_pcm_hw_params_dump(params, log);
	fprintf(stderr, "--------------------\n");



	rc = snd_pcm_hw_params_set_access (handle, params,
									   SND_PCM_ACCESS_RW_INTERLEAVED);

	if (rc){
		fprintf(stderr,"Can't set_access: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}




	{
	int afm = get_alsa_fmt(w->bits_per_sample,
						   w->encoding);
	printf("AFM: %d\n",afm);
	}
	rc = snd_pcm_hw_params_set_format (handle, params,/*SND_PCM_FORMAT_FLOAT_LE*/
									   get_alsa_fmt(w->bits_per_sample,
													w->encoding)

									   );

	if (rc){
		fprintf(stderr,"Can't set format: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}
	rrate = w->sample_rate;
	printf("RRATE BEFORE: %d\n",rrate);
	rc = snd_pcm_hw_params_set_rate_near (handle, params, &rrate, NULL);
	if (rc){
		fprintf(stderr,"Can't set sempla rate: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	printf ("Set Sample %d\n",rrate);

	rc = snd_pcm_hw_params_set_channels (handle, params, 2 /*w->channels*/);
	if (rc){
		fprintf(stderr,"Can't set channels: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}



	{
/*		snd_pcm_uframes_t buffer_size = 2048*80;
		snd_pcm_uframes_t period_size = 2048;*/

/*		snd_pcm_uframes_t buffer_size = 4096*4;
		snd_pcm_uframes_t period_size = 4096;

		snd_pcm_hw_params_set_buffer_size_near (handle, params, &buffer_size);
		snd_pcm_hw_params_set_period_size_near (handle, params, &period_size, NULL);*/
	}


	fprintf(stderr, "HW Params of device \"%s\":\n",
			snd_pcm_name(handle));
	fprintf(stderr, "--------------------\n");
	snd_pcm_hw_params_dump(params, log);
	fprintf(stderr, "--------------------\n");

	rc = snd_pcm_hw_params (handle, params);
	if (rc){
		fprintf(stderr,"Can't apply hw param %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}




	{
	snd_pcm_sw_params_t *sw_params;
	snd_pcm_uframes_t nframes;

	snd_pcm_sw_params_malloc (&sw_params);

	snd_pcm_sw_params_current (handle, sw_params);

	snd_pcm_sw_params_get_avail_min(sw_params,&nframes);
	if (rc){
		fprintf(stderr,"Can't get mit avail: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}
	printf("AVAIL MIN IS: %ld - %0.2f ms\n",nframes,
	   frames_to_ms(w->sample_rate,nframes));

	snd_pcm_sw_params_get_start_threshold(sw_params,&nframes);
	if (rc){
		fprintf(stderr,"Can't get mit avail: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	printf("START  IS: %ld - %0.2f ms\n",nframes,
	   frames_to_ms(w->sample_rate,nframes));

	nframes = 16;
/*	rc = snd_pcm_sw_params_set_avail_min(handle,sw_params,nframes);
	if (rc){
		fprintf(stderr,"Can't set avail min %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}*/

	rc = snd_pcm_sw_params_set_start_threshold(handle,sw_params,nframes);
	if (rc){
		fprintf(stderr,"Can't set avail min %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	rc = snd_pcm_sw_params(handle, sw_params);
	if (rc){
		fprintf(stderr,"Can't set SW Params: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	snd_pcm_sw_params_current (handle, sw_params);
	snd_pcm_sw_params_get_start_threshold(sw_params,&nframes);
	if (rc){
		fprintf(stderr,"Can't get mit avail: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	printf("START  IS: %ld - %0.2f ms\n",nframes,
	   frames_to_ms(w->sample_rate,nframes));

	}






/*	 fprintf(stderr, "%s, ", snd_pcm_format_description(hwparams.format)); */


	rc = snd_pcm_prepare (handle);
	if (rc){
		fprintf(stderr,"Can't prepare: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}
	{

		int ws=128;
		int div=w->bits_per_sample/8*w->channels;
		uint8_t sb[65536];
		fseek(w->file,w->data,SEEK_SET);
		while(!feof(w->file)){
			fread(sb,1,ws,w->file);
			rc = snd_pcm_writei(handle, sb, ws/div);
			if (rc!=ws/div){
				fprintf(stderr,"Wrote %d instead of %d\n",rc, ws/div);
	rc = snd_pcm_prepare (handle);
			/*	return NULL;*/
			}
			else{
				printf("wrote %d\n",ws/div);
			}
			usleep(500);
		}

	}



}


void play_alsa(dawe_wav_t *w)
{
	dawe_device_t * d;
	d = dawe_alsa_create_device("plughw",w);

}
