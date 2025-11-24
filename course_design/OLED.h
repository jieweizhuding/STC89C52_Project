/*	
******************************************************************************************
 *
 *		Init_OLED();									
 *		Oled_Clean();									
 *		Oled_DispString_6x8(0,Line_0,"  Main Meun");
 *		Oled_DispString_8x16(100,Line_1,"Hz");       	
 *		Oled_DispFloat_0V(80,Line_0,DCVoltage);      
 *		Oled_DispFloat_2V(80,Line_0,DCVoltage);
*/


#ifndef OLED_H_
#define OLED_H_

#include <REGX52.H>
sbit sda=P3^4;
sbit scl=P3^6;
//#define sda_out 	(P2DIR |= BIT2)
#define sda_1 (sda=1)
#define sda_0 (sda=0)

//#define scl_out 	(P2DIR |= BIT0)
#define scl_1 (scl=1)
#define scl_0 (scl=0)


#define Line_0		0
#define Line_1		2
#define Line_2		4
#define Line_3		6

extern void delay_ms(unsigned int ms_number);

extern void Init_OLED(void);
extern void Oled_Fill(unsigned char x);
#define Oled_Clean()	Oled_Fill(0)


extern void Oled_DispString_6x8(unsigned char Column, unsigned char Line,unsigned char *ch);

extern void Oled_DispNUM(unsigned char Column,unsigned char Line,unsigned char num);

extern void Oled_DispNUM_2(unsigned char Column,unsigned char Line,unsigned char num);

#endif /* OLED_H_ */
