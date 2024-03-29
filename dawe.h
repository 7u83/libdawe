/*
 * Copyright (C) 2021  7u83 <7u83@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 */

/**
 * @file
 * @brief all definitions for libdawe
 */


#ifndef _DAWE_H_
#define _DAWE_H_

#include "sysdefs.h"
#include "dawe_tpool.h"



/**
 * @brief The dawe_formats enum
 */
typedef enum dawe_formats {
	DAWE_FMT_PCM=1,			/**< PCM */
	DAWE_FMT_FLOAT=2,		/**< Float */
	DAWE_FMT_INTERLEAVED=0,		/**< Interleaved */
	DAWE_FMT_NON_INTERLEAVED=4	/**< Non-interleaved */
} dawe_format_t;


#define DAWE_SESS_MAX_THREADS 1

/**
 * @brief The dawe_sess struct
 */
struct dawe_sess {
	uint32_t sampling_rate;		/**< the overall one and only
					     sampling rate */
	uint16_t bits_per_sample;	/**< Bits per sample */
	int format;			/**< Contains the format, can
					     contain one of #dawe_format */
	dawe_tpool_t * tpool;		/**< a thread pool (#dawe_tpool)
					     used internally by libdawe */

};
typedef struct dawe_sess dawe_sess_t;
dawe_sess_t * dawe_sess_create(
			uint32_t sampling_rate,
			uint16_t bits_per_sample,
			int format,
			uint32_t max_threads
		);
void dawe_sess_destroy(dawe_sess_t * s);

#define DAWE_DEVICE_ALSA 1
#define DAWE_DEVICE_JACK 2

struct dawe_device_param{
	const char * name;
};


typedef struct dawe_bus{
	int channels;
	int frames;
	int step;
	uint8_t **data;
} dawe_bus_t;


/**
 * @brief The dawe_device struct
 */
typedef struct dawe_device{
	int type;
	int dir;
	uint32_t latency;
	int (*set_param)( struct dawe_device * d, const char *name, void *val);
	int (*start)(struct dawe_device *d);

	void * data;		/**< private, driver specific data*/
	dawe_bus_t ** inputs;
	dawe_bus_t ** outputs;
} dawe_device_t;




typedef enum dawe_x{
	DAWE_INT
} dawe_x_t;

dawe_device_t * dawe_device_create();

void dawe_device_destroy(dawe_device_t * d);

#define frames_to_ms(sampling_rate,frames) \
	(((float)(frames)/(float)(sampling_rate))*(float)(1000))


#endif
