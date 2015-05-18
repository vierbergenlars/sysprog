#ifndef _DATAMGR_H
#define _DATAMGR_H
#include <stdio.h>
#include "../util/list.h"
#include "sensor.h"

/**
 * Creates a new sensor list
 * @return list_t*|NULL
 */
list_t* sensor_list_create();
/**
 * Creates a new sensor list and adds sensors from a mapping file
 * @return list_t*|NULL
 */
list_t* sensor_list_from_file(FILE*);
/**
 * Locates the sensor that the received packet is from
 * @return sensor_data*|NULL
 */
sensor_data* sensor_list_find_sensor(list_t*, sensor_wire_data*);
#endif
