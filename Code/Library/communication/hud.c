/*******************************************************************************
 * Copyright (c) 2009-2014, MAV'RIC Development Team
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 * \file hud.c
 * 
 * \author MAV'RIC Team
 * \author Gregoire Heitz
 *   
 * \brief This file sends the mavlink HUD message
 *
 ******************************************************************************/


#include "hud.h"
#include "print_util.h"
#include "coord_conventions.h"
#include "mavlink_communication.h"

void hud_init(hud_structure_t* hud_structure, const position_estimator_t* pos_est, const control_command_t* controls, const ahrs_t* ahrs, const mavlink_stream_t* mavlink_stream)
{
	hud_structure->ahrs = ahrs;
	hud_structure->controls            = controls;
	hud_structure->pos_est             = pos_est;
	hud_structure->mavlink_stream      = mavlink_stream;
	
	print_util_dbg_print("HUD structure initialised.\r\n");
}

task_return_t hud_send_message(hud_structure_t* hud_structure) 
{
	float groundspeed = sqrt(hud_structure->pos_est->vel[0] * hud_structure->pos_est->vel[0] + hud_structure->pos_est->vel[1] * hud_structure->pos_est->vel[1]);
	float airspeed=groundspeed;

	aero_attitude_t aero_attitude;
	aero_attitude = coord_conventions_quat_to_aero(hud_structure->ahrs->qe);
	
	int16_t heading;
	if(aero_attitude.rpy[2] < 0)
	{
		heading = (int16_t)(360.0f + 180.0f * aero_attitude.rpy[2] / PI); //you want to normalize between 0 and 360°
	}
	else
	{
		heading = (int16_t)(180.0f * aero_attitude.rpy[2] / PI);
	}
	
	
	mavlink_message_t msg;	
	mavlink_msg_vfr_hud_pack(	hud_structure->mavlink_stream->sysid, 
								hud_structure->mavlink_stream->sysid,
								&msg,
								airspeed, 
								groundspeed, 
								heading, 
								(int32_t)((hud_structure->controls->thrust + 1.0f) * 50), 
								-hud_structure->pos_est->local_position.pos[2] + hud_structure->pos_est->local_position.origin.altitude, 
								-hud_structure->pos_est->vel[2]	);
	mavlink_stream_send(hud_structure->mavlink_stream, &msg);

	return TASK_RUN_SUCCESS;
}