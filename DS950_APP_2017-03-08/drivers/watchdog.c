#include "watchdog.h"
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////
//			 	������		:		Wdt_Init		
//			   	����		:		���Ź�Ӳ����ʼ��
//				����		:		void
//				����		:		wit_yuan
//				ʱ��		:		2016-12-17
////////////////////////////////////////////////////////////////////////////
void Wdt_Init(void)
{
// 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
// 	/* �������Ź�ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/ 
// 	IWDG_SetPrescaler(IWDG_Prescaler_256);  
// 	/* ι��ʱ�� 1s/6.4MS=120 .ע�ⲻ�ܴ���0xfff*/
// 	IWDG_SetReload(10);
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
 	/* �������Ź�ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/ 
 	IWDG_SetPrescaler(IWDG_Prescaler_256);  
 	/* ι��ʱ�� 1s/6.4MS=120 .ע�ⲻ�ܴ���0xfff*/
 	IWDG_SetReload(120 * 2); //2s�Ŀ��Ź�
	IWDG_ReloadCounter(); 
//	IWDG_SetReload(120);//4s���� //1s����          
 	IWDG_Enable();   
}  

 //////////////////////////////////////////////////////////////////////////
//			 	������		:		Kick_Dog		
//			   	����		:		�ֶ�ι��
//				����		:		void
//				����		:		wit_yuan
//				ʱ��		:		2016-12-17
////////////////////////////////////////////////////////////////////////////
void Kick_Dog(void)
{ 
    IWDG_ReloadCounter();      
}


