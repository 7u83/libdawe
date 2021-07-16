
#ifndef _DAWE_WAV_H
#define _DAWE_WAV_H

#include <stdio.h>

#include "sysdefs.h"

struct dawe_wav{
	FILE * file;
	char * filename;

	uint32_t sample_rate;
	uint32_t bytes_per_second;
	uint16_t encoding;
	uint16_t channels;
	uint16_t block_align;
	uint16_t bits_per_sample;

	long data;
	uint32_t data_len;
};
typedef struct dawe_wav dawe_wav_t;

extern void dawe_wav_close(dawe_wav_t *w);
extern dawe_wav_t * dawe_wav_open(const char *filename);
extern void dawe_wav_print(dawe_wav_t *w);

#endif
