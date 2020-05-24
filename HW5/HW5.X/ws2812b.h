/* 
 * File:   ws2812b.h
 * Author: nalex
 *
 * Created on May 23, 2020, 9:13 PM
 */

#ifndef WS2812B_H__
#define WS2812B_H__

#include<xc.h> // processor SFR definitions

// link three 8bit colors together
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

void ws2812b_setup();
void ws2812b_setColor(wsColor*,int);
wsColor HSBtoRGB(float hue, float sat, float brightness);

#endif