/* 
 * File:   main.c
 * Author: nalex
 *
 * Created on May 23, 2020, 4:31 PM
 */

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "ssd1306.h"
#include "i2c_master_noint.h"
#include "font.h"
#include "imu.h"


// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here

    TRISBbits.TRISB4 = 1;   // initializes B4 as a digital input
    TRISAbits.TRISA4 = 0;   // initializes A4 as a digital output
    
    LATAbits.LATA4 = 0;      // turn off the digital output 
    
    i2c_master_setup();
    imu_setup();
    ssd1306_setup();
    
    __builtin_enable_interrupts();

    char m[50];
    signed short imu[10];

    _CP0_SET_COUNT(0);
    while (1) {
    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
    // remember the core timer runs at half the sysclk

    // Flash LED at 5Hz
    if (_CP0_GET_COUNT()<24000000/10){
        LATAbits.LATA4 = 1; // Turns on the digital output
    }
    else if (_CP0_GET_COUNT()<24000000/5){
        LATAbits.LATA4 = 0; // Turns off the digital output
    }
    else {
        _CP0_SET_COUNT(0);
    }     
    
    imu_read(IMU_OUT_TEMP_L,imu,7);
    
    if (0) {
        sprintf(m,"g: %d %d %d",imu[1],imu[2],imu[3]);
        drawString(0,0,m);
        sprintf(m,"a: %d %d %d",imu[4],imu[5],imu[6]);
        drawString(0,8,m);
        sprintf(m,"TEMP: %d",imu[0]);
        drawString(0,16,m);
    } else {
        barx(-imu[5]);
        bary(imu[4]);
    }
    
    ssd1306_update();

    }
}

void barx(signed short x) {
    ssd1306_clear();
    signed char len = abs(x)/500;
    int i;
    
    if (x<0) {
        for (i = 0; i<len; i++) {
            ssd1306_drawPixel(60-i,16,1);
        }
    } else {
        for (i = 0; i<len; i++) {
            ssd1306_drawPixel(60+i,16,1);
        }
    }
    
    
}

void bary(signed short y) {
    signed char len = abs(y)/500;
    int i;
    
    if (y<0) {
        for (i = 0; i<len; i++) {
            ssd1306_drawPixel(60,16-i,1);
        }
    } else {
        for (i = 0; i<len; i++) {
            ssd1306_drawPixel(60,16+i,1);
        }
    }
}