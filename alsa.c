
/*#include <unistd.h>*/


#include "dawe.h"
#include "dawe_wav.h"

#include <alsa/asoundlib.h>


static int get_alsa_fmt(uint16_t bits,uint16_t encoding)
{
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

struct alsa_device {
		snd_pcm_t *handle;
		snd_pcm_hw_params_t *hwparams;
		snd_pcm_sw_params_t *swparams;
};
typedef struct alsa_device alsa_device_t;


#define ALSA_PARAM_PERIODS "periods"
#define ALSA_PARAM_PERIOD_SIZE "period_size"
#define ALSA_PARAM_CHANNELS "channels"
#define ALSA_PARAM_START_THRESHOLD "start_threshold"

static int set_param(dawe_device_t * d,const char *name,void *value)
{
	alsa_device_t *a;
	a=(alsa_device_t*)(d->data);
	int rc;

	if(!strcmp(name,ALSA_PARAM_PERIODS)){
		int val = *(int*)value;
		rc = snd_pcm_hw_params_set_periods(a->handle,a->hwparams,val,1);
		goto x;
	}
	if(!strcmp(name,ALSA_PARAM_PERIOD_SIZE)){
		int val = *(int*)value;
		rc = snd_pcm_hw_params_set_period_size(a->handle,a->hwparams,val,NULL);
		goto x;
	}

	if(!strcmp(name,ALSA_PARAM_CHANNELS)){
		int val = *(int*)value;
		rc = snd_pcm_hw_params_set_channels (a->handle, a->hwparams, val);

		goto x;
	}

	if(!strcmp(name,"avail_min")){
		int val = *(int*)value;
		rc = snd_pcm_sw_params_set_avail_min(a->handle,a->swparams,val);
		goto x;
	}
	if(!strcmp(name,ALSA_PARAM_START_THRESHOLD)){
		int val = *(int*)value;
		rc = snd_pcm_sw_params_set_start_threshold(a->handle,a->swparams,val);
			if (!rc)
				rc = snd_pcm_sw_params(a->handle, a->swparams);
		goto x;
	}


	return -ENOENT;
x:
	if (!rc)
		return rc;

	fprintf(stderr,"Can't set %s: %d %s\n",name,rc, snd_strerror(rc));
	return rc;
}

static int start(dawe_device_t * d)
{
	alsa_device_t *a;
	a=(alsa_device_t*)(d->data);
	int rc;

	rc = snd_pcm_hw_params (a->handle, a->hwparams);
	if (rc){
		fprintf(stderr,"Can't apply hw param %d %s\n",rc, snd_strerror(rc));
	}

	rc = snd_pcm_sw_params_malloc (&a->swparams);
	if (rc<0)
		exit(0);
	rc = snd_pcm_sw_params_current (a->handle, a->swparams);
	if (rc){
		printf("cant get current sw params\n");
		exit(0);
	}

	return rc;
}


static 		snd_output_t *log=NULL;
static void dump(dawe_device_t *d)
{
	alsa_device_t * a = d->data;
	int rc;
	rc = snd_output_stdio_attach(&log, stdout, 0);
	if (rc){
		fprintf(stderr,"output stdio attach: %d %s\n",rc, snd_strerror(rc));
		return;
	}

	fprintf(stderr, "FULLL HW Params of device \"%s\":\n",
			snd_pcm_name(a->handle));
	snd_pcm_hw_params_dump(a->hwparams, log);
	fprintf(stderr, "--------------------\n");

}

struct dawe_device * dawe_alsa_create_devicex(
		const char *name, dawe_wav_t *w, int dir, int mode)
{
	int rc;
	snd_output_t *log;
	unsigned int rrate;
	dawe_device_t * d;
	alsa_device_t * a;

	d = dawe_device_create();
	if (!d)
		return NULL;

	a = malloc(sizeof(alsa_device_t));
	if (!a)
		goto errX;
	memset(a,0,sizeof(alsa_device_t));
	d->data=a;
	d->set_param=set_param;
	d->start=start;

	rc = snd_pcm_hw_params_malloc(&a->hwparams);
	if (rc<0)
		goto errX;


	rc = snd_pcm_open(&a->handle, name, dir, mode);
	if (rc){
		fprintf(stderr,"Can't open device: %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}

	rc = snd_pcm_hw_params_any(a->handle, a->hwparams);
	if (rc<0){
		fprintf(stderr,"Error hw_params_any: %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}

	rc = snd_output_stdio_attach(&log, stdout, 0);
	if (rc){
		fprintf(stderr,"output stdio attach: %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}

	fprintf(stderr, "FULLL HW Params of device \"%s\":\n",
			snd_pcm_name(a->handle));
	fprintf(stderr, "DUMP BEFORE --------------------\n");
	snd_pcm_hw_params_dump(a->hwparams, log);
	fprintf(stderr, "--------------------\n");


	rc = snd_pcm_hw_params_set_access (a->handle, a->hwparams,
									   SND_PCM_ACCESS_RW_INTERLEAVED);

	if (rc){
		fprintf(stderr,"Can't set_access: %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}




	{
	int afm = get_alsa_fmt(w->bits_per_sample,  w->encoding);
	printf("AFM: %d\n",afm);
	}
	rc = snd_pcm_hw_params_set_format (a->handle, a->hwparams,/*SND_PCM_FORMAT_FLOAT_LE*/
									   get_alsa_fmt(w->bits_per_sample,
													w->encoding)

									   );

	if (rc){
		fprintf(stderr,"Can't set format: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}
	rrate = w->sample_rate;
	printf("RRATE BEFORE: %d\n",rrate);
	rc = snd_pcm_hw_params_set_rate_near (a->handle, a->hwparams, &rrate, NULL);
	if (rc){
		fprintf(stderr,"Can't set sempla rate: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	printf ("Set Sample %d\n",rrate);




	rc = snd_pcm_hw_params (a->handle, a->hwparams);
	if (rc){
		fprintf(stderr,"Can't apply hw param %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}

	fprintf(stderr, "HW Params of device \"%s\":\n",snd_pcm_name(a->handle));
	fprintf(stderr, "DUMP AFTER --------------------\n");
	snd_pcm_hw_params_dump(a->hwparams, log);
	fprintf(stderr, "--------------------\n");


	{
	snd_pcm_sw_params_t *sw_params;
	snd_pcm_uframes_t nframes;

	snd_pcm_sw_params_malloc (&sw_params);

	snd_pcm_sw_params_current (a->handle, sw_params);

	rc = snd_pcm_sw_params_get_avail_min(sw_params,&nframes);
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

	nframes = 21000*2;
/*	rc = snd_pcm_sw_params_set_avail_min(handle,sw_params,nframes);
	if (rc){
		fprintf(stderr,"Can't set avail min %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}*/

	rc = snd_pcm_sw_params_set_start_threshold(a->handle,sw_params,nframes);
	if (rc){
		fprintf(stderr,"Can't set avail min %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

/*	rc = snd_pcm_sw_params(a->handle, sw_params);
	if (rc){
		fprintf(stderr,"Can't set SW Params: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}
*/
	snd_pcm_sw_params_current (a->handle, sw_params);
	snd_pcm_sw_params_get_start_threshold(sw_params,&nframes);
	if (rc){
		fprintf(stderr,"Can't get mit avail: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	printf("START  IS: %ld - %0.2f ms\n",nframes,
		   frames_to_ms(w->sample_rate,nframes));

	}


	return d;

errX:
	if (rc<0){
		fprintf(stderr,"ERR: %s\n",snd_strerror(rc));
	}
	if (d)
		dawe_device_destroy(d);

	if (a){
		if (a->handle){
			snd_pcm_close(a->handle);
		}
		free(a);
	}
	return NULL;
}

struct dawe_device * dawe_alsa_create_device(dawe_sess_t *s,
		const char *name, int dir, int mode)
{
	int rc;
	unsigned int rrate;
	dawe_device_t * d;
	alsa_device_t * a;

	d = dawe_device_create();
	if (!d)
		return NULL;

	a = malloc(sizeof(alsa_device_t));
	if (!a)
		goto errX;
	memset(a,0,sizeof(alsa_device_t));
	d->data=a;
	d->set_param=set_param;
	d->start=start;

	/* open audio device */
	rc = snd_pcm_open(&a->handle, name, dir, mode);
	if (rc){
		fprintf(stderr,"Can't open device: %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}

	/* allocate memory for hw and sw params */
	rc = snd_pcm_hw_params_malloc(&a->hwparams);
	if (rc<0)
		goto errX;

	/* read current hw and sw params */
	rc = snd_pcm_hw_params_any(a->handle, a->hwparams);
	if (rc<0){
		fprintf(stderr,"Error hw_params_any: %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}


	rc = snd_pcm_hw_params_set_format (a->handle, a->hwparams,
									   get_alsa_fmt(s->bits_per_sample,
													s->format)
									   );
	if (rc){
		fprintf(stderr,"Can't set xformat: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}

	rrate = s->sampling_rate;
	rc = snd_pcm_hw_params_set_rate_near (a->handle, a->hwparams, &rrate, NULL);
	if (rc){
		fprintf(stderr,"Can't set sampla rate: %d %s\n",rc, snd_strerror(rc));
		return NULL;
	}


	rc = snd_pcm_hw_params_set_access (a->handle, a->hwparams,
									   SND_PCM_ACCESS_RW_INTERLEAVED);
	if (rc){
		fprintf(stderr,"Can't set_access: %d %s\n",rc, snd_strerror(rc));
		goto errX;
	}

	{
			unsigned int val=3;
		rc =rc=snd_pcm_hw_params_get_period_size_min(a->hwparams,&val,0);
		if (rc){
			fprintf(stderr,"Can't get periods: %d %s\n",rc, snd_strerror(rc));
			return NULL;
		}
		printf("Got periods: %d\n",val);
	//	return NULL;
		}



	return d;

errX:
	if (rc<0){
		fprintf(stderr,"ERR: %s\n",snd_strerror(rc));
	}
	if (d)
		dawe_device_destroy(d);

	if (a){
		if (a->handle){
			snd_pcm_close(a->handle);
		}
		free(a);
	}
	return NULL;
}





void play_wav(dawe_device_t *d,dawe_wav_t *w)
{

	uint8_t sb[65536];
	int ws=2048;
	int div=w->bits_per_sample/8*w->channels;
	alsa_device_t *a =(alsa_device_t*)(d->data);
	int rc;


	rc = snd_pcm_prepare (a->handle);
	if (rc){
		fprintf(stderr,"Can't prepare: %d %s\n",rc, snd_strerror(rc));
		return;
	}

	dump(d);
	/*exit(0);
*/
	fseek(w->file,w->data,SEEK_SET);
	while(!feof(w->file)){
		fread(sb,1,ws,w->file);
		rc = snd_pcm_writei(a->handle, sb, ws/div);
		if (rc!=ws/div){
			fprintf(stderr,"Wrote %d instead of %d\n",rc, ws/div);
			rc = snd_pcm_prepare (a->handle);
			/*	return NULL;*/
		}
		else{
			//printf("wrote %d\n",	ws/div);
		}
		usleep(1);
	}

}

static void callback(snd_async_handler_t *ahandler)
{
	snd_pcm_state_t state;
	snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
	state = snd_pcm_state(handle);

	switch (state){
		case SND_PCM_STATE_RUNNING:
			return;
		default:
			printf("asyn call back occured %d\n",state);
		/* SND_PCM_STATE_XRUN */

	}

}



static void copy(dawe_device_t * din, dawe_device_t *dout)
{
#define S 4
	uint8_t buffer[256000];
	int rc;
	alsa_device_t *ain, *aout;
	snd_async_handler_t * ahandler;

	ain = (alsa_device_t*)(din->data);
	aout = (alsa_device_t*)(dout->data);

	snd_pcm_nonblock(ain->handle,0);

	rc = snd_pcm_prepare (ain->handle);
	if (rc){
		fprintf(stderr,"Can't prepare ain: %d %s\n",rc, snd_strerror(rc));
		return;
	}

	rc = snd_async_add_pcm_handler(&ahandler, aout->handle, callback,
								   din);

	rc = snd_async_add_pcm_handler(&ahandler, ain->handle, callback,
								   din);


	rc = snd_pcm_prepare (aout->handle);
	if (rc){
		fprintf(stderr,"Can't prepare ain: %d %s\n",rc, snd_strerror(rc));
		return;
	}


	while(1){
		rc = snd_pcm_readi(ain->handle, buffer, 64);
		/*printf("Read RC: %d\n",rc);*/
		if (rc==-11){
				continue;
		}


		if (rc<0){
printf("Read RC: %d %s\n",rc, snd_strerror(rc));
			/*	rc = snd_pcm_prepare (aout->handle); */
				rc = snd_pcm_recover(ain->handle,rc,1);
				printf("Recover RC %d\n",rc);

				continue;
		}

printf("Read %d\n",rc);
		rc = snd_pcm_writei(aout->handle, buffer, rc);
		/*printf("Write RC: %d\n",rc);*/
		if (rc<0){
			printf("prepare\n");
			rc = snd_pcm_prepare (aout->handle);
		}
	}
}


/*
 *   Transfer method - write and wait for room in buffer using poll
 */

static int wait_for_poll(snd_pcm_t *handle, struct pollfd *ufds, unsigned int count)
{
	unsigned short revents;

	while (1) {
		poll(ufds, count, -1);
		snd_pcm_poll_descriptors_revents(handle, ufds, count, &revents);
		if (revents & POLLERR)
			return -EIO;
		if (revents & POLLIN)
			return 0;
	}
}


void pollt(dawe_device_t * din, dawe_device_t *dout)
{
	uint8_t buffer[65536];
	struct pollfd *ufds;
	alsa_device_t *ain,*aout;
	int count,rc;
	ain = (alsa_device_t*)(din->data);
	aout = (alsa_device_t*)(dout->data);
	snd_pcm_sframes_t avail;

	count = snd_pcm_poll_descriptors_count (ain->handle);
	printf("Count %d\n",count);

	ufds = malloc(sizeof(struct pollfd) * count);
	if (ufds == NULL) {
		printf("No enough memory\n");
		exit(0);
	}
	rc = snd_pcm_poll_descriptors(ain->handle, ufds, count);
	if (rc<=0){
		printf("poll rc %d\n",rc);
		exit(0);
	}

	rc = snd_pcm_prepare (ain->handle);
	if (rc){
		fprintf(stderr,"Can't prepare ain: %d %s\n",rc, snd_strerror(rc));
		return;
	}


	rc = snd_pcm_readi(ain->handle, buffer, 64);
	if (rc < 0) {
		printf("Read Fail %d: %s\n",rc,snd_strerror(rc));
	}
while(1){
	wait_for_poll(ain->handle,ufds,1);
	avail = snd_pcm_avail_update(ain->handle);

	if (avail<0){
	printf("Frames avail err: %d %s\n",avail,snd_strerror(avail));
	}
	else{
		printf("Frames avail: %d\n",avail);
		rc = snd_pcm_readi(ain->handle, buffer, avail);
		if (rc < 0) {
			printf("Read Fail %d: %s",rc,snd_strerror(rc));
		}
		else{
			printf("Got %d frames\n",rc);
			usleep(10000);
		}

	}
}
	exit(0);
	if (count <= 0) {
		printf("Invalid poll descriptors count\n");
		return count;
	}


}


void play_alsa(dawe_wav_t *w)
{

	dawe_device_t *dout, *din;
	int val;
	dawe_sess_t *s;

	const char *card_in,*card_out;

	card_in = card_out = "plughw:A96";

	card_in = "hw:A96";
	card_out = "hw:A96";
/*card_in = card_out = "sysdefault";
*/
	s=dawe_sess_create(w->sample_rate,w->bits_per_sample,w->encoding);


/*	int mode = SND_PCM_STREAM_PLAYBACK; *//*| SND_PCM_STREAM_CAPTURE*/;
	dout = dawe_alsa_create_device(s,card_out,SND_PCM_STREAM_PLAYBACK,0);
	if (!dout){
		return;
	}


	printf("period out\n");
	val=2;
	dout->set_param(dout,ALSA_PARAM_PERIODS,&val);
	val=512;
	dout->set_param(dout,ALSA_PARAM_PERIOD_SIZE,&val);
	val=w->channels;
	dout->set_param(dout,ALSA_PARAM_CHANNELS,&val);
	dout->start(dout);
	val = 1;
	dout ->set_param(dout,ALSA_PARAM_START_THRESHOLD,&val);
	dump(dout);



	/*play_wav(dout,w);
	exit(0);
*/
	din = dawe_alsa_create_device(s,card_in,SND_PCM_STREAM_CAPTURE,
								 SND_PCM_NONBLOCK);

	if (!din){
		printf("No input working\n");
		return;
	}

	printf("period in\n");

	val=512;
	din->set_param(din,ALSA_PARAM_PERIODS,&val);
	val=16;
	din->set_param(din,ALSA_PARAM_PERIOD_SIZE,&val);
	val=w->channels;
	din->set_param(din,ALSA_PARAM_CHANNELS,&val);
	din->start(din);
	dump(din);

/*	val=256;
	din->set_param(din,"avail_min",&val);
*/



/*	val = 128;
	dout ->set_param(dout,ALSA_PARAM_START_THRESHOLD,&val);
*/
	pollt(din,dout);

	copy (din,dout);
	return;
	play_wav(dout,w);

}
