/*
  June 2012

  BaseFlightPlus Rev -

  An Open Source STM32 Based Multicopter

  Includes code and/or ideas from:

  1)AeroQuad
  2)BaseFlight
  3)CH Robotics
  4)MultiWii
  5)S.O.H. Madgwick

  Designed to run on Naze32 Flight Control Board

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef  MPU6050_H
#define  MPU6050_H
#include "stdint.h"
///////////////////////////////////////////////////////////////////////////////
// MPU6050 Gyro Module Variables
///////////////////////////////////////////////////////////////////////////////

#define GYRO_SCALE_FACTOR     0.00121414f       //  MPU6050 14.375 LSBs per dps

///////////////////////////////////////////////////////////////////////////////
// Read Gyro
///////////////////////////////////////////////////////////////////////////////

uint8_t MPU6050_gyro_read(int16_t * data);

void MPU6050_temp_read(int16_t * data);

///////////////////////////////////////////////////////////////////////////////
// Compute Gyro Temperature Compensation Bias
///////////////////////////////////////////////////////////////////////////////

void MPU6050_compute_gyro_TC_bias(float *bias, int16_t temp);

///////////////////////////////////////////////////////////////////////////////
// Compute Gyro Runtime Bias
///////////////////////////////////////////////////////////////////////////////

void MPU6050_compute_gyro_runtime_bias(float* bias, uint32_t samples);

///////////////////////////////////////////////////////////////////////////////
// Gyro Initialization
///////////////////////////////////////////////////////////////////////////////

void MPU6050_gyro_init(void);

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// MPU6050 ACC module Variables
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Read Accel
///////////////////////////////////////////////////////////////////////////////

void readAccel(void);

///////////////////////////////////////////////////////////////////////////////
// Compute Accel Runtime Bias
///////////////////////////////////////////////////////////////////////////////

uint8_t MPU6050_acc_read(int16_t* data);

///////////////////////////////////////////////////////////////////////////////
// Accel Initialization
///////////////////////////////////////////////////////////////////////////////

void MPU6050_acc_init(void);

///////////////////////////////////////////////////////////////////////////////
#endif