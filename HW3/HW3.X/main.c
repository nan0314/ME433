/* 
 * File:   main.c
 * Author: nalex
 *
 * Created on May 23, 2020, 11:17 AM
 */

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "i2c_master_noint.h"

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
    
    __builtin_enable_interrupts();
    
    unsigned char wadd = 0b01000000;
    unsigned char radd = 0b01000001;
    unsigned char value;
    
    // Make all A pins Outputs
    
    i2c_master_start(); // send a START signal
    i2c_master_send(wadd); // send write address
    i2c_master_send(0x00); // send A pins register
    i2c_master_send(0x00); // set A pins as output
    i2c_master_stop(); // send a stop
    
    // Make all B pins Inputs
    i2c_master_start(); // send a START signal
    i2c_master_send(wadd); // send write address
    i2c_master_send(0x01); // send B pins register
    i2c_master_send(0xFF); // set B pins as inputs
    i2c_master_stop(); // send a stop

    _CP0_SET_COUNT(0);
    while (1) {
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk
        
        if (_CP0_GET_COUNT()<24000000/10){
            LATAbits.LATA4 = 1; // Turns on the digital output
        }
        else if (_CP0_GET_COUNT()<24000000/5){
            LATAbits.LATA4 = 0; // Turns off the digital output
        }
        else {
            _CP0_SET_COUNT(0);
        }     
        
        // Read GPB0 value
        
        i2c_master_start(); // send a START signal
        i2c_master_send(wadd); // send write address
        i2c_master_send(0x13); // send GPBO input register
        i2c_master_restart(); // send a RESTART signal
        i2c_master_send(radd); // send read address
        value = i2c_master_recv(); // receive GPB0 value
        i2c_master_ack(1); // send an ACK (1) or NACK (0)
        i2c_master_stop(); // send a stop
        
        // If button not pushed turn off LED, else turn on LED
        if (value) {
            i2c_master_start(); // send a START signal
            i2c_master_send(wadd); // send write address
            i2c_master_send(0x14); // send A pins output register
            i2c_master_send(0x00); // set all A pins to low
            i2c_master_stop(); // send a stop
        }
        else {
            i2c_master_start(); // send a START signal
            i2c_master_send(wadd); // send write address
            i2c_master_send(0x14); // send A pins output register
            i2c_master_send(0b10000000); // set GPA7 pin to high
            i2c_master_stop(); // send a stop
        }
        
            
        
    }
}