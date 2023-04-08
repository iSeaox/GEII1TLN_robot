/**
  TMR4 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    tmr4.c

  @Summary
    This is the generated driver implementation file for the TMR4 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for TMR4.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC18F47K40
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above 
        MPLAB 	          :  MPLAB X 5.20
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "tmr4.h"
#include "pwm1.h"
#include "pwm2.h"

int16_t Vitesse_Moteur_G;
int16_t Vitesse_Moteur_D;

extern int16_t Consigne_Vitesse_Moteur_G;
int16_t Erreur_Vitesse_Moteur_G;

// preinitialisation à 400 pour éviter la prise des asservissement au départ
// Sauts au démarrage.
int16_t Sigma_Erreur_Vitesse_Moteur_G = 400;
int16_t Ancienne_Erreur_Vitesse_Moteur_G = 0;
//extern int16_t Passage;

extern int16_t Consigne_Vitesse_Moteur_D;
int16_t Erreur_Vitesse_Moteur_D;

// preinitialisation à 400 pour éviter la prise des asservissement au départ
// Sauts au démarrage.
int16_t Sigma_Erreur_Vitesse_Moteur_D = 400;
int16_t Ancienne_Erreur_Vitesse_Moteur_D = 0;

int16_t dutyValue_G;
int16_t dutyValue_D;

int16_t ancien_dutyValue_G = 400;
int16_t ancien_dutyValue_D = 400;

extern int16_t Consigne_Vitesse_Moteur_G_interne;
extern int16_t Consigne_Vitesse_Moteur_D_interne;


/**
  Section: Global Variables Definitions
*/

void (*TMR4_InterruptHandler)(void);

/**
  Section: TMR4 APIs
*/

void TMR4_Initialize(void)
{
    // Set TMR4 to the options selected in the User Interface

    // T4CS LFINTOSC; 
    T4CLKCON = 0x04;

    // T4PSYNC Not Synchronized; T4MODE Software control; T4CKPOL Rising Edge; T4CKSYNC Not Synchronized; 
    T4HLT = 0x00;

    // T4RSEL T4CKIPPS pin; 
    T4RST = 0x00;

    // PR4 77; 
    T4PR = 0x4D;

    // TMR4 0; 
    T4TMR = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR4bits.TMR4IF = 0;

    // Enabling TMR4 interrupt.
    PIE4bits.TMR4IE = 1;

    // Set Default Interrupt Handler
    TMR4_SetInterruptHandler(TMR4_DefaultInterruptHandler);

    // T4CKPS 1:4; T4OUTPS 1:1; TMR4ON on; 
    T4CON = 0xA0;
}

void TMR4_ModeSet(TMR4_HLT_MODE mode)
{
   // Configure different types HLT mode
    T4HLTbits.MODE = mode;
}

void TMR4_ExtResetSourceSet(TMR4_HLT_EXT_RESET_SOURCE reset)
{
    //Configure different types of HLT external reset source
    T4RSTbits.RSEL = reset;
}

void TMR4_Start(void)
{
    // Start the Timer by writing to TMRxON bit
    T4CONbits.TMR4ON = 1;
}

void TMR4_StartTimer(void)
{
    TMR4_Start();
}

void TMR4_Stop(void)
{
    // Stop the Timer by writing to TMRxON bit
    T4CONbits.TMR4ON = 0;
}

void TMR4_StopTimer(void)
{
    TMR4_Stop();
}

uint8_t TMR4_Counter8BitGet(void)
{
    uint8_t readVal;

    readVal = TMR4;

    return readVal;
}

uint8_t TMR4_ReadTimer(void)
{
    return TMR4_Counter8BitGet();
}

void TMR4_Counter8BitSet(uint8_t timerVal)
{
    // Write to the Timer4 register
    TMR4 = timerVal;
}

void TMR4_WriteTimer(uint8_t timerVal)
{
    TMR4_Counter8BitSet(timerVal);
}

void TMR4_Period8BitSet(uint8_t periodVal)
{
   PR4 = periodVal;
}

void TMR4_LoadPeriodRegister(uint8_t periodVal)
{
   TMR4_Period8BitSet(periodVal);
}

void TMR4_ISR(void)
{

    // clear the TMR4 interrupt flag
    PIR4bits.TMR4IF = 0;

    if(TMR4_InterruptHandler)
    {
        TMR4_InterruptHandler();
    }
}


void TMR4_SetInterruptHandler(void (* InterruptHandler)(void)){
    TMR4_InterruptHandler = InterruptHandler;
}

//char kpmd=30; //40    
//char kimd=5; //24
//char kpmg=30;    
//char kimg=5;

float R0;
float R1;

//#define Kpm     30
//#define Te      0.001
//#define Tau     0.07
//#define R0      Kpm * Te / (2 * Tau ) + Kpm
//#define R1      Kpm * Te / (2 * Tau ) - Kpm
#define R0      20.2
#define R1      -19.8

float DMG;
float DMD;

