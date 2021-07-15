
#ifndef _DAWE_WAV_H
#define _DAWE_WAV_H

#include <stdio.h>

struct dawe_wav{
	FILE * file;
};
typedef struct dawe_wav dawe_wav_t;

extern void dawe_wav_close(dawe_wav_t *w);
extern dawe_wav_t * dawe_wav_open(const char *filename);

#endif
