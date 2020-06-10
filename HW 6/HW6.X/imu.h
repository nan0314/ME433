/* 
 * File:   imu.h
 * Author: nalex
 *
 * Created on June 8, 2020, 9:02 PM
 */

#ifndef IMU_H__
#define IMU_H__

#include<xc.h> // processor SFR definitions

#define IMU_WHOAMI 0x0F
#define IMU_ADDR 0b11010110
#define IMU_CTRL1_XL 0x10
#define IMU_CTRL2_G 0x11
#define IMU_CTRL3_C 0x12
#define IMU_OUT_TEMP_L 0x20
#define IMU_WRITE 0b11010110
#define IMU_READ 0b11010111

void imu_setup();
void imu_read(unsigned char, signed short *, int);

#endif