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
 * \file servos_mix_adaptive_morph.h
 * 
 * \author MAV'RIC Team
 * \author Simon Pyroth
 *   
 * \brief Links between regulation output and PWM commands for a adaptive_morph aircraft
 *
 ******************************************************************************/


#ifndef SERVOS_MIX_adaptive_morph_H_
#define SERVOS_MIX_adaptive_morph_H_

#ifdef __cplusplus
	extern "C" {
#endif


#include "control_command.h"
#include "servos.h"
#include "stabilisation.h"
#include "remote.h"



/**
 * \brief Enumerate the direction of the servos for a positiv input (ex: if positiv input to servo, does it go up ?=
 */
typedef enum
{
	UP2 		= 1,					///< Aileron going up for positiv input
	DOWN2		= -1					///< Aileron going down for positiv input
} adaptive_morph_servo_dir_t;


/**
 * \brief	debug adaptive morph
 */
typedef struct 
{	
	int32_t	is_pitch_control;			///< Check if pitch control or Roll control
	int32_t	is_single_folding;			///check if it should fold only one wing side
} debug_adaptive_morph_t;


/**
 * \brief The servo mix structure for a adaptive_morph
 */
typedef struct
{
	uint8_t				servo_elevator;		///< Right aileron servo index
	uint8_t				servo_wing_left;	///< Left aileron servo index
	uint8_t				servo_wing_right;	///< Left aileron servo index
	uint8_t				servo_tail;			///< Left aileron servo index
	uint8_t				motor;				///< Propulsion motor index
	
	adaptive_morph_servo_dir_t	servo_elevator_dir;	///< Right aileron servo direction
	adaptive_morph_servo_dir_t	servo_wing_left_dir;		///< Left aileron servo direction
	adaptive_morph_servo_dir_t	servo_wing_right_dir;		///< Left aileron servo direction
	adaptive_morph_servo_dir_t	servo_tail_dir;		///< Left aileron servo direction
	
	float				min_amplitude;		///< Minimum value which can be put on servo
	float				max_amplitude;		///< Maximum value which can be put on servo
	float				min_thrust;			///< Minimum value which can be put on the motor
	float				max_thrust;			///< Maximum value which can be put on the motor
	
	float				trim_pitch;			///< Trim value for pitch
	float				trim_roll_left;		///< Trim value for roll
	float				trim_roll_right;	///< Trim value for roll
	float				trim_tail;		///< Trim value for roll
	
	debug_adaptive_morph_t debug;
} servo_mix_adaptive_morph_conf_t;


/**
 * \brief	Servos mix structure
 */
typedef struct 
{	
	servo_mix_adaptive_morph_conf_t	config;			///< Configuration of the mix
	debug_adaptive_morph_t debug;
	control_command_t*		command;				///< Pointer to the control command structure
	servos_t*          		servos;					///< Pointer to the servos structure
	remote_t*				remote;					///< Pointer to the remote structure
} servo_mix_adaptive_morph_t;


/**
 * \brief			Initialize the servo mix
 * 
 * \param mix		Pointer to the servo mix structure of the adaptive_morph
 * \param config	Pointer to the configuration of servo mix structure
 * \param command	Pointer to the command
 * \param servos	Pointer to the servos structure
 *
 * \return	True if the init succeed, false otherwise
 */
bool servo_mix_adaptive_morph_init(servo_mix_adaptive_morph_t* mix, const servo_mix_adaptive_morph_conf_t* config, control_command_t* command, servos_t* servos, remote_t* remote);


/**
 * \brief			Update the servos mix
 * 
 * \param mix		Pointer to the servos mix structure
 */
void servos_mix_adaptive_morph_update(servo_mix_adaptive_morph_t* mix);

/**
 * \brief			Update the servos mix. This function adds possibility to use another command than the one used at initialisation (for example for a full manual acces)
 * 
 * \param mix		Pointer to the servos mix structure
 * \param command	Pointer to the command
 */
void servos_mix_adaptive_morph_update_command(servo_mix_adaptive_morph_t* mix, control_command_t* command);


#ifdef __cplusplus
	}
#endif

#endif