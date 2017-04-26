#include "watchdog.h"
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////
//			 	函数名		:		Wdt_Init		
//			   	功能		:		看门狗硬件初始化
//				参数		:		void
//				作者		:		wit_yuan
//				时间		:		2016-12-17
////////////////////////////////////////////////////////////////////////////
void Wdt_Init(void)
{
// 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
// 	/* 独立看门狗时钟分频,40K/256=156HZ(6.4ms)*/ 
// 	IWDG_SetPrescaler(IWDG_Prescaler_256);  
// 	/* 喂狗时间 1s/6.4MS=120 .注意不能大于0xfff*/
// 	IWDG_SetReload(10);
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
 	/* 独立看门狗时钟分频,40K/256=156HZ(6.4ms)*/ 
 	IWDG_SetPrescaler(IWDG_Prescaler_256);  
 	/* 喂狗时间 1s/6.4MS=120 .注意不能大于0xfff*/
 	IWDG_SetReload(120 * 2); //2s的看门狗
	IWDG_ReloadCounter(); 
//	IWDG_SetReload(120);//4s重启 //1s重启          
 	IWDG_Enable();   
}  

 //////////////////////////////////////////////////////////////////////////
//			 	函数名		:		Kick_Dog		
//			   	功能		:		手动喂狗
//				参数		:		void
//				作者		:		wit_yuan
//				时间		:		2016-12-17
////////////////////////////////////////////////////////////////////////////
void Kick_Dog(void)
{ 
    IWDG_ReloadCounter();      
}


