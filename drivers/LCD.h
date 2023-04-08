// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __LCD_H_
#define	__LCD_H_

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>
#define _XTAL_FREQ 64000000

typedef enum{
    FIRST_LINE,
    SECOND_LINE,     
}LCD_POSITION;

typedef enum{
    COMMAND,
    DATA,
}LCD_REGISTER_TYPE;

extern void initLCD();
extern void clearLCD();
extern void writeLCD(uint8_t byte, LCD_REGISTER_TYPE type);
extern void gotoLCD(char ligne, char colonne);
extern void printLCD(char *t);
void        retroLCD(uint8_t on_off);

#endif