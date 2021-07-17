#include <stdio.h>
#include <errno.h>
#include <string.h>


#include "dawe_wav.h"
#include "dawe_err.h"

void play(dawe_wav_t *wav)
{
	extern void play_alsa(dawe_wav_t*wav);
	play_alsa(wav);
}

int main(int argc, char ** argv)
{
	dawe_wav_t *wav;

	if (argc<2)
		return 1;

	wav = dawe_wav_open(argv[1]);
	if (!wav){
		fprintf(stderr,"Can't open %s: %s\n",argv[1],
				dawe_strerror(errno));
		return -1;
	}

	dawe_wav_print(wav);

	play(wav);

	dawe_wav_close(wav);

	return 0;
}
