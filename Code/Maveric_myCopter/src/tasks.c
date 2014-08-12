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
 * \file tasks.c
 *
 * Definition of the tasks executed on the autopilot
 */ 


#include "tasks.h"
#include "central_data.h"
#include "print_util.h"
#include "stabilisation.h"
#include "gps_ublox.h"
#include "navigation.h"
#include "led.h"
#include "imu.h"
#include "orca.h"
#include "delay.h"
#include "i2cxl_sonar.h"
#include "analog_monitor.h"
#include "lsm330dlc.h"
#include "hmc5883l.h"

#include "pwm_servos.h"


#include "remote_controller.h"
#include "remote.h"
#include "attitude_controller_p2.h"

central_data_t* central_data;

/**
 * \brief	Function to call when the motors should be switched off
 */
void switch_off_motors(void);

task_set_t* tasks_get_main_taskset() 
{
	central_data = central_data_get_pointer_to_struct();

	return central_data->scheduler.task_set;
}

void tasks_rc_user_channels(uint8_t *chan_switch, signal_quality_t* rc_check, int8_t *motor_state)
{
	
	remote_controller_get_channel_mode(chan_switch);
	
	// if ((spektrum_satellite_get_neutral(RC_TRIM_P3) * RC_SCALEFACTOR) > 0.0f)
	if ( central_data->remote.channels[CHANNEL_AUX1] > 0.0f )
	{
		central_data->navigation.collision_avoidance = true;
	}
	else
	{
		central_data->navigation.collision_avoidance = false;
	}
	
	remote_controller_get_motor_state(motor_state);
	
	*rc_check = remote_check( &central_data->remote );
}

void switch_off_motors(void)
{
	print_util_dbg_print("Switching off motors!\n");

	central_data->state.mav_state = MAV_STATE_STANDBY;
	state_disable_mode(&central_data->state, MAV_MODE_FLAG_SAFETY_ARMED);
	state_set_new_mode(&central_data->state, MAV_MODE_SAFE);
	
	central_data->waypoint_handler.in_the_air = false;
}

