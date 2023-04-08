// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __Commandes_H_
#define	__Commandes_H_

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>
#define _XTAL_FREQ 64000000

#define Capteur_Analog_CNY_Gauche  31     //RD7
#define Capteur_Analog_CNY_Centre  30     //RD6
#define Capteur_Analog_CNY_Droite  29     //RD5

#define Capteur_Digital_CNY_Gauche  0   //PORTBbits.RB0
#define Capteur_Digital_CNY_Droite  1   //PORTBbits.RB1
#define Micro_Digital               2   //PORTBbits.RB2

#define ON                          1
#define OFF                         0

#define Led_0                       0 
#define Led_1                       1

uint32_t millisec_32bits = 0;
extern uint16_t selection;

// definitions des prototypes
// mesure de temps
uint32_t millis(void);
// LCD
void LCDvalue8fp(uint8_t v, uint8_t fp);
void LCDvalue16fp(uint16_t v, uint8_t fp);
void LCDvalue_laser_Range(uint16_t v);
void LCDvalue32(uint32_t v);
uint8_t Digital_read(uint8_t io);
void LCDvalue_Boolean(uint8_t v);

void Motors(uint8_t on_off);
void Buzzer(uint8_t on_off);
void LED(uint8_t io, uint8_t on_off);
uint16_t Valeur_selection(void);
void Ecrit_Valeur_selection(uint16_t Val_sel);
uint8_t BP_selection(void);
void Speed_Motors(int16_t Cons_Vit_Mot_G, int16_t Cons_Vit_Mot_D );

void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData); //commencer à écrire 0x0010
uint8_t DATAEE_ReadByte(uint16_t bAdd);


#endif