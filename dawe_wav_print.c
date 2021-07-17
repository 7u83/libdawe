#include <stdio.h>

#include "dawe_wav.h"

void dawe_wav_print(dawe_wav_t *w)
{
	printf("WAV FILE INFO\n");
	printf("=============\n");
	printf("FILE: %s\n",w->filename);
	printf("SAMPLE RATE: %d HZ\n",w->sample_rate);
	printf("BITS PER SAMPLE: %d\n",w->bits_per_sample);
	printf("BYTES PER SAMPLE: %d\n",w->bytes_per_second);
	printf("CHANNELS: %d\n",w->channels);
	printf("ENCODING: 0x%04X\n",w->encoding);
	printf("VALID BPS: %d\n",w->valid_bits_per_sample);
	printf("CHANNEL MASK: %08x\n",w->channel_mask);
	printf("DATA BEGINS AT: %ld\n",w->data);
	printf("DATA LEN: %d\n",w->data_len);
}
