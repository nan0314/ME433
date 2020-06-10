#include "font.h"
#include "ssd1306.h"

// Draw a letter at a position
void drawChar(unsigned char x, unsigned char y, char letter) {
    unsigned char j;
    unsigned char k;
    unsigned char on;
    
    for (j = 0; j<5; j++) {
        for (k = 0; k<8; k++) {
            on = (ASCII[letter-0x20][j]>>k)&1;
            ssd1306_drawPixel(x+j, y+k, on);
        }
    }
}

void drawString(unsigned char p1, unsigned char p2,char m[]) {
    unsigned int s = 0;
    
    while(m[s] != 0) {
        drawChar(p1,p2,m[s]);
        p1+=5;
        s++;
    }
}

