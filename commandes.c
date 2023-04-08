#include "Commandes.h"
#include <xc.h>
#include <stdint.h>
#include "drivers/LCD.h"

int16_t Consigne_Vitesse_Moteur_G;
int16_t Consigne_Vitesse_Moteur_D;

uint32_t millis(void)
{
    return (millisec_32bits);
}


void LCDvalue8fp(uint8_t v, uint8_t fp)
{
uint8_t i; 
	writeLCD(0x04, 0);		//	COMMAND		//now cursor move left
    writeLCD(0x30+v%10, 1);
    for(i = 1; i<=fp || v>=10 ; i++)
    {
        if(i == fp )
            {
                writeLCD('.', 1);
            }
        v/=10;
        writeLCD(0x30+v%10, 1);
    }
    while ( i<3 )
    {
        writeLCD(0x30, 1); i++;
    }
	writeLCD(6 , 0);					//restore normal cursor behavior
}

void LCDvalue16fp(uint16_t v, uint8_t fp)
{
uint8_t i;  
	writeLCD(0x04, 0);					//now cursor move left
    writeLCD(0x30+v%10, 1);
    for(i = 1; i<=fp || v>=10 ; i++)
    {
        if(i == fp )
            {
                writeLCD('.', 1);
            }    
        v/=10;
        writeLCD(0x30+v%10, 1);
    }
    while ( i<5 )
    {
        writeLCD(0x30, 1); i++;
    }
	writeLCD(6 , 0);					//restore normal cursor behavior
}

void LCDvalue_laser_Range(uint16_t v)
{
uint8_t i;  
	writeLCD(0x04, 0);					//now cursor move left
    if (v > 400) printLCD("XAM");
    else if (v == 0)   printLCD("---");
    else
    {
        writeLCD(0x30+v%10, 1);
        for(i = 1; v>=10 ; i++)
        {   
            v/=10;
            writeLCD(0x30+v%10, 1);
        }
        while ( i<3 )
        {
            writeLCD(0x30, 1); i++;
        }        
    }
	writeLCD(6 , 0);					//restore normal cursor behavior
}

void LCDvalue32(uint32_t v)
{
uint8_t i;  
	writeLCD(0x04, 0);					//now cursor move left
    writeLCD(0x30+v%10, 1);
    for(i = 1; v>=10 ; i++)
    {  
        v/=10;
        writeLCD(0x30+v%10, 1);
    }
    while ( i<10 )
    {
        writeLCD(0x30, 1); i++;
    }
	writeLCD(6 , 0);					//restore normal cursor behavior
}

uint8_t Digital_read(uint8_t io)
{
    switch (io)
    {
        case Capteur_Digital_CNY_Gauche :
            return (PORTBbits.RB0);
            break;
        case Capteur_Digital_CNY_Droite :
            return (PORTBbits.RB1);
            break;  
        case Micro_Digital :
            return (PORTBbits.RB2);
            break;      
    }
}

void LCDvalue_Boolean(uint8_t v)
{
uint8_t i; 
    if(v)
            {
                writeLCD('1', 1);
            }
    else
            {
                writeLCD('0', 1);
            }    
}

void Motors(uint8_t on_off)
{
    if(on_off)
            {
                LATDbits.LATD3 = 1;  // Prise des moteurs
            }
    else
            {
                LATDbits.LATD3 = 0; // Relachement des moteurs
            }    
}

void Buzzer(uint8_t on_off)
{
    if(on_off)
            {
                LATBbits.LATB4 = 1;  // Buzzer on
            }
    else
            {
                LATBbits.LATB4 = 0;  // Buzzer oFF
            }    
}

void LED(uint8_t io, uint8_t on_off)
{
    switch (io)
    {
        case Led_0  :   if(on_off)
                            {
                                LATDbits.LATD4 = 1;  // led on
                            }
                        else
                            {
                                LATDbits.LATD4 = 0;  // led oFF
                            }   
            break;
        case Led_1  :   if(on_off)
                            {
                                LATDbits.LATD2 = 1;  // led on
                            }
                        else
                            {
                                LATDbits.LATD2 = 0;  // led oFF
                            }   
            break;      
    }
}

uint16_t Valeur_selection(void)
{
    return (selection);
}

void Ecrit_Valeur_selection(uint16_t Val_sel)
{
    selection = Val_sel;
}

uint8_t BP_selection(void)
{
    return (PORTAbits.RA7);
}

void Speed_Motors(int16_t Cons_Vit_Mot_G, int16_t Cons_Vit_Mot_D )
{
    Consigne_Vitesse_Moteur_G = Cons_Vit_Mot_G;
    Consigne_Vitesse_Moteur_D = Cons_Vit_Mot_D;   
}

void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData) //commencer à écrire 0x0010
{
    NVMADRH = (uint8_t)((bAdd >> 8) & 0x03);
    NVMADRL = (uint8_t)(bAdd & 0xFF);
    NVMDAT = bData;
    NVMCON1bits.NVMREG = 0;
    NVMCON1bits.WREN = 1;
    GIE = 0;                        
    NVMCON2 = 0x55;
    NVMCON2 = 0xAA;
    NVMCON1bits.WR = 1;
//    while (NVMCON1bits.WR);                        // Wait for write to complete

//    NVMCON1bits.WREN = 0;
    GIE = 1;               
}

uint8_t DATAEE_ReadByte(uint16_t bAdd)
{
    NVMADRH = (uint8_t)((bAdd >> 8) & 0x03);
    NVMADRL = (uint8_t)(bAdd & 0xFF);
    NVMCON1bits.NVMREG = 0;
    NVMCON1bits.RD = 1;
    NOP();                    // NOPs maybe required for latency at high freq
    NOP();
    return (NVMDAT);
}