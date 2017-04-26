/*********************copyright wit_yuan qj 2016-10-09*********************/
#include "lcd1602.h"
#include <stm32f10x.h>
#include "rtconfig.h"
#include "rtthread.h"

#define NOP_TIME 10		   

#define EN_rcc                    		RCC_APB2Periph_GPIOC
#define EN_gpio                   		GPIOC
#define EN_pin                    		(GPIO_Pin_10)

#define RD_rcc                    		RCC_APB2Periph_GPIOC
#define RD_gpio                   		GPIOC
#define RD_pin                    		(GPIO_Pin_9)
	 
#define WR_rcc                    		RCC_APB2Periph_GPIOC
#define WR_gpio                   		GPIOC
#define WR_pin                    		(GPIO_Pin_8)


#define lcd_io_rcc1                    	RCC_APB2Periph_GPIOC
#define lcd_io_gpio1                   	GPIOC
#define lcd_io_pin1                    	(GPIO_Pin_0)

#define lcd_io_rcc2                   	RCC_APB2Periph_GPIOC
#define lcd_io_gpio2                   	GPIOC
#define lcd_io_pin2                    	(GPIO_Pin_1)

#define lcd_io_rcc3                    	RCC_APB2Periph_GPIOC
#define lcd_io_gpio3                   	GPIOC
#define lcd_io_pin3                    	(GPIO_Pin_2)

#define lcd_io_rcc4                    	RCC_APB2Periph_GPIOC
#define lcd_io_gpio4                   	GPIOC
#define lcd_io_pin4                    	(GPIO_Pin_3)

#define lcd_io_rcc5                    	RCC_APB2Periph_GPIOC
#define lcd_io_gpio5                   	GPIOC
#define lcd_io_pin5                    	(GPIO_Pin_4)

#define lcd_io_rcc6                    	RCC_APB2Periph_GPIOC
#define lcd_io_gpio6                   	GPIOC
#define lcd_io_pin6                    	(GPIO_Pin_5)

#define lcd_io_rcc7                    	RCC_APB2Periph_GPIOC
#define lcd_io_gpio7                   	GPIOC
#define lcd_io_pin7                    	(GPIO_Pin_6)

#define lcd_io_rcc8                    	RCC_APB2Periph_GPIOC
#define lcd_io_gpio8                   	GPIOC
#define lcd_io_pin8                    	(GPIO_Pin_7)

enum{
	LCD_CLR=0,
	LCD_SET,
}E_SET_RESET;


static void DelayMs(unsigned int i_delay)
{
 	int i = 0,j;
	for(j = 0 ; j < i_delay ; j ++)
		for(i = 0 ; i < 15000 ; i ++);
} 

static void DelayUs(unsigned int i_delay)
{
 	int i = 0,j;
	for(j = 0 ; j < i_delay ; j ++)
		for(i = 0 ; i < 5 ; i ++);
}

