/**
  Generated Pin Manager File

  Company:
    Microchip Technology Inc.

  File Name:
    pin_manager.c

  Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC18F47K40
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB             :  MPLAB X 5.20

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "pin_manager.h"
#include "mcc.h"
#include "../commandes.h"

extern int16_t Vitesse_Moteur_G;
extern int16_t Vitesse_Moteur_D;

//#define Max_Selection 8

uint16_t selection = 0;

void (*IOCAF5_InterruptHandler)(void);
void (*IOCAF6_InterruptHandler)(void);
void (*IOCAF7_InterruptHandler)(void);
void (*IOCCF0_InterruptHandler)(void);


void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATE = 0x00;
    LATD = 0x00;
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;

    /**
    TRISx registers
    */
    TRISE = 0x02;
    TRISA = 0xE0;
    TRISB = 0xC7;
    TRISC = 0x99;
    TRISD = 0xE3;

    /**
    ANSELx registers
    */
    ANSELD = 0x1C;
    ANSELC = 0x66;
    ANSELB = 0xF8;
    ANSELE = 0x05;
    ANSELA = 0x1F;

    /**
    WPUx registers
    */
    WPUD = 0x06;
    WPUE = 0x00;
    WPUB = 0x00;
    WPUA = 0xC0;
    WPUC = 0x00;

    /**
    ODx registers
    */
    ODCONE = 0x00;
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;
    ODCOND = 0x00;

    /**
    SLRCONx registers
    */
    SLRCONA = 0xFF;
    SLRCONB = 0xFF;
    SLRCONC = 0xFF;
    SLRCOND = 0xFF;
    SLRCONE = 0x07;

    /**
    INLVLx registers
    */
    INLVLA = 0xFF;
    INLVLB = 0xFF;
    INLVLC = 0xFF;
    INLVLD = 0xFF;
    INLVLE = 0x0F;


    /**
    IOCx registers 
    */
    //interrupt on change for group IOCAF - flag
    IOCAFbits.IOCAF5 = 0;
    //interrupt on change for group IOCAF - flag
    IOCAFbits.IOCAF6 = 0;
    //interrupt on change for group IOCAF - flag
    IOCAFbits.IOCAF7 = 0;
    //interrupt on change for group IOCAN - negative
    IOCANbits.IOCAN5 = 1;
    //interrupt on change for group IOCAN - negative
    IOCANbits.IOCAN6 = 1;
    //interrupt on change for group IOCAN - negative
    IOCANbits.IOCAN7 = 1;
    //interrupt on change for group IOCAP - positive
    IOCAPbits.IOCAP5 = 0;
    //interrupt on change for group IOCAP - positive
    IOCAPbits.IOCAP6 = 1;
    //interrupt on change for group IOCAP - positive
    IOCAPbits.IOCAP7 = 0;
    //interrupt on change for group IOCCF - flag
    IOCCFbits.IOCCF0 = 0;
    //interrupt on change for group IOCCN - negative
    IOCCNbits.IOCCN0 = 1;
    //interrupt on change for group IOCCP - positive
    IOCCPbits.IOCCP0 = 0;



    // register default IOC callback functions at runtime; use these methods to register a custom function
    IOCAF5_SetInterruptHandler(IOCAF5_DefaultInterruptHandler);
    IOCAF6_SetInterruptHandler(IOCAF6_DefaultInterruptHandler);
    IOCAF7_SetInterruptHandler(IOCAF7_DefaultInterruptHandler);
    IOCCF0_SetInterruptHandler(IOCCF0_DefaultInterruptHandler);
   
    // Enable IOCI interrupt 
    PIE0bits.IOCIE = 1; 
    
	
    SSP1CLKPPS = 0x13;   //RC3->MSSP1:SCL1;    
    RX1PPS = 0x17;   //RC7->EUSART1:RX1;    
    RC3PPS = 0x0F;   //RC3->MSSP1:SCL1;    
    RC1PPS = 0x06;   //RC1->CCP2:CCP2;    
    RC4PPS = 0x10;   //RC4->MSSP1:SDA1;    
    RC2PPS = 0x05;   //RC2->CCP1:CCP1;    
    RC6PPS = 0x09;   //RC6->EUSART1:TX1;    
    SSP1DATPPS = 0x14;   //RC4->MSSP1:SDA1;    
}
  
