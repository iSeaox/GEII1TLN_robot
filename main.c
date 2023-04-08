#include "mcc_generated_files/mcc.h"
#include "drivers/vl53l0x.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "drivers/LCD.h"
#include "mcc_generated_files/eusart1.h"
#include <math.h>
#include "commandes.h"


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
//    gotoLCD(0,0); printLCD("     HELLO      ");
//    gotoLCD(1,0); printLCD("PROGRAMME MOI !!");
    
    Speed_Motors(5, -5);
    
    uint32_t elapsed_100ms = 0;
    Motors(1);
    int error = 0;
    int lastError = 0;
    int integralError = 0;
    while (1) {
        float coefP = - 7 / 200.0;
        float coefD =  3.5 / 250.0;
        float coefI = 0;
        int vBase = 10;
        uint16_t aValCenter = ADCC_GetSingleConversion(Capteur_Analog_CNY_Centre);
        uint16_t aValRight = ADCC_GetSingleConversion(Capteur_Analog_CNY_Droite);
        uint16_t aValLeft = ADCC_GetSingleConversion(Capteur_Analog_CNY_Gauche);
        if(!(-50 < aValLeft - aValRight && aValLeft - aValRight < 50 && aValCenter < 800)) {
            if(aValLeft - aValRight < 0) {
                error = aValLeft - 2 * aValRight + aValCenter;
            }
            else {
                error = aValLeft * 2 - aValLeft - aValCenter;
            }
        }
        
        Speed_Motors((coefP * error + coefD * (error - lastError) + coefI * integralError) + 20, (coefP * error + coefD * (error - lastError) + + coefI * integralError) - 10 );
        
        
        if(millis() - elapsed_100ms > 10) {
            integralError += error;
            lastError = error;
            elapsed_100ms = millis();
        }
//        
//        if(aValCenter > 800) {
//            Motors(0);
//            if(aValLeft < 800) {
//                Speed_Motors(5, -20);
//            }else if(aValRight < 800) {
//                Speed_Motors(20, -5);
//            }
//            else {
//                Speed_Motors(10, -10);
//            }
//        }
    }
    
}
    
    
