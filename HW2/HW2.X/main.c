/* 
 * File:   main.c
 * Author: nalex
 *
 * Created on May 22, 2020, 3:23 PM
 */


#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include <stdlib.h>
#include "spi.h"
#include <math.h>




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

#define NUMSAMPS 120

unsigned short makeOutput(unsigned char c, unsigned short v);

static volatile int SinWave[NUMSAMPS];
static volatile int TriWave[NUMSAMPS];

void makeSinWave() {
	int i = 0, max = 4095;
	while (i < NUMSAMPS) {
		SinWave[i] = 2047.0*sin(2*M_PI/(NUMSAMPS-1)*i) + 2048;
        i++;
    }
}


void makeTriWave() {
	int i = 0, max = 4095, A = 68*2;
	for (i = 0; i < NUMSAMPS/2; i++) {
		if (i < NUMSAMPS / 4 + 1) {
			TriWave[i] = i*A;
		}
		else {
			TriWave[i] = TriWave[i-1]-A;
		}
	}
}

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
    
    initSPI();
    __builtin_enable_interrupts();
    
    
    unsigned short v;
    unsigned char c;
    unsigned short i;
    unsigned int count = 0;
    unsigned int count2 = 0;
    
    makeSinWave();
    makeTriWave();
    
    while (1) {
        
        v = SinWave[count];
        c = 0;
        i = makeOutput(c,v);
        LATAbits.LATA0 = 0; // Trigger CS low
        spi_io(i>>8);  // Send i
        spi_io(i);
        LATAbits.LATA0 = 1; // Terminate CS high
        
        v = TriWave[count2];
        c = 1;
        i = makeOutput(c,v);
        LATAbits.LATA0 = 0; // Trigger CS low
        spi_io(i>>8);
        spi_io(i);  // Send i
        LATAbits.LATA0 = 1; // Terminate CS high
        
        count++;
        if (count >= NUMSAMPS) {
            count = 0;
        }
        count2++;
        if (count2 >= NUMSAMPS/2) {
            count2 = 0;
        }
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<48000000/2/240){ //240Hz
            ;
        }
    }
    
}

unsigned short makeOutput(unsigned char c, unsigned short v) {
    
    unsigned short p;
    p= c<<15;
    p = p|(0b111<<12);
    p = p|v;       
    
    return p;
    
}

