/* 
 * File:   main.c
 * Author: nalex
 *
 * Created on May 23, 2020, 10:31 AM
 */

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "ws2812b.h"

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
    
    
    __builtin_enable_interrupts();

    ws2812b_setup();
            
    wsColor colors[4];
    float h1 = 0;
    float h2 = 60;
    float h3 = 120;
    float h4 = 180;
    float update = .2;
            
    while (1) {
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk
        
        if (h1 >= 360) {
            h1 = 0;
        }
        else if (h2 >= 360){
            h2 = 0;
        }
        else if (h3 >= 360){
            h3 = 0;
        }
        else if (h4 >= 360){
            h4= 0;
        }
       
        colors[0] = HSBtoRGB(h1,1,1);
        colors[1] = HSBtoRGB(h2,1,1);
        colors[2] = HSBtoRGB(h3,1,1);
        colors[3] = HSBtoRGB(h4,1,1);
        
        h1+= update;
        h2+= update;
        h3+= update;
        h4+= update;
        ws2812b_setColor(colors, 4);
        
        
    }
}