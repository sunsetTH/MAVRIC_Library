/*******************************************************************************
 * Copyright (c) 2009-2016, MAV'RIC Development Team
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
 * \file velocity_controller_copter.cpp
 *
 * \author MAV'RIC Team
 * \author Felix Schill
 * \author Nicolas Dousse
 * \author Julien Lecoeur
 *
 * \brief A velocity controller for copters.
 *
 ******************************************************************************/

#include <array>

#include "control/velocity_controller_copter.hpp"
#include "util/coord_conventions.hpp"
#include "util/constants.hpp"
#include "util/quick_trig.hpp"

extern "C"
{
#include "util/quaternions.h"
#include "util/maths.h"
#include "util/vectors.h"
}


Velocity_controller_copter::Velocity_controller_copter(const args_t& args, const conf_t& config) :
    Velocity_controller(args.vel_args, config),
    thrust_hover_point_(config.thrust_hover_point)
{}


bool Velocity_controller_copter::compute_attitude_and_thrust_from_desired_accel(const std::array<float,3>& accel_vector,
                                                                                attitude_command_t& attitude_command,
                                                                                thrust_command_t& thrust_command)
{
    // Desired attitude with yaw facing north using axis/angle representation
    float alpha   = maths_clip(maths_fast_sqrt(accel_vector[X]*accel_vector[X] + accel_vector[Y]*accel_vector[Y]), 1);
    float axis[3] = {0.0f, 0.0f, 0.0f};
    if (alpha != 0.0f)
    {
        axis[X] =   accel_vector[Y] / alpha;
        axis[Y] = - accel_vector[X] / alpha;
        axis[Z] =  0.0f;
    }
    quat_t qrollpitch = coord_conventions_quaternion_from_angle_axis(alpha, axis);

    // Rotation to face the goal
    quat_t qyaw = coord_conventions_quaternion_from_rpy(  0.0f,
                                                          0.0f,
                                                          velocity_command_.heading );

    // Desired thrust
    float thrust = thrust_hover_point_ + accel_vector[Z] / quick_trig_cos(alpha);

    // Output
    attitude_command      = quaternions_multiply(qrollpitch, qyaw);
    thrust_command.xyz[X] = 0.0f;
    thrust_command.xyz[Y] = 0.0f;
    thrust_command.xyz[Z] = thrust;

    return true;
}