#define LCD_IO_BITS(name,value) \
do{\
	if(value==0)\
	{\
		GPIO_ResetBits(lcd_io_gpio##name, lcd_io_pin##name);\
	}\
	else\
	{\
		GPIO_SetBits  (lcd_io_gpio##name, lcd_io_pin##name);\
	}\
}while(0);

//引脚定义如下：1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
void rt_hw_lcd_init_output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(EN_rcc|RD_rcc|WR_rcc|\
							lcd_io_rcc1|lcd_io_rcc2|lcd_io_rcc3|lcd_io_rcc4|\
						    lcd_io_rcc5|lcd_io_rcc6|lcd_io_rcc7|lcd_io_rcc8,
							ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = EN_pin;
    GPIO_Init(EN_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = RD_pin;
    GPIO_Init(RD_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = WR_pin;
    GPIO_Init(WR_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin1;
    GPIO_Init(lcd_io_gpio1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin2;
    GPIO_Init(lcd_io_gpio2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin3;
    GPIO_Init(lcd_io_gpio3, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin4;
    GPIO_Init(lcd_io_gpio4, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin5;
    GPIO_Init(lcd_io_gpio5, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin6;
    GPIO_Init(lcd_io_gpio6, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin7;
    GPIO_Init(lcd_io_gpio7, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin8;
    GPIO_Init(lcd_io_gpio8, &GPIO_InitStructure);
}

void rt_hw_lcd_init_data_input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(
							lcd_io_rcc1|lcd_io_rcc2|lcd_io_rcc3|lcd_io_rcc4|\
						    lcd_io_rcc5|lcd_io_rcc6|lcd_io_rcc7|lcd_io_rcc8,
							ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin1;
    GPIO_Init(lcd_io_gpio1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin2;
    GPIO_Init(lcd_io_gpio2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin3;
    GPIO_Init(lcd_io_gpio3, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin4;
    GPIO_Init(lcd_io_gpio4, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin5;
    GPIO_Init(lcd_io_gpio5, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin6;
    GPIO_Init(lcd_io_gpio6, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin7;
    GPIO_Init(lcd_io_gpio7, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin8;
    GPIO_Init(lcd_io_gpio8, &GPIO_InitStructure);
}


void rt_hw_lcd_init_data_output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(
							lcd_io_rcc1|lcd_io_rcc2|lcd_io_rcc3|lcd_io_rcc4|\
						    lcd_io_rcc5|lcd_io_rcc6|lcd_io_rcc7|lcd_io_rcc8,
							ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin1;
    GPIO_Init(lcd_io_gpio1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin2;
    GPIO_Init(lcd_io_gpio2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin3;
    GPIO_Init(lcd_io_gpio3, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin4;
    GPIO_Init(lcd_io_gpio4, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin5;
    GPIO_Init(lcd_io_gpio5, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin6;
    GPIO_Init(lcd_io_gpio6, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin7;
    GPIO_Init(lcd_io_gpio7, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = lcd_io_pin8;
    GPIO_Init(lcd_io_gpio8, &GPIO_InitStructure);
}


void rt_hw_lcd_RD_set_reset(unsigned int i_value)
{
#if 0
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = RD_pin;
    GPIO_Init(RD_gpio, &GPIO_InitStructure);
#endif

    switch (i_value)
    {
    case LCD_CLR:
        GPIO_ResetBits(RD_gpio, RD_pin);
        break;
    case LCD_SET:
        GPIO_SetBits(RD_gpio, RD_pin);
        break;
	}
}

void rt_hw_lcd_WR_set_reset(unsigned int i_value)
{	  
#if 0  
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = WR_pin;
    GPIO_Init(WR_gpio, &GPIO_InitStructure);
#endif

    switch (i_value)
    {
    case LCD_CLR:
        GPIO_ResetBits(WR_gpio, WR_pin);
        break;
    case LCD_SET:
        GPIO_SetBits(WR_gpio, WR_pin);
        break;
	}
}

void rt_hw_lcd_EN_set_reset(unsigned int i_value)
{
//    GPIO_InitTypeDef GPIO_InitStructure;
//   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Pin   = EN_pin;
//    GPIO_Init(EN_gpio, &GPIO_InitStructure);

    switch (i_value)
    {
    case LCD_CLR:
        GPIO_ResetBits(EN_gpio, EN_pin);
        break;
    case LCD_SET:
        GPIO_SetBits(EN_gpio, EN_pin);
        break;
	}
}

//该值为一个8位的值，以此值来设置相应的值
void rt_hw_lcd_data_set_reset(unsigned int i_value)
{
	//GPIO1
	LCD_IO_BITS(1,(i_value&(1<<0))>>0);
	//GPIO2
	LCD_IO_BITS(2,(i_value&(1<<1))>>1);
	//GPIO3
	LCD_IO_BITS(3,(i_value&(1<<2))>>2);
	//GPIO4
	LCD_IO_BITS(4,(i_value&(1<<3))>>3);
	//GPIO5
	LCD_IO_BITS(5,(i_value&(1<<4))>>4);
	//GPIO6
	LCD_IO_BITS(6,(i_value&(1<<5))>>5);
	//GPIO7
	LCD_IO_BITS(7,(i_value&(1<<6))>>6);
	//GPIO8
	LCD_IO_BITS(8,(i_value&(1<<7))>>7);
}


unsigned char LCD_Check_Busy( void ) 
{ 
//	DataPort= 0xFF; 
	rt_hw_lcd_data_set_reset(0xFF);
	rt_hw_lcd_RD_set_reset(LCD_CLR); 
	rt_hw_lcd_WR_set_reset(LCD_SET); 
	rt_hw_lcd_EN_set_reset(LCD_CLR); 
	//_nop_(); 
	DelayUs(NOP_TIME);

	rt_hw_lcd_EN_set_reset(LCD_SET); 

	//设置端口状态为输入
	rt_hw_lcd_init_data_input();

	DelayUs(NOP_TIME);

	return GPIO_ReadInputDataBit(lcd_io_gpio8, lcd_io_pin8);
}

void LCD_Write_Com(unsigned char com) 
{  
	unsigned int iCount = 0;
	while( LCD_Check_Busy( ) )
	{
		iCount ++;
		rt_thread_delay( RT_TICK_PER_SECOND / 100);
		if(iCount == 10)
			break;
	}; //忙则等待

	//设置为输出状态
	rt_hw_lcd_init_data_output();

	rt_hw_lcd_RD_set_reset(LCD_CLR); 
	rt_hw_lcd_WR_set_reset(LCD_CLR); 
	rt_hw_lcd_EN_set_reset(LCD_SET);
//	DataPort= com; 
	rt_hw_lcd_data_set_reset(com);
	//_nop_(); 
	DelayUs(NOP_TIME);
	rt_hw_lcd_EN_set_reset(LCD_CLR);
}

void LCD_Write_Com_hard(unsigned char com) 
{  
	unsigned int iCount = 0;
	while( LCD_Check_Busy( ) )
	{
		iCount ++;
		if(iCount == 3000)
			break;
	}; //忙则等待

	//设置为输出状态
	rt_hw_lcd_init_data_output();

	rt_hw_lcd_RD_set_reset(LCD_CLR); 
	rt_hw_lcd_WR_set_reset(LCD_CLR); 
	rt_hw_lcd_EN_set_reset(LCD_SET);
//	DataPort= com; 
	rt_hw_lcd_data_set_reset(com);
	//_nop_(); 
	DelayUs(NOP_TIME);
	rt_hw_lcd_EN_set_reset(LCD_CLR);
}


void LCD_Write_Data(unsigned char Data) 
{ 
	unsigned int iCount = 0;
	while(LCD_Check_Busy())
	{
		iCount ++;
		if(iCount == 30)
		{
			rt_thread_delay( RT_TICK_PER_SECOND / 100);
			//printf("---busy---\r\n");
			break;
		}
	}; //忙则等待	   

	//设置数据为输出模式
	rt_hw_lcd_init_data_output();

	rt_hw_lcd_RD_set_reset(LCD_SET);
	rt_hw_lcd_WR_set_reset(LCD_CLR); 
	rt_hw_lcd_EN_set_reset(LCD_SET); 
	//DataPort= Data; 
	rt_hw_lcd_data_set_reset(Data);
	//_nop_();
	DelayUs(NOP_TIME);
	rt_hw_lcd_EN_set_reset(LCD_CLR);
}

void LCD_Write_Data_hard(unsigned char Data) 
{ 
	unsigned int iCount = 0;
	while(LCD_Check_Busy())
	{
		iCount ++;
		if(iCount == 3000)
		{
			break;
		}
	}; //忙则等待

	//设置数据为输出模式
	rt_hw_lcd_init_data_output();

	rt_hw_lcd_RD_set_reset(LCD_SET);
	rt_hw_lcd_WR_set_reset(LCD_CLR); 
	rt_hw_lcd_EN_set_reset(LCD_SET); 
	//DataPort= Data; 
	rt_hw_lcd_data_set_reset(Data);
	//_nop_();
	DelayUs(NOP_TIME);
	rt_hw_lcd_EN_set_reset(LCD_CLR);
}

void LCD_Clear(void) 
{ 
	LCD_Write_Com(0x01); 
	//DelayMs(5);
	DelayUs(NOP_TIME);
}

void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
{     
	if (y == 0) 
 	{     
	 	LCD_Write_Com(0x80 + x);     //表示第一行
 	}
 	else 
 	{      
 		LCD_Write_Com(0xC0 + x);      //表示第二行
 	}        
 	while (*s) 
 	{     
 		LCD_Write_Data( *s);     
 		s ++;     
 	}
}

void LCD_Write_String_hard(unsigned char x,unsigned char y,unsigned char *s) 
{     
	if (y == 0) 
 	{     
	 	LCD_Write_Com_hard(0x80 + x);     //表示第一行
 	}
 	else 
 	{      
 		LCD_Write_Com_hard(0xC0 + x);      //表示第二行
 	}        
 	while (*s) 
 	{     
 		LCD_Write_Data_hard( *s);     
 		s ++;     
 	}
}


void LCD_Write_Digital(unsigned char x,unsigned char y,unsigned char uData) 
{     
	unsigned char arr[3];
	unsigned char *p;
	arr[0] = uData/10+'0';
	arr[1] = uData%10+'0';
	arr[2] = 0;
	p =arr;

	if (y == 0) 
 	{     
	 	LCD_Write_Com(0x80 + x);     //表示第一行
 	}
 	else 
 	{      
 		LCD_Write_Com(0xC0 + x);      //表示第二行
 	}        
 	while (*p) 
 	{     
 		LCD_Write_Data( *p);     
 		p ++;     
 	}
}

void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
{     
	if (y == 0) 
 	{     
 		LCD_Write_Com(0x80 + x);     
 	}    
 	else 
 	{     
 		LCD_Write_Com(0xC0 + x);     
 	}        
	LCD_Write_Data( Data);  
}





void rt_hw_lcd1602_init(void) 
{
	//IO口初始化
	rt_hw_lcd_init_output();	

	LCD_Write_Com_hard(0x38);    /*显示模式设置*/ 
	DelayMs(5);
	 
	LCD_Write_Com_hard(0x38); 
	DelayMs(5); 
	
	LCD_Write_Com_hard(0x38); 
	DelayMs(5); 
	LCD_Write_Com_hard(0x38);  
	LCD_Write_Com_hard(0x08);    /*显示关闭*/ 
	LCD_Write_Com_hard(0x01);    /*显示清屏*/ 
	LCD_Write_Com_hard(0x06);    /*显示光标移动设置*/ 
	DelayMs(5); 
	LCD_Write_Com_hard(0x0C);    /*显示开及光标设置*/
}

void LCD_CLOSE()
{
	LCD_Write_Com(0x08);    /*关闭显示*/ 
}

void TestIO()
{
	rt_hw_lcd_init_output();
	rt_hw_lcd_data_set_reset(2);

}

/****************************end of file*********************************/

