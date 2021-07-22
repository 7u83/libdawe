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
dawe_sess_t * dawe_sess_create(uint16_t sampling_rate,
							   uint16_t bits_per_sample,
							   int format);

#define DAWE_DEVICE_ALSA 1
#define DAWE_DEVICE_JACK 2

struct dawe_device_param{
	const char * name;

};

/**
 * @brief The dawe_device struct
 */
struct dawe_device{
	int type;
	int dir;
	uint32_t latency;
	int (*set_param)( struct dawe_device * d, const char *name, void *val);
	int (*start)(struct dawe_device *d);

	void * data;		/** private, driver specific data*/
};

typedef struct dawe_device dawe_device_t;
dawe_device_t * dawe_device_create();
void dawe_device_destroy(dawe_device_t * d);

#define frames_to_ms(sampling_rate,frames) \
	(((float)(frames)/(float)(sampling_rate))*(float)(1000))


#endif