task_return_t tasks_set_mav_mode_n_state(void* arg)
{
	uint8_t channel_switches = 0;
	signal_quality_t RC_check = SIGNAL_GOOD;
	int8_t motor_switch = 0;
	
	float dist_from_home_sqr;
	
	LED_Toggle(LED1);
	// LED_Toggle(LED2);
	
	tasks_rc_user_channels(&channel_switches,&RC_check, &motor_switch);
	
	switch(central_data->state.mav_state)
	{
		case MAV_STATE_BOOT:
			break;

		case MAV_STATE_CALIBRATING:
			break;

		case MAV_STATE_STANDBY:
			if (motor_switch == 1)
			{
				switch(channel_switches)
				{
					case 0:
						print_util_dbg_print("Switching on the motors!\n");

						position_estimation_reset_home_altitude(&central_data->position_estimator);
						
						central_data->waypoint_handler.waypoint_set = false;
						state_set_new_mode(&central_data->state,MAV_MODE_ATTITUDE_CONTROL);
						break;

					default:
						print_util_dbg_print("Switches not ready, both should be pushed!\n");
						break;
				}
			}
			if (state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_SAFETY_ARMED))
			{
				switch (channel_switches)
				{
					case 0:
						state_set_new_mode(&central_data->state,MAV_MODE_ATTITUDE_CONTROL);
						if (central_data->waypoint_handler.in_the_air)
						{
							central_data->state.mav_state = MAV_STATE_ACTIVE;
						}
						break;

					case 1:
						state_set_new_mode(&central_data->state,MAV_MODE_VELOCITY_CONTROL);
						if (central_data->waypoint_handler.in_the_air)
						{
							central_data->state.mav_state = MAV_STATE_ACTIVE;
						}
						break;

					case 2:
						if (central_data->waypoint_handler.in_the_air)
						{
							state_set_new_mode(&central_data->state,MAV_MODE_POSITION_HOLD);
							
							// Activate automatic take-off mode
							if (state_test_if_first_time_in_mode(&central_data->state) )
							{
								waypoint_handler_waypoint_take_off(&central_data->waypoint_handler);
							}
							
							dist_from_home_sqr =	SQR(central_data->position_estimator.local_position.pos[X] - central_data->waypoint_handler.waypoint_hold_coordinates.pos[X]) +
							SQR(central_data->position_estimator.local_position.pos[Y] - central_data->waypoint_handler.waypoint_hold_coordinates.pos[Y]) +
							SQR(central_data->position_estimator.local_position.pos[Z] - central_data->waypoint_handler.waypoint_hold_coordinates.pos[Z]);
						
							if (central_data->waypoint_handler.dist2wp_sqr <= 16.0f)
							{
								central_data->state.mav_state = MAV_STATE_ACTIVE;
								print_util_dbg_print("Automatic take-off finised, dist_from_home_sqr (10x):");
								print_util_dbg_print_num(dist_from_home_sqr * 10.0f,10);
								print_util_dbg_print(".\n");
							}
						}
						break;

					case 3:
						if (central_data->waypoint_handler.in_the_air)
						{
							//central_data->state.mav_state = MAV_STATE_ACTIVE;
							state_set_new_mode(&central_data->state,MAV_MODE_GPS_NAVIGATION);
							
							// Automatic take-off mode
							if(state_test_if_first_time_in_mode(&central_data->state) )
							{
								waypoint_handler_waypoint_take_off(&central_data->waypoint_handler);
							}

							if (!central_data->waypoint_handler.waypoint_set)
							{
								waypoint_handler_waypoint_init(&central_data->waypoint_handler);
							}

							dist_from_home_sqr =	SQR(central_data->position_estimator.local_position.pos[X] - central_data->waypoint_handler.waypoint_hold_coordinates.pos[X]) +
							SQR(central_data->position_estimator.local_position.pos[Y] - central_data->waypoint_handler.waypoint_hold_coordinates.pos[Y]) +
							SQR(central_data->position_estimator.local_position.pos[Z] - central_data->waypoint_handler.waypoint_hold_coordinates.pos[Z]);
						
							if (central_data->waypoint_handler.dist2wp_sqr <= 16.0f)
							{
								central_data->state.mav_state = MAV_STATE_ACTIVE;
								print_util_dbg_print("Automatic take-off finised, dist_from_home_sqr (10x):");
								print_util_dbg_print_num(dist_from_home_sqr * 10.0f,10);
								print_util_dbg_print(".\n");
							}
						}
						break;
				}
				
				switch (RC_check)
				{
					case SIGNAL_GOOD:
						break;

					case SIGNAL_BAD:
						central_data->state.mav_state = MAV_STATE_CRITICAL;
						break;

					case SIGNAL_LOST:
						central_data->state.mav_state = MAV_STATE_CRITICAL;
						break;
				}
				if (remote_get_throttle( &central_data->remote ) > -0.7f)
				{
					central_data->waypoint_handler.in_the_air = true;
				}
			}

			if (motor_switch == -1)
			{
				switch_off_motors();
			}
			
			break;

		case MAV_STATE_ACTIVE:
			switch(channel_switches)
			{
				case 0:
					state_set_new_mode(&central_data->state,MAV_MODE_ATTITUDE_CONTROL);
					break;

				case 1:
					state_set_new_mode(&central_data->state,MAV_MODE_VELOCITY_CONTROL);
					break;

				case 2:
					state_set_new_mode(&central_data->state,MAV_MODE_POSITION_HOLD);
					if (state_test_if_first_time_in_mode(&central_data->state) )
					{
						waypoint_handler_waypoint_hold_init(&central_data->waypoint_handler,central_data->position_estimator.local_position);
					}
					break;

				case 3:
					state_set_new_mode(&central_data->state,MAV_MODE_GPS_NAVIGATION);
					if (state_test_if_first_time_in_mode(&central_data->state) )
					{
						central_data->waypoint_handler.auto_landing_behavior = DESCENT_TO_SMALL_ALTITUDE;
						waypoint_handler_waypoint_hold_init(&central_data->waypoint_handler,central_data->position_estimator.local_position);
					}
					break;
			}
			
			if (state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_AUTO_ENABLED))
			{
				if (!central_data->waypoint_handler.waypoint_set)
				{
					waypoint_handler_waypoint_init(&central_data->waypoint_handler);
				}

				waypoint_handler_waypoint_navigation_handler(&central_data->waypoint_handler);
			}
			
			if (motor_switch == -1)
			{
				switch_off_motors();
			}
		
			switch (RC_check)
			{
				case SIGNAL_GOOD:
					break;

				case SIGNAL_BAD:
					central_data->state.mav_state = MAV_STATE_CRITICAL;
					break;

				case SIGNAL_LOST:
					central_data->state.mav_state = MAV_STATE_CRITICAL;
					break;
			}
			break;

		case MAV_STATE_CRITICAL:
			switch(channel_switches)
			{
				case 0:
					state_set_new_mode(&central_data->state,MAV_MODE_ATTITUDE_CONTROL);
					break;

				case 1:
					state_set_new_mode(&central_data->state,MAV_MODE_VELOCITY_CONTROL);
					break;

				case 2:
					state_set_new_mode(&central_data->state,MAV_MODE_POSITION_HOLD);
					break;

				case 3:
					state_set_new_mode(&central_data->state,MAV_MODE_GPS_NAVIGATION);
					break;
			}

			if (motor_switch == -1)
			{
				switch_off_motors();
			}
			
			// In MAV_MODE_VELOCITY_CONTROL, MAV_MODE_POSITION_HOLD and MAV_MODE_GPS_NAVIGATION
			if (state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_STABILIZE_ENABLED))
			{
				waypoint_handler_waypoint_critical_handler(&central_data->waypoint_handler);
			}
			
			switch (RC_check)
			{
				case SIGNAL_GOOD:  
					// !! only if receivers are back, switch into appropriate mode
					central_data->state.mav_state = MAV_STATE_ACTIVE;
					central_data->waypoint_handler.critical_behavior = CLIMB_TO_SAFE_ALT;
					central_data->waypoint_handler.critical_next_state = false;
					break;

				case SIGNAL_BAD:
					break;

				case SIGNAL_LOST:
					if (central_data->waypoint_handler.critical_landing)
					{
						central_data->state.mav_state = MAV_STATE_EMERGENCY;
					}

					break;
			}
			break;

		case MAV_STATE_EMERGENCY:
			state_set_new_mode(&central_data->state,MAV_MODE_SAFE);
			switch (RC_check)
			{
				case SIGNAL_GOOD:
					central_data->state.mav_state = MAV_STATE_STANDBY;
					break;

				case SIGNAL_BAD:
					break;

				case SIGNAL_LOST:
					break;
			}
			break;
	}
	
	central_data->state.mav_mode_previous = central_data->state.mav_mode;
	
	// if (central_data->state.simulation_mode_previous != central_data->state.simulation_mode)
	// {
	// 	simulation_switch_between_reality_n_simulation(&central_data->sim_model);
	// 	central_data->state.simulation_mode_previous = central_data->state.simulation_mode;
	// }
	
	return TASK_RUN_SUCCESS;
}


