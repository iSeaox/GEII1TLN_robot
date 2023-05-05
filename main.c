#include "mcc_generated_files/mcc.h"
#include "drivers/vl53l0x.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "drivers/LCD.h"
#include "mcc_generated_files/eusart1.h"
#include <math.h>
#include "commandes.h"

#define DIR_FORWARD 4
#define DIR_RIGHT 1
#define DIR_LEFT 2
#define DIR_STOP 3


void main(void)//__at 0x00200
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    LED(0, ON);
    LED(1, OFF);
    Motors(0);
    Speed_Motors( 0, 0 );
    initLCD();
    clearLCD();
    retroLCD(ON);
    __delay_ms(50);
    
    Speed_Motors(5, -5);
    
    uint32_t elapsed_100ms = 0;
    uint32_t elasped_rotate = 0;
    Motors(1);
    int error = 0;
    int lastError = 0;
    int integralError = 0;
    
    uint8_t leftCounter = 0;
    uint8_t rightCounter = 0;
    
    uint8_t lastDRight = 0;
    uint8_t lastDLeft = 0;
    
    uint8_t state = 0;
    
    uint8_t stateSequence[11] = {DIR_FORWARD, DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_LEFT, DIR_FORWARD, DIR_LEFT, DIR_LEFT, DIR_LEFT, DIR_RIGHT, DIR_STOP};
    int currentSeq = 0;
    while (1) {
        float coefP = - 9.5 / 200.0;
        float coefD =  -4.5 / 250;
        float coefI = 0;
        int vBase = 10;
        
        // --- Acquisition ---
        uint16_t aValCenter = ADCC_GetSingleConversion(Capteur_Analog_CNY_Centre);
        uint16_t aValRight = ADCC_GetSingleConversion(Capteur_Analog_CNY_Droite);
        uint16_t aValLeft = ADCC_GetSingleConversion(Capteur_Analog_CNY_Gauche);
        
        uint8_t cptDLeft = Digital_read(Capteur_Digital_CNY_Gauche);
        uint8_t cptDRight = Digital_read(Capteur_Digital_CNY_Droite);
        
        if(cptDLeft && !lastDLeft) {
            leftCounter++;
        }
        if(cptDRight && !lastDRight) {
            rightCounter++;
        }
        
        lastDRight = cptDRight;
        lastDLeft = cptDLeft;
        
        // --- Traitement ----
        if(cptDLeft && cptDRight && state == 0) {
            state = stateSequence[currentSeq];
            currentSeq++;
            elasped_rotate = millis();
        }
        
        if(!(-50 < aValLeft - aValRight && aValLeft - aValRight < 50 && aValCenter < 800)) {
            if(aValLeft < aValRight) {
                error = aValLeft - 2 * aValRight + aValCenter;
            }
            else {
                error = aValLeft * 2 - aValLeft - aValCenter;
            }
        }
        
        if(state == 0) {
            Speed_Motors((coefP * error + coefD * (error - lastError) + coefI * integralError) + 20, (coefP * error + coefD * (error - lastError) + + coefI * integralError) - 10 );
        }
        else if(state == 1) {
            Speed_Motors(10, 10);
            if(millis() - elasped_rotate > 200) {
                if(error < 10 && error > -10 && !(aValCenter < 300 && aValLeft < 300 && aValRight < 300)) {
                    state = 0;
                }
            }
        }
        else if(state == 2) {
            Speed_Motors(-10, -10);
            if(millis() - elasped_rotate > 200) {
                if(error < 10 && error > -10 && !(aValCenter < 300 && aValLeft < 300 && aValRight < 300)) {
                    state = 0;
                }
            }
        }
        else if(state == 3) {
            Motors(0);
            Buzzer(1);
            __delay_ms(250);
            Buzzer(0);
            __delay_ms(250);
        }
        else if(state == 4) {
            Speed_Motors((coefP * error + coefD * (error - lastError) + coefI * integralError) + 20, (coefP * error + coefD * (error - lastError) + + coefI * integralError) - 10 );
            if(millis() - elasped_rotate > 200) {
                state = 0;
            }
        }
        
        
        /*(millis() - elapsed_100ms > 10) {
            gotoLCD(0, 6);
            LCDvalue16fp(stateSequence[currentSeq], 0);
            gotoLCD(1, 6);
            LCDvalue16fp(currentSeq, 0);
            
            
            integralError += error;
            lastError = error;
            elapsed_100ms = millis();
        }*/
    }
    
}
    
    
