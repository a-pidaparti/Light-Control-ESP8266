
/*
 * File:   pidap008_lightcontrol_lib.c
 * Author: ashvin pidaparti
 *
 * Created on April 25, 2019, 11:43 PM
 */
#include "p24Fxxxx.h"
#include "xc.h"
#include "pidap008_microphone_lib_header.h"



// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)
                                                            
// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                   // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

#define RED LATBbits.LATB6
#define GREEN LATBbits.LATB7
#define BLUE LATBbits.LATB8

void SPIinit(void){
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPOR1bits.RP2R = 7;                     // Use Pin RP2 (RB2) for SDO1 = "7" (Table 10-3)
    RPOR1bits.RP3R = 8;                     // Use Pin RP3 (RB3) for SCK1OUT = "8"
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS

    
    RED = 1;            //set all ~CS pins to hight (disable all pots)
    GREEN = 1;
    BLUE = 1;
    
    
    SPI1CON1 = 0;
    SPI1STAT = 0;
    SPI1CON2 = 0;
    SPI1BUF = 0;
    SPI1CON1bits.SPRE = 0b110;  //sets secondary prescalar to 2:1
    SPI1CON1bits.PPRE = 0b11;   //sets primary prescalar to 1;1. with secondary PS, SCK = 8000 kHz

    SPI1CON1bits.CKE = 1;       //Data output changes on falling edge (pot reads on rising edge)
    SPI1CON1bits.CKP = 0;       //Set "idle" state as low and "active" as high
    SPI1CON1bits.MSTEN = 1;     //enables master mode
    _SPI1IE = 0;                //Turn off SPI1 interrupts
    _SPI1IF = 0;                //Clear SPI1 interrupt flag   
    SPI1STATbits.SPIEN = 1;     //enable SPI1
    
}

void delay(int count){      //wait count ms
    int counter = count;
    while(counter >= 0){
        asm("repeat #15993");
        asm("nop");
        counter--;
    }
}

void SPItransfer(char color, uint8_t package){
    
    uint8_t trash;          //place to dump SPI1BUF0 values after transferring
    
    if (color == 'R'){  //if red selected
        RED = 0;
        GREEN = 1;
        BLUE = 1; 
    }
    
    else if (color == 'B'){ //if blue selected
        RED = 1;
        GREEN = 1;
        BLUE = 0;
    }
    
    else if(color == 'G'){  //if green selected
        RED = 1;
        GREEN = 0;
        BLUE = 1;
    }
    
    SPI1BUF = 0x11; //0b00010001    //command byte corresponding to receive on pot 1
    while(!SPI1STATbits.SPIRBF);    //poll for whether or not data has been received (all data has been sent)
    trash = SPI1BUF;    //discard value of SPI1BUF0
    
    
    SPI1BUF = package;  //send package
    while(!SPI1STATbits.SPIRBF);    //wait for transfer to start
    trash = SPI1BUF;
    
    RED = 1;    //reset ~CS pins
    BLUE = 1;
    GREEN = 1;

}

void writeColor(uint8_t r, uint8_t g, uint8_t b){   //call SPItransfer for all colors
    SPItransfer('R',r);
    SPItransfer('B',b);
    SPItransfer('G',g);
}

void boostedWheel(){
//    //In a period of 360 degrees, R is maximum from 0 to 60, falls to 0 from 60 to 120, rises to maximum from 240 to 300
//    //G rises from 0 to maximum from 0 to 60, falls to 0 from 180 to 240
//    //B rises 120-180, falls 300-360
//  
    int min = 50;   //minimum effective level 
    int max = 175;  //maximum level to allow for boosting
    int r = max;    //Allows for setting the desired maximum level
    int g = 0;
    int b = 0;
    writeColor(r + boost, g + boost, b + boost);
    
    while(g <= 125){
        writeColor(r + boost,g + boost,b);
        g += 1;
        delay(5);
    }
    count ++;
    while(r > min){
        writeColor(r + boost, g + boost, b);
        r --;
        delay(5);
    }
    count++;
    while(b <= max){
        writeColor(r, g + boost, b + boost);
        b ++;
        delay(5);
    }
    count++;
    while(g > min){
        writeColor(r, g + boost, b + boost);
        g -= 1;
        delay(5);
    }
    
    while(r <= max){
        writeColor(r + boost, g, b + boost);
        r ++;
        delay(5);
    }
    
    while(b > min){
        writeColor(r + boost, g, b + boost);
        b --;
        delay(5);
    }
}

void wheel(){
//    //In a period of 360 degrees, R is maximum from 0 to 60, falls to 0 from 60 to 120, rises to maximum from 240 to 300
//    //G rises from 0 to maximum from 0 to 60, falls to 0 from 180 to 240
//    //B rises 120-180, falls 300-360
//  
    int min = 50;   //minimum effective level 
    int max = 175;  //maximum level to allow for boosting
    int r = max;    //Allows for setting the desired maximum level
    int g = 0;
    int b = 0;
    writeColor(r, g, b);
    
    while(g < max){
        writeColor(r,g,b);
        g ++;
        delay(5);
    }
    count ++;
    while(r > min){
        writeColor(r, g, b);
        r --;
        delay(5);
    }
    count++;
    while(b < max){
        writeColor(r, g, b);
        b ++;
        delay(5);
    }
    count++;
    while(g > min){
        writeColor(r, g, b);
        g -= 1;
        delay(5);
    }
    
    while(r < max){
        writeColor(r, g, b);
        r ++;
        delay(5);
    }
    
    while(b > min){
        writeColor(r, g, b);
        b --;
        delay(5);
    }
}
