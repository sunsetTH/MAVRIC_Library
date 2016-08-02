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
 * \file position_controller.hpp
 *
 * \author MAV'RIC Team
 * \author Basil Huber
 *
 * \brief Interface for position controller
 *
 ******************************************************************************/


#ifndef POSITION_CONTROLLER_HPP_
#define POSITION_CONTROLLER_HPP_

#include "control/pid_controller.hpp"
#include "control/stabilisation.hpp"
#include "sensing/ins.hpp"

class Position_controller
{
public:

    Position_controller(control_command_t& vel_command, const INS& ins, const quat_t& qe);

    virtual void update() = 0;

    inline void set_position_command(local_position_t pos_command_lf){pos_command_lf_ = pos_command_lf;};

    inline float goal_distance(){return goal_distance_;};

protected:

    void update_internal();

    const INS&          ins_;
    const quat_t&       qe_;                                ///< Attitude quaternion structure
    
    control_command_t&  vel_command_lf_;
    local_position_t    pos_command_lf_;
    float               rel_goal_pos_[3];                  ///< Relative goal position in NED (updated by update_internal() )
    float               goal_distance_;                    ///< Distance to the goal (updated by update_internal() )
    float               dir_desired_sg_[3];                ///< Direction to the goal in semi global frame (unit vector) (updated by update_internal() )

};

#endif /* POSITION_CONTROLLER_HPP_ */