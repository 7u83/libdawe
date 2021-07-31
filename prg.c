#include <stdio.h>
#include <errno.h>
#include <string.h>


#include "dawe_wav.h"
#include "dawe_err.h"
#include "dawe_tpool.h"


void job(void *arg)
{
	int i = (int)arg;
	sleep(1);
	printf("HELLO WORLD %d\n",i);
}

void play(dawe_wav_t *wav)
{
	extern void play_alsa(dawe_wav_t*wav);
	play_alsa(wav);
}
#include <pthread.h>
int main(int argc, char ** argv)
{
	dawe_wav_t *wav;
	int ret;
	pthread_t this_thread;

	if (argc<2)
		return 1;

	wav = dawe_wav_open(argv[1]);
	if (!wav){
		fprintf(stderr,"Can't open %s: %s\n",argv[1],
				dawe_strerror(errno));
		return -1;
	}

	dawe_wav_print(wav);

	this_thread = pthread_self();

	/* struct sched_param is used to store the scheduling priority */
   struct sched_param params;
   params.sched_priority = sched_get_priority_max(SCHED_FIFO);

   ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);
   if (ret != 0) {

	   printf("Unsuccessful in setting thread realtime prio\n");
/*	   return;*/
   }

	{
		int i;
		dawe_tpool_t * p;
		p=dawe_tpool_create(215);
		printf("tppool created\n");
		for (i=0;i<50;i++){
			dawe_tpool_add_job(p,job,(void *)i);
			printf("job added %d\n",i);

		}



   }


	play(wav);

	dawe_wav_close(wav);

	return 0;
}
