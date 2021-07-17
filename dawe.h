#ifndef _DAWE_H_
#define _DAWE_H_

#include "sysdefs.h"

struct dawe_terminal
{
	int x;
};

struct dawe_bus {
	char * name;
	int channels;
	int latency;
};


#define DAWE_FMT_PCM	1
#define DAWE_FMT_FLOAT	2


/**
 * @brief The dawe_sess struct
 */
struct dawe_sess {
	uint16_t sampling_rate;		/** the overall one and only sampling rate */
	uint16_t bits_per_sample;	/** */
	int format;
};
typedef struct dawe_sess dawe_sess_t;

/**
 * @brief The dawe_device struct
 */
struct dawe_device{
	uint32_t latency;
	void * data;		/** private, driver specific data*/
};
typedef struct dawe_device dawe_device_t;
dawe_device_t * dawe_device_create();

#define frames_to_ms(sampling_rate,frames) \
	(((float)(frames)/(float)(sampling_rate))*(float)(1000))


#endif
