#include <stdlib.h>

#include "dawe.h"

dawe_sess_t * dawe_sess_create(uint16_t sampling_rate,
							   uint16_t bits_per_sample,
							   int format)
{
	dawe_sess_t * s;
	s = malloc(sizeof(dawe_sess_t));
	if (s==NULL){
		return NULL;
	}
	s->sampling_rate=sampling_rate;
	s->bits_per_sample=bits_per_sample;
	s->format=format;
	return s;
}