void tasks_run_imu_update(void* arg)
{
	if (state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_HIL_ENABLED))
	{
		simulation_update(&central_data->sim_model);
	} 
	else 
	{
		lsm330dlc_gyro_update(&(central_data->imu.raw_gyro));
		lsm330dlc_acc_update(&(central_data->imu.raw_accelero));
		hmc5883l_update(&(central_data->imu.raw_compass));
	}
	
	imu_update(	&central_data->imu);
	qfilter_update(&central_data->attitude_filter);
	
	if (central_data->imu.calibration_level == OFF)
	{
		position_estimation_update(&central_data->position_estimator);
	}
}


task_return_t tasks_run_stabilisation(void* arg) 
{
	tasks_run_imu_update(0);
	
	switch(central_data->state.mav_mode.byte - (central_data->state.mav_mode.byte & MAV_MODE_FLAG_DECODE_POSITION_HIL))
	{
		case MAV_MODE_ATTITUDE_CONTROL:
			remote_controller_get_command_from_remote(&central_data->controls);
			central_data->controls.control_mode = ATTITUDE_COMMAND_MODE;
			central_data->controls.yaw_mode=YAW_RELATIVE;
		
			stabilisation_copter_cascade_stabilise(&central_data->stabilisation_copter);
			break;
		
		case MAV_MODE_VELOCITY_CONTROL:
			remote_controller_get_velocity_vector_from_remote(&central_data->controls);
			
			central_data->controls.control_mode = VELOCITY_COMMAND_MODE;
			central_data->controls.yaw_mode = YAW_RELATIVE;
			
			stabilisation_copter_cascade_stabilise(&central_data->stabilisation_copter);
		
			break;
		
		case MAV_MODE_POSITION_HOLD:
			central_data->controls = central_data->controls_nav;
			central_data->controls.control_mode = VELOCITY_COMMAND_MODE;
		
			if ((central_data->state.mav_state == MAV_STATE_CRITICAL) && (central_data->waypoint_handler.critical_behavior == FLY_TO_HOME_WP))
			{
				central_data->controls.yaw_mode = YAW_COORDINATED;
			}
			else
			{
				central_data->controls.yaw_mode = YAW_ABSOLUTE;
			}
		
			stabilisation_copter_cascade_stabilise(&central_data->stabilisation_copter);
		
			break;
		
		case MAV_MODE_GPS_NAVIGATION:
			central_data->controls = central_data->controls_nav;
			central_data->controls.control_mode = VELOCITY_COMMAND_MODE;
			
			// if no waypoints are set, we do position hold therefore the yaw mode is absolute
			if (((central_data->waypoint_handler.waypoint_set&&(central_data->state.mav_state != MAV_STATE_STANDBY)))||((central_data->state.mav_state == MAV_STATE_CRITICAL)&&(central_data->waypoint_handler.critical_behavior == FLY_TO_HOME_WP)))
			{
				central_data->controls.yaw_mode = YAW_COORDINATED;
			}
			else
			{
				central_data->controls.yaw_mode = YAW_ABSOLUTE;
			}
		
			stabilisation_copter_cascade_stabilise(&central_data->stabilisation_copter);
			break;
		
		default:
			servos_set_value_failsafe( &central_data->servos );
			break;
	}
	
	// !!! -- for safety, this should remain the only place where values are written to the servo outputs! --- !!!
	if (!state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_HIL_ENABLED))
	{
		pwm_servos_write_to_hardware( &central_data->servos );
	}
	
	return TASK_RUN_SUCCESS;
}

