/*
 * File:   pidap008_finalproject_main.c
 * Author: ashvin
 *
 * Created on May 2, 2019, 5:17 PM
 */


#include "xc.h"

void setup(void){
    CLKDIVbits.RCDIV = 0;          //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9ffe;       //sets all pins to digital IO except RA0;
    TRISA = 0b0000000000000001;    // receive input on pin 0
    TRISB = 0b0000111000000000;    //set portB to receive input on pin 9
    microphoneInit();
    SPIinit();
    ICinit();
}

int main(){

    setup();
    writeColor(0,0,0);

    while(1){
        //statBits 1-3 represent 3 lines the ESP8266 will send to the PIC. statBit1 is for boosted or not.
        //statBits<2:3> = 0 means off, statBits<2:3> = 1 means wheel, statBits<2:3> = 2 means strobe and 
        //statBits<2:3> = 3 is unimplemented.
        while(!statBit2){  //if 0b00 or 0b01
            writeColor(0,0,0);
        }
        
        while(statBit2){   //if 0b10 or 0b11
            if(statBit1){   //if 0b11
                boostedWheel();
            }
            else{
                wheel();
            }

        }
        
    }
}
