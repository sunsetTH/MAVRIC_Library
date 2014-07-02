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
 * \file coord_conventions.c
 * 
 * Coordinate conventions
 */ 


#include "coord_conventions.h"
#include <math.h>
#include "maths.h"
#include "print_util.h"
#include "conf_platform.h"
#include "quick_trig.h"


global_position_t local_to_global_position(local_coordinates_t input)
{
	global_position_t output;
	output.latitude = input.origin.latitude  + rad_to_deg( input.pos[0] / EARTH_RADIUS);
	output.longitude= input.origin.longitude + rad_to_deg( input.pos[1] / ( EARTH_RADIUS * cos(deg_to_rad(output.latitude)) ) );
	output.altitude = -input.pos[2] + input.origin.altitude;
	output.heading=input.heading;

	return output;
}


local_coordinates_t global_to_local_position(global_position_t position, global_position_t origin) 
{
	local_coordinates_t output;
	output.origin = origin;
	double small_radius = cos(deg_to_rad(position.latitude)) * EARTH_RADIUS;
	output.pos[X] = (float)(sin(deg_to_rad((position.latitude-origin.latitude))) * EARTH_RADIUS);
	output.pos[Y] = (float)(sin(deg_to_rad((position.longitude-origin.longitude))) * small_radius);
	output.pos[Z] = (float)(-(position.altitude - origin.altitude));
	output.heading = position.heading;
	
	return output;
}


Aero_Attitude_t Quat_to_Aero(UQuat_t qe) 
{
	Aero_Attitude_t aero;

	aero.rpy[0] = atan2(2*(qe.s*qe.v[0] + qe.v[1]*qe.v[2]) , (qe.s*qe.s - qe.v[0]*qe.v[0] - qe.v[1]*qe.v[1] + qe.v[2]*qe.v[2])); 
	aero.rpy[1] = -asin(2*(qe.v[0]*qe.v[2] - qe.s*qe.v[1]));
	aero.rpy[2] = atan2(2*(qe.s*qe.v[2] + qe.v[0]*qe.v[1]) , (qe.s*qe.s + qe.v[0]*qe.v[0] - qe.v[1]*qe.v[1] - qe.v[2]*qe.v[2]));
	
	return aero;
}


UQuat_t quaternion_from_aero(Aero_Attitude_t aero)
{
	UQuat_t quat;

	// intermediate values
	float cr, cp, cy, sr, sp, sy;
	cr = quick_cos(aero.rpy[0] / 2);
	cp = quick_cos(aero.rpy[1] / 2);
	cy = quick_cos(aero.rpy[2] / 2);
	sr = quick_sin(aero.rpy[0] / 2);
	sp = quick_sin(aero.rpy[1] / 2);
	sy = quick_sin(aero.rpy[2] / 2);


	quat.s = 	(cr * cp * cy) + (sr * sp * sy);
	quat.v[0] = (sr * cp * cy) - (cr * sp * sy);
	quat.v[1] = (cr * sp * cy) + (sr * cp * sy);
	quat.v[2] = (cr * cp * sy) - (sr * sp * cy);

	return quat;
}


float get_yaw(UQuat_t qe) 
{
	return  atan2(2*(qe.s*qe.v[2] + qe.v[0]*qe.v[1]) , (qe.s*qe.s + qe.v[0]*qe.v[0] - qe.v[1]*qe.v[1] - qe.v[2]*qe.v[2]));
}