// new task to test P^2 attutude controller
task_return_t tasks_run_stabilisation_quaternion(void* arg);
task_return_t tasks_run_stabilisation_quaternion(void* arg)
{
	tasks_run_imu_update(0);
	
	switch(central_data->state.mav_mode.byte - (central_data->state.mav_mode.byte & MAV_MODE_FLAG_DECODE_POSITION_HIL))
	{
		case MAV_MODE_ATTITUDE_CONTROL:
		case MAV_MODE_VELOCITY_CONTROL:
		case MAV_MODE_POSITION_HOLD:
		case MAV_MODE_GPS_NAVIGATION:
			remote_controller_get_command_from_remote(&central_data->controls);
			
			central_data->command.attitude.rpy[0] 	= 2 * central_data->controls.rpy[0];
			central_data->command.attitude.rpy[1] 	= 2 * central_data->controls.rpy[1];
			central_data->command.attitude.rpy[2] 	= 2 * central_data->controls.rpy[2];
			central_data->command.attitude.mode 	= ATTITUDE_COMMAND_MODE_RPY;
			central_data->command.thrust.thrust 	= central_data->controls.thrust;
		
			attitude_controller_p2_update( &central_data->attitude_controller );			
			servos_mix_quadcopter_diag_update( &central_data->servo_mix );
			break;
		
		default:
			servos_set_value_failsafe( &central_data->servos );
			break;
	}

	// !!! -- for safety, this should remain the only place where values are written to the servo outputs! --- !!!
	if (!state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_HIL_ENABLED))
	{
		pwm_servos_write_to_hardware( &central_data->servos );
	}
	
	return TASK_RUN_SUCCESS;
} 



