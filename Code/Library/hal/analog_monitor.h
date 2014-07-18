/**
 * \page The MAV'RIC License
 *
 * The MAV'RIC Framework
 *
 * Copyright © 2011-2014
 *
 * Laboratory of Intelligent Systems, EPFL
 */


/**
 * \file analog_monitor.h
 *
 * The driver for the analog monitor module
 */ 


#ifndef ANALOG_MONITOR_H_
#define ANALOG_MONITOR_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "scheduler.h"

#define MONITOR_CHANNELS 10
#define MONITOR_SAMPLES  10

typedef enum
{
	ANALOG_RAIL_2,		// ANA connector, pin1
	ANALOG_RAIL_3,		// ANA connector, pin2
	ANALOG_RAIL_4,		// ANA connector, pin3
	ANALOG_RAIL_5,		// ANA connector, pin4
	ANALOG_RAIL_6,		// 6V
	ANALOG_RAIL_7,		// 5V_ANA
	ANALOG_RAIL_10,		// BATTERY_FILTERED
	ANALOG_RAIL_11,		// BATTERY
	ANALOG_RAIL_12,		// P8 connector, pin 1
	ANALOG_RAIL_13  	// P9 connector, pin 1
} analog_rails_t;

typedef struct 
{
	bool enable[MONITOR_CHANNELS];
	int16_t buffer[MONITOR_CHANNELS][MONITOR_SAMPLES];
	float avg[MONITOR_CHANNELS];
} analog_monitor_t;

/**
 * \brief	Initialisation of the analog monitor
 * 
 * \param	analog_monitor	The pointer to the analog monitor structure
 */
void analog_monitor_init(analog_monitor_t* analog_monitor);


/**
 * \brief	Update of the analog monitor
 * 
 * \param	analog_monitor	The pointer to the analog monitor structure
 */
task_return_t analog_monitor_update(analog_monitor_t* analog_monitor);

#ifdef __cplusplus
	}
#endif

#endif /* ANALOG_MONITOR_H_ */