#include <string.h>
#include <stdlib.h>

#include "dawe.h"

/**
 * @brief Create a new empty device
 * @return
 */
dawe_device_t * dawe_device_create()
{
	dawe_device_t *d = malloc(sizeof(dawe_device_t));
	if (!d)
		return NULL;
	memset(d,0,sizeof(dawe_device_t));
	return d;
}


void dawe_device_destroy(dawe_device_t * d)
{
	free(d);
}
