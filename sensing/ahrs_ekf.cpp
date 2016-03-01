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
 * \file ahrs_ekf.c
 * 
 * \author MAV'RIC Team
 * \author Nicolas Dousse
 *   
 * \brief EKF attitude estimation
 *
 ******************************************************************************/


#include "matrix.hpp"
#include "ahrs_ekf.h"

extern "C"
{
#include "constants.h"
}


using namespace mat;

Mat<7,1> x_state;
Mat<7,7> F;
Mat<7,7> P;
Mat<7,7> Q;
Mat<3,3> R_acc;
Mat<3,3> R_mag;
Mat<7,7> Id;

void ahrs_ekf_init_cpp(ahrs_ekf_t* ahrs_ekf, imu_t* imu);

void ahrs_ekf_predict_step(ahrs_ekf_t* ahrs_ekf);

void ahrs_ekf_update_step(ahrs_ekf_t* ahrs_ekf);

void ahrs_ekf_init_cpp(ahrs_ekf_t* ahrs_ekf)
{
	P = Mat<7,7>(100.0f,true);

	// Initalisation of the state
	x_state(0,0) = 0.0f;
	x_state(1,0) = 0.0f;
	x_state(2,0) = 0.0f;
	x_state(3,0) = 1.0f;
	x_state(4,0) = 0.0f;
	x_state(5,0) = 0.0f;
	x_state(6,0) = 0.0f;

	R_acc(0,0) = 0.01f;
	R_acc(1,1) = 0.01f;
	R_acc(2,2) = 0.01f;

	R_mag(0,0) = 0.01f;
	R_mag(1,1) = 0.01f;
	R_mag(2,2) = 0.01f;

	Id = Mat<7,7>(1.0f,true);
	
}


void ahrs_ekf_predict_step(ahrs_ekf_t* ahrs_ekf)
{
	float dt = ahrs_ekf->imu->dt;

	float w_x = ahrs_ekf->imu->scaled_gyro.data[X];
	float w_y = ahrs_ekf->imu->scaled_gyro.data[Y];
	float w_z = ahrs_ekf->imu->scaled_gyro.data[Z];

	Mat<7,1> x_k1k1 = x_state;

	Mat<7,1> x_kk1;
	// x(k,k-1) = f(x(k-1,k-1),u(k)); // with zero-order Taylor expansion
	x_kk1(0,0) = x_k1k1(0,0);
	x_kk1(1,0) = x_k1k1(1,0);
	x_kk1(2,0) = x_k1k1(2,0);
	x_kk1(3,0) = x_k1k1(3,0) + 0.5* (-(w_x-x_k1k1(0,0))*x_k1k1(4,0) - (w_y-x_k1k1(1,0))*x_k1k1(5,0) - (w_z-x_k1k1(2,0))*x_k1k1(6,0)) * dt;
	x_kk1(4,0) = x_k1k1(4,0) + 0.5* ((w_x-x_k1k1(0,0))*x_k1k1(3,0) + (w_z-x_k1k1(2,0))*x_k1k1(5,0) - (w_y-x_k1k1(1,0))*x_k1k1(6,0)) * dt;
	x_kk1(5,0) = x_k1k1(5,0) + 0.5* ((w_y-x_k1k1(1,0))*x_k1k1(3,0) - (w_z-x_k1k1(2,0))*x_k1k1(4,0) + (w_x-x_k1k1(0,0))*x_k1k1(6,0)) * dt;
	x_kk1(6,0) = x_k1k1(6,0) + 0.5* ((w_z-x_k1k1(2,0))*x_k1k1(3,0) + (w_y-x_k1k1(1,0))*x_k1k1(4,0) - (w_x-x_k1k1(0,0))*x_k1k1(5,0)) * dt;

	// F(k,k-1) = I + jacobian(x(k-1),u(k))*dt;
	F(0,0) = 1.0f;
	F(1,1) = 1.0f;
	F(2,2) = 1.0f;

	F(3,0) = x_k1k1(4,0) * dt;
	F(3,1) = x_k1k1(5,0) * dt;
	F(3,2) = x_k1k1(6,0) * dt;
	F(3,3) = 1.0f; // 1.0f + 0.0f;
	F(3,4) = -(w_x-x_k1k1(0,0)) * dt;
	F(3,5) = -(w_y-x_k1k1(1,0)) * dt;
	F(3,6) = -(w_z-x_k1k1(2,0)) * dt;
	
	F(4,0) = 1.0f - x_k1k1(3,0) * dt;
	F(4,1) = x_k1k1(6,0) * dt;
	F(4,2) = x_k1k1(5,0) * dt;
	F(4,3) = w_x-x_k1k1(0,0) * dt;
	F(4,4) = 1.0f; // 1.0f + 0.0f;
	F(4,5) = (w_z-x_k1k1(2,0)) * dt;
	F(4,6) = -(w_y-x_k1k1(1,0)) * dt;
	
	F(5,0) = 1.0f - x_k1k1(6,0) * dt;
	F(5,1) = -x_k1k1(3,0) * dt;
	F(5,2) = x_k1k1(4,0) * dt;
	F(5,3) = (w_y-x_k1k1(1,0)) * dt;
	F(5,4) = -(w_z-x_k1k1(2,0)) * dt;
	F(5,5) = 1.0f; // 1.0f + 0.0f;
	F(5,6) = (w_x-x_k1k1(0,0)) * dt;
	
	F(6,0) = x_k1k1(5,0) * dt;
	F(6,1) = -x_k1k1(4,0) * dt;
	F(6,2) = x_k1k1(3,0) * dt;
	F(6,3) = (w_z-x_k1k1(2,0)) * dt;
	F(6,4) = (w_y-x_k1k1(1,0)) * dt;
	F(6,5) = -(w_x-x_k1k1(0,0)) * dt;
	F(6,6) = 1.0f; // 1.0f + 0.0f;

	// Q(k) = cov(del_w * del_w^T)

	Q(0,0) = ahrs_ekf->sigma_w * ahrs_ekf->sigma_w * dt;
	Q(1,1) = ahrs_ekf->sigma_w * ahrs_ekf->sigma_w * dt;
	Q(2,2) = ahrs_ekf->sigma_w * ahrs_ekf->sigma_w * dt;

	Q(3,3) = 0.0001f;
	Q(3,4) = 0.0000001f;
	Q(3,5) = 0.0000001f;
	Q(3,6) = 0.0000001f;

	Q(4,3) = 0.0000001f;
	Q(4,4) = 0.0001f;
	Q(4,5) = 0.0000001f;
	Q(4,6) = 0.0000001f;

	Q(5,3) = 0.0000001f;
	Q(5,4) = 0.0000001f;
	Q(5,5) = 0.0001f;
	Q(5,6) = 0.0000001f;

	Q(6,3) = 0.0000001f;
	Q(6,4) = 0.0000001f;
	Q(6,5) = 0.0000001f;
	Q(6,6) = 0.0001f;

	// P(k,k-1) = F(k)*P(k-1,k-1)*F(k)' + Q(k)
	P = (F ^ P ^ F.transpose()) + Q;

	op::normalize(x_kk1,x_state);

}

