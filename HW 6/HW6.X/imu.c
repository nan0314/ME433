#include "imu.h"

void imu_setup(){
    unsigned char who = 0;

    // read from IMU_WHOAMI
    i2c_master_start(); // send a START signal
    i2c_master_send(IMU_WRITE); // send write address
    i2c_master_send(IMU_WHOAMI); // send WHOAMI input register
    i2c_master_restart(); // send a RESTART signal
    i2c_master_send(IMU_READ); // send read address
    who = i2c_master_recv(); // receive WHOAMI value
    i2c_master_ack(1); // send an ACK (1) or NACK (0)
    i2c_master_stop(); // send a stop
    
    
    if (who != 0b01101001){
        while(1){}
    }

    // init IMU_CTRL1_XL
    i2c_master_start(); // send a START signal
    i2c_master_send(IMU_WRITE); // send write address
    i2c_master_send(0x10); // send CTRL1_XL register
    i2c_master_send(0b10000010); // Set to desired settings
    i2c_master_stop(); // send a stop
    
    // init IMU_CTRL2_G
    i2c_master_start(); // send a START signal
    i2c_master_send(IMU_WRITE); // send write address
    i2c_master_send(0x11); // send CTRL2_G register
    i2c_master_send(0b10001000); // Set to desired settings
    i2c_master_stop(); // send a stop
    
    // init IMU_CTRL4_C
    i2c_master_start(); // send a START signal
    i2c_master_send(IMU_WRITE); // send write address
    i2c_master_send(0x13); // send CTRL2_G register
    i2c_master_send(0b00010000); // Set to desired settings
    i2c_master_stop(); // send a stop
    // default settings adequate
}

void imu_read(unsigned char reg, signed short * data, int len){
    
    unsigned char recieve[2*len];
    int i,k = 0;
    
    // read multiple from the imu, each data takes 2 reads so you need len*2 chars
    i2c_master_start(); // send a START signal
    i2c_master_send(IMU_WRITE); // send write address
    i2c_master_send(reg); // send register address
    i2c_master_restart(); // send a RESTART signal
    i2c_master_send(IMU_READ); // send read address
    for (i = 0; i < 2*len-1; i++){
        recieve[i] = i2c_master_recv(); //receive IMU value
        i2c_master_ack(0);
    }
    recieve[2*len-1] = i2c_master_recv(); // receive IMU value
    i2c_master_ack(1); // send an ACK (1) or NACK (0)
    i2c_master_stop(); // send a stop
    
    // turn the chars into the shorts
    for (i = 0; i < len; i++) {
        data[i] = (recieve[k+1]<<8)|(recieve[k]);
        k+=2;
    }
    
}