void TMR4_DefaultInterruptHandler(void){
    // add your TMR4 interrupt custom code
    // or set custom function using TMR4_SetInterruptHandler()
       //Consigne_Vitesse_Moteur_G = -5;
    /*   Erreur_Vitesse_Moteur_G = Consigne_Vitesse_Moteur_G - Vitesse_Moteur_G;  //87000
       Sigma_Erreur_Vitesse_Moteur_G = Sigma_Erreur_Vitesse_Moteur_G + Erreur_Vitesse_Moteur_G;
       if (Sigma_Erreur_Vitesse_Moteur_G > 255)  
            {Sigma_Erreur_Vitesse_Moteur_G = 255;}
       else if (Sigma_Erreur_Vitesse_Moteur_G < -255)
            {Sigma_Erreur_Vitesse_Moteur_G = -255;}
       dutyValue_G = (Erreur_Vitesse_Moteur_G *Kpm) +(Sigma_Erreur_Vitesse_Moteur_G * Kim);
       //if (dutyValue_G<400) {dutyValue_G=400;}
       
       //Consigne_Vitesse_Moteur_D =5;
       Erreur_Vitesse_Moteur_D = Consigne_Vitesse_Moteur_D - Vitesse_Moteur_D;
       Sigma_Erreur_Vitesse_Moteur_D = Sigma_Erreur_Vitesse_Moteur_D + Erreur_Vitesse_Moteur_D;
       if (Sigma_Erreur_Vitesse_Moteur_D > 255)  
            {Sigma_Erreur_Vitesse_Moteur_D = 255;}
       else if (Sigma_Erreur_Vitesse_Moteur_D < -255) 
            {Sigma_Erreur_Vitesse_Moteur_D = -255;}
       dutyValue_D = (Erreur_Vitesse_Moteur_D *Kpm) +(Sigma_Erreur_Vitesse_Moteur_D * Kim);
       //if (dutyValue_D>400) {dutyValue_D=400;}
 
       PWM2_LoadDutyValue( dutyValue_G ); //
       PWM1_LoadDutyValue( dutyValue_D ); //
     */  
       //Consigne_Vitesse_Moteur_G =20;
       //Vitesse_Moteur_G = 3;
       // deuxieme version de correcteur synthese par euler
    
    
    
       Erreur_Vitesse_Moteur_G = Consigne_Vitesse_Moteur_G - Vitesse_Moteur_G;
       dutyValue_G = (float)ancien_dutyValue_G + R0 * (float) Erreur_Vitesse_Moteur_G + R1 * (float) Ancienne_Erreur_Vitesse_Moteur_G;
       Ancienne_Erreur_Vitesse_Moteur_G = Erreur_Vitesse_Moteur_G;
       
       if (dutyValue_G > 800)  
            {dutyValue_G = 800;}
       else if (dutyValue_G < 0) 
            {dutyValue_G = 0;}
       ancien_dutyValue_G = dutyValue_G;
   
       Erreur_Vitesse_Moteur_D = Consigne_Vitesse_Moteur_D - Vitesse_Moteur_D;
       dutyValue_D = (float) ancien_dutyValue_D + R0 * (float) Erreur_Vitesse_Moteur_D + R1 * (float) Ancienne_Erreur_Vitesse_Moteur_D;
       Ancienne_Erreur_Vitesse_Moteur_D = Erreur_Vitesse_Moteur_D;
       if (dutyValue_D > 800)  
            {dutyValue_D = 800;}
       else if (dutyValue_D < 0) 
            {dutyValue_D = 0;}
       ancien_dutyValue_D = dutyValue_D;
       
       PWM2_LoadDutyValue(  (int16_t) dutyValue_G );
       PWM1_LoadDutyValue(  (int16_t) dutyValue_D );    
       
      
       
       //PWM2_LoadDutyValue(  570 );
       //PWM1_LoadDutyValue(  570 );       
  
    /*Consigne_Vitesse_Moteur_G_interne = Consigne_Vitesse_Moteur_G;
    Consigne_Vitesse_Moteur_D_interne = Consigne_Vitesse_Moteur_D;
    
    Erreur_Vitesse_Moteur_G = Consigne_Vitesse_Moteur_G_interne - Vitesse_Moteur_G;
    Sigma_Erreur_Vitesse_Moteur_G = Sigma_Erreur_Vitesse_Moteur_G + Erreur_Vitesse_Moteur_G;
    dutyValue_G = Erreur_Vitesse_Moteur_G * 4 + Sigma_Erreur_Vitesse_Moteur_G;   // avec 20 ms 
    

    Erreur_Vitesse_Moteur_D = Consigne_Vitesse_Moteur_D_interne - Vitesse_Moteur_D;
    Sigma_Erreur_Vitesse_Moteur_D = Sigma_Erreur_Vitesse_Moteur_D + Erreur_Vitesse_Moteur_D;
    dutyValue_D = Erreur_Vitesse_Moteur_D * 4 + Sigma_Erreur_Vitesse_Moteur_D;  // avec 20 ms
    
    Ancienne_Erreur_Vitesse_Moteur_G = Erreur_Vitesse_Moteur_G;
    Ancienne_Erreur_Vitesse_Moteur_D = Erreur_Vitesse_Moteur_D;
    
    PWM2_LoadDutyValue( dutyValue_G );
    PWM1_LoadDutyValue( dutyValue_D );*/
}

/**
  End of File
*/