void ahrs_ekf_update_step(ahrs_ekf_t* ahrs_ekf)
{
	uint16_t i;

	float mag_global[3];

	Mat<7,1> x_kk1 = x_state;

	mag_global[0] = cos(63.0f/180.0f*PI);
	mag_global[1] = 0.0f;
	mag_global[2] = sin(63.0f/180.0f*PI);

	Mat<3,1> z_acc;
	for (i = 0; i < 3; ++i)
	{
		z_acc(i,0) = ahrs_ekf->imu->scaled_accelero.data[i];
	}
	
	Mat<3,1> z_mag;
	for (i = 0; i < 3; ++i)
	{
		z_mag(i,0) = ahrs_ekf->imu->scaled_compass.data[i];
	}

	// h_acc(x(k,k-1))
	Mat<3,1> h_acc_xkk1;
	h_acc_xkk1(0,0) = 2.0f*(x_kk1(4,0)*x_kk1(6,0) - x_kk1(3,0)*x_kk1(5,0));
	h_acc_xkk1(1,0) = 2.0f*(x_kk1(5,0)*x_kk1(6,0) + x_kk1(3,0)*x_kk1(4,0));
	h_acc_xkk1(2,0) = (1.0f - 2.0f*(x_kk1(4,0)*x_kk1(4,0) + x_kk1(5,0)*x_kk1(5,0)));

	// h_mag(x(k,k-1))
	Mat<3,1> h_mag_xkk1;
	h_mag_xkk1(0,0) = (1.0f - 2.0f*(x_kk1(5,0)*x_kk1(5,0) + x_kk1(6,0)*x_kk1(6,0)))*mag_global[0] + 2.0f*(x_kk1(4,0)*x_kk1(6,0) - x_kk1(3,0)*x_kk1(5,0))*mag_global[2];
	h_mag_xkk1(1,0) = 2.0f*(x_kk1(4,0)*x_kk1(5,0) - x_kk1(3,0)*x_kk1(6,0))*mag_global[0] + 2.0f*(x_kk1(5,0)*x_kk1(6,0) + x_kk1(3,0)*x_kk1(4,0))*mag_global[2];
	h_mag_xkk1(2,0) = 2.0f*(x_kk1(4,0)*x_kk1(6,0) - x_kk1(3,0)*x_kk1(5,0))*mag_global[0] + (1.0f - 2.0f*(x_kk1(4,0)*x_kk1(4,0) + x_kk1(5,0)*x_kk1(5,0)))*mag_global[2];

	// H_acc(k) = jacobian(h_acc(x(k,k-1)))
	Mat<3,7> H_acc_k;

	H_acc_k(0,3) = 2.0f * x_kk1(5,0);
	H_acc_k(0,4) = -2.0 * x_kk1(6,0);
	H_acc_k(0,5) = 2.0f * x_kk1(3,0);
	H_acc_k(0,6) = -2.0f * x_kk1(4,0);

	H_acc_k(1,3) = -2.0f * x_kk1(4,0);
	H_acc_k(1,4) = -2.0f * x_kk1(3,0);
	H_acc_k(1,5) = -2.0f * x_kk1(6,0);
	H_acc_k(1,6) = -2.0f * x_kk1(5,0);

	H_acc_k(2,3) = 0.0f;
	H_acc_k(2,4) = 4.0f * x_kk1(4,0);
	H_acc_k(2,5) = 4.0f * x_kk1(5,0);
	H_acc_k(2,6) = 0.0f;

	// H_mag(k) = jacobian(h_mag(x(k,k-1)))
	Mat<3,7> H_mag_k;

	H_mag_k(0,3) = -2.0f * x_kk1(5,0) * mag_global[2];
	H_mag_k(0,4) = 2.0f * x_kk1(6,0) * mag_global[2];
	H_mag_k(0,5) = -4.0f * x_kk1(5,0) * mag_global[0] - 2.0f * x_kk1(3,0) * mag_global[2];
	H_mag_k(0,6) = -4.0f * x_kk1(6,0) * mag_global[0] + 2.0f * x_kk1(4,0) * mag_global[2];

	H_mag_k(1,3) = -2.0f * x_kk1(6,0) * mag_global[0] + 2.0f * x_kk1(4,0) * mag_global[2];
	H_mag_k(1,4) = 2.0f * x_kk1(5,0) * mag_global[0] + 2.0f * x_kk1(3,0) * mag_global[2];
	H_mag_k(1,5) = 2.0f * x_kk1(4,0) * mag_global[0] + 2.0f * x_kk1(6,0) * mag_global[2];
	H_mag_k(1,6) = -2.0f * x_kk1(3,0) * mag_global[0] + 2.0f * x_kk1(5,0) * mag_global[2];

	H_mag_k(2,3) = -2.0f * x_kk1(5,0) * mag_global[0];
	H_mag_k(2,4) = 2.0f * x_kk1(6,0) * mag_global[0] - 4.0f * x_kk1(4,0) * mag_global[2];
	H_mag_k(2,5) = -2.0f * x_kk1(3,0) * mag_global[0] - 4.0f * x_kk1(5,0) * mag_global[2];
	H_mag_k(2,6) = 2.0f * x_kk1(4,0) * mag_global[0];

	// Innovation y(k) = z(k) - h(x(k,k-1))
	Mat<3,1> yk_acc = z_acc - h_acc_xkk1;
	Mat<3,1> yk_mag = z_mag - h_mag_xkk1;

	// Innovation covariance S(k) = H(k) * P(k,k-1) * H(k)' + R

	//Mat<7,3> H_acc_T = ;

	Mat<3,3> Sk_acc = (H_acc_k ^ P ^ H_acc_k.transpose()) + R_acc;
	Mat<3,3> Sk_mag = (H_mag_k ^ P ^ H_mag_k.transpose()) + R_mag;

	// Kalman gain: K(k) = P(k,k-1) * H(k)' * S(k)^-1
	Mat<3,3> Sk_inv;
	op::inverse(Sk_acc, Sk_inv);
	Mat<7,3> K_acc = P ^ (H_acc_k.transpose() ^ Sk_inv);
	op::inverse(Sk_mag, Sk_inv);
	Mat<7,3> K_mag = P ^ (H_mag_k.transpose() ^ Sk_inv);

	// Updated state estimate: x(k,k) = x(k,k-1) + K(k)*y_k
	x_kk1 = x_state;
	x_kk1 = x_kk1 + (K_acc ^ yk_acc);
	Mat<7,1> x_kk = x_kk1 + (K_mag ^ yk_mag);

	op::normalize(x_kk,x_state);

	// Update covariance estimate
	P = (Id - (K_acc ^ H_acc_k)) ^ P;
	P = (Id - (K_mag ^ H_mag_k)) ^ P;

}

bool ahrs_ekf_init(ahrs_ekf_t* ahrs_ekf, imu_t* imu, ahrs_t* ahrs)
{
	bool init_success = true;

	ahrs_ekf->imu = imu;
	ahrs_ekf->ahrs = ahrs;

	ahrs_ekf_init_cpp(ahrs_ekf);

	return init_success;
}

void ahrs_ekf_update(ahrs_ekf_t* ahrs_ekf)
{
	ahrs_ekf_predict_step(ahrs_ekf);

	ahrs_ekf_update_step(ahrs_ekf);

	uint16_t i;
	for (i = 0; i < 7; ++i)
	{
		ahrs_ekf->x[i] = x_state(i,0);
	}

	ahrs_ekf->ahrs->qe.s = x_state(i,3);
	ahrs_ekf->ahrs->qe.v[0] = x_state(i,4);
	ahrs_ekf->ahrs->qe.v[1] = x_state(i,5);
	ahrs_ekf->ahrs->qe.v[2] = x_state(i,6);

}