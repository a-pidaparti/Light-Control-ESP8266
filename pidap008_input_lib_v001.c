/*
 * File:   pidap008_lightcontrol_lib.c
 * Author: ashvin pidaparti
 *
 * Created on April 25, 2019, 11:43 PM
 */
#include "p24Fxxxx.h"
#include "xc.h"
#include "pidap008_output_lib_header.h"



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

volatile unsigned int boost = 0;
volatile int count = 0;

#define RED LATBbits.LATB6
#define GREEN LATBbits.LATB7
#define BLUE LATBbits.LATB8

volatile int statBit1 = 0;
volatile int statBit2 = 0;


void __attribute__((__interrupt__, __auto_psv__)) _IC1Interrupt(){
    _IC1IF = 0;
    statBit1 ^= 1;      //toggle
}

void __attribute__((__interrupt__, __auto_psv__)) _IC2Interrupt(){
    _IC2IF = 0;
    statBit2 ^= 1;      //toggle
}


void __attribute__((__interrupt__, __auto_psv__)) _ADC1Interrupt()
{
    _AD1IF = 0;             //reset interrupt flag
    boost = ADC1BUF0/13;    //boost is the value added to the values sent to the pots
//    if(boost < 40){
//        boost = 0;
//    }            //divide by 13 to scale so value passed to pot <256
}

void microphoneInit(void)
{    
    //Timer 3 Setup
    T3CON = 0x0030;                         // Stop Timer, Tcy clk source, PRE 1:256
    TMR3 = 0;                               // Initialize to zero (also best practice)
    PR3 = 4096;                             // Set period to max
    T3CONbits.TON = 1;                      // Start T3

    AD1CON1bits.ADON = 0;                   //turn off ADC
    AD1CON1 = 0;                            //best practice to reset all control registers
    AD1CON2 = 0;
    AD1CON2 = 0;
    AD1CON1bits.ASAM = 1;                   //turn on autosampling
    AD1CON1bits.SSRC = 0b010;               //Use timer 3 for when to convert
    AD1CON3bits.SAMC = 1;                   //Sample every Tad (every time timer 3 completes)
    AD1CON3bits.ADCS = 1;                   //Convert every 2 Tcy
    AD1CON1bits.ADON = 1;                   //Turn ADC1 back on
    
    _AD1IF = 0;                             //set interrupt flag
    _AD1IE = 1;                             //enable interrupt
}

void ICinit(void)
{
    //Peripheral Pin Select Setup
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 9;                    // Use Pin RP9 = "9", for Input Capture 1 (Table 10-2)
    RPINR7bits.IC2R = 10;
    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS
    
    CNPU2bits.CN21PUE = 1;
    CNPU2bits.CN16PUE = 1;
    CNPU1bits.CN15PUE = 1;
    
    //Timer 2 Setup
    T2CON = 0x0030;                         // Stop Timer, Tcy clk source, PRE 1:256
    TMR2 = 0;                               // Initialize to zero (also best practice)
    PR2 = 65535;                            // Set period to max
    T2CONbits.TON = 1;                      // Restart 16-bit Timer2
    _T2IF = 0;                              // Clear interrupt flag
    
    //Input Capture Setup
    IC1CON = 0;                             // Turn off and reset internal state of IC1
    IC1CONbits.ICTMR = 1;                   // Use Timer 2 for capture source
    IC1CONbits.ICM = 0b010;                 // Turn on and capture every falling edge
    _IC1IE = 1;                             // Enable interrupt 
    
    IC2CON = 0;                             // Turn off and reset internal state of IC1
    IC2CONbits.ICTMR = 1;                   // Use Timer 2 for capture source
    IC2CONbits.ICM = 0b010;                 // Turn on and capture every falling edge
    _IC2IE = 1;                             // Enable interrupt 
}