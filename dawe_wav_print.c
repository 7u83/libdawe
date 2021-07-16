#include <stdio.h>

#include "dawe_wav.h"

void dawe_wav_print(dawe_wav_t *w)
{
	printf("WAV FILE INFO\n");
	printf("=============\n");
	printf("FILE: %s\n",w->filename);
	printf("SAMPLE RATE: %d KHZ\n",w->sample_rate);
	printf("BITS PER SAMPLE: %d\n",w->bits_per_sample);
	printf("CHANNELS: %d\n",w->channels);
	printf("ENCODING: 0x%04X\n",w->encoding);
}
