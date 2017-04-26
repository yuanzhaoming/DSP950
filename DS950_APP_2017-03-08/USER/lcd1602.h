/*********************copyright wit_yuan qj 2016-10-09*********************/
#ifndef _lcd1602_h_
#define _lcd1602_h_

extern void LCD_Init(void);

extern void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s);


extern void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data);

extern void LCD_Clear(void);
extern void LCD_CLOSE(void);

extern void LCD_Write_Digital(unsigned char x,unsigned char y,unsigned char uData);

extern void rt_hw_lcd1602_init(void);

extern void LCD_Write_String_hard(unsigned char x,unsigned char y,unsigned char *s);

#endif
/****************************end of file*********************************/


