#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include <stdlib.h>

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    // Turn off all A analog pins
    ANSELA = 0;
    // Make an output pin for CS
    
    TRISAbits.TRISA0 = 0;   // Declares pin A0 as digital output
    LATAbits.LATA0 = 1;     // Initialized A0 to high
    
    // Set SDO1
    RPA1Rbits.RPA1R = 0b0011; // Sets A1 as SDO1
    // Set SDI1
    SDI1Rbits.SDI1R = 0b000l; // Sets B5 as SDI1

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 2000; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}

unsigned char spi_io(unsigned char o) {
    SPI1BUF = o;
    while(!SPI1STATbits.SPIRBF) { 
        ;   // waits to receive bytes
    }return SPI1BUF;
}