void PIN_MANAGER_IOC(void)
{   
	// interrupt on change for pin IOCAF5
    if(IOCAFbits.IOCAF5 == 1)
    {
        IOCAF5_ISR();  
    }	
	// interrupt on change for pin IOCAF6
    if(IOCAFbits.IOCAF6 == 1)
    {
        IOCAF6_ISR();  
    }	
	// interrupt on change for pin IOCAF7
    if(IOCAFbits.IOCAF7 == 1)
    {
        IOCAF7_ISR();  
    }	
	// interrupt on change for pin IOCCF0
    if(IOCCFbits.IOCCF0 == 1)
    {
        IOCCF0_ISR();  
    }	
}

/**
   IOCAF5 Interrupt Service Routine
*/
void IOCAF5_ISR(void) {

    // Add custom IOCAF5 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCAF5_InterruptHandler)
    {
        IOCAF5_InterruptHandler();
    }
    IOCAFbits.IOCAF5 = 0;
}

/**
  Allows selecting an interrupt handler for IOCAF5 at application runtime
*/
void IOCAF5_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCAF5_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCAF5
*/
void IOCAF5_DefaultInterruptHandler(void){
    // add your IOCAF5 interrupt custom code
    // or set custom function using IOCAF5_SetInterruptHandler()
    TMR1_StopTimer();
    //Periode_Moteur_G
    Vitesse_Moteur_G = 87000 /TMR1_ReadTimer();       //18000 
    if (PORTEbits.RE1 == 1)
    {   // sens de rotation positif
    }
    else
    {   // sens de rotation negatif  
        Vitesse_Moteur_G = -Vitesse_Moteur_G;  
    }
    TMR1_WriteTimer(0); 
    TMR1_StartTimer();
    // en theorie la ligne au dessus n'est pas necessaire mais si je ne la place
    // le code ne marche pas
}

/**
   IOCAF6 Interrupt Service Routine
*/
void IOCAF6_ISR(void) {

    // Add custom IOCAF6 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCAF6_InterruptHandler)
    {
        IOCAF6_InterruptHandler();
    }
    IOCAFbits.IOCAF6 = 0;
}

/**
  Allows selecting an interrupt handler for IOCAF6 at application runtime
*/
void IOCAF6_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCAF6_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCAF6
*/
void IOCAF6_DefaultInterruptHandler(void){
    // add your IOCAF6 interrupt custom code
    // or set custom function using IOCAF6_SetInterruptHandler()
    //LATDbits.LATD4 = !LATDbits.LATD4;
    __delay_ms(3);
    if (!PORTAbits.RA6)
    {
        if (PORTDbits.RD1 == 1)
        {   // sens de rotation positif
            selection ++;
        }
        else
        {   // sens de rotation negatif  
            selection --; 
        }
    }
    else
    {
        if (PORTDbits.RD1 == 1)
        {   // sens de rotation positif
            selection --;
        }
        else
        {   // sens de rotation negatif  
            selection ++; 
        }
    }
    __delay_ms(75);
}

/**
   IOCAF7 Interrupt Service Routine
*/
void IOCAF7_ISR(void) {

    // Add custom IOCAF7 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCAF7_InterruptHandler)
    {
        IOCAF7_InterruptHandler();
    }
    IOCAFbits.IOCAF7 = 0;
}

/**
  Allows selecting an interrupt handler for IOCAF7 at application runtime
*/
void IOCAF7_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCAF7_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCAF7
*/
void IOCAF7_DefaultInterruptHandler(void){
    // add your IOCAF7 interrupt custom code
    // or set custom function using IOCAF7_SetInterruptHandler()
}

/**
   IOCCF0 Interrupt Service Routine
*/
void IOCCF0_ISR(void) {

    // Add custom IOCCF0 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCCF0_InterruptHandler)
    {
        IOCCF0_InterruptHandler();
    }
    IOCCFbits.IOCCF0 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF0 at application runtime
*/
void IOCCF0_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCCF0_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF0
*/
void IOCCF0_DefaultInterruptHandler(void){
    // add your IOCCF0 interrupt custom code
    // or set custom function using IOCCF0_SetInterruptHandler()
    TMR3_StopTimer();
    //Periode_Moteur_D
    Vitesse_Moteur_D = 87000 /TMR3_ReadTimer();      //18000  
    if (PORTDbits.RD0 == 1)
    {   // sens de rotation positif
        Vitesse_Moteur_D = -Vitesse_Moteur_D;
    }
    else
    {   // sens de rotation negatif  
          
    }
    TMR3_WriteTimer(0); 
    TMR3_StartTimer();
    // en theorie la ligne au dessus n'est pas necessaire mais si je ne la place
    // le code ne marche pas
}

/**
 End of File
*/