task_return_t tasks_run_gps_update(void* arg) 
{
	if (state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_HIL_ENABLED))
	{
		simulation_simulate_gps(&central_data->sim_model);
	} 
	else 
	{
		gps_ublox_update(&central_data->gps);
	}
	
	return TASK_RUN_SUCCESS;
}


task_return_t tasks_run_barometer_update(void* arg)
{
	if (state_test_if_in_flag_mode(&central_data->state,MAV_MODE_FLAG_HIL_ENABLED))
	{
		simulation_simulate_barometer(&central_data->sim_model);
	} 
	else
	{
		bmp085_update(&(central_data->pressure));
	}

	return TASK_RUN_SUCCESS;
}


//task_return_t sonar_update(void* arg)
//{
	// TODO: add the simulation sonar task
	//central_data_t* central_data = central_data_get_pointer_to_struct();
	//i2cxl_sonar_update(&central_data->i2cxl_sonar);
	//
	//return TASK_RUN_SUCCESS;
//}

void tasks_create_tasks() 
{	
	central_data = central_data_get_pointer_to_struct();
	
	scheduler_t* scheduler = &central_data->scheduler;

	// scheduler_add_task(scheduler, 4000,	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_HIGHEST, &tasks_run_stabilisation                                          , 0                                                    , 0);
	scheduler_add_task(scheduler, 4000, 	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_HIGHEST, &tasks_run_stabilisation_quaternion                               , 0 													, 0);

	scheduler_add_task(scheduler, 20000, 	RUN_REGULAR, PERIODIC_RELATIVE, PRIORITY_HIGH   , (task_function_t)&remote_update 									, (task_argument_t)&central_data->remote 				, 1);
//	scheduler_add_task(scheduler, 200000, 	RUN_REGULAR, PERIODIC_RELATIVE, PRIORITY_NORMAL , (task_function_t)&remote_mode_update 								, (task_argument_t)&central_data->remote 				, 2);
	
	scheduler_add_task(scheduler, 15000, 	RUN_REGULAR, PERIODIC_RELATIVE, PRIORITY_HIGH   , &tasks_run_barometer_update                                       , 0 													, 3);
	scheduler_add_task(scheduler, 100000, 	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_HIGH   , &tasks_run_gps_update                                             , 0 													, 4);
	scheduler_add_task(scheduler, 10000, 	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_HIGH   , (task_function_t)&navigation_update                               , (task_argument_t)&central_data->navigation 			, 5);
	scheduler_add_task(scheduler, 200000, 	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_NORMAL , &tasks_set_mav_mode_n_state                                       , 0  													, 6);
	scheduler_add_task(scheduler, 4000, 	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_NORMAL , (task_function_t)&mavlink_communication_update                    , (task_argument_t)&central_data->mavlink_communication , 7);
	scheduler_add_task(scheduler, 100000, 	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_LOW    , (task_function_t)&analog_monitor_update                           , (task_argument_t)&central_data->analog_monitor 		, 8);
	scheduler_add_task(scheduler, 10000, 	RUN_REGULAR, PERIODIC_ABSOLUTE, PRIORITY_LOW    , (task_function_t)&waypoint_handler_control_time_out_waypoint_msg  , (task_argument_t)&central_data->waypoint_handler 		, 9);

	scheduler_sort_tasks(scheduler);
}