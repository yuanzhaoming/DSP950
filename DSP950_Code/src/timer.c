#include "./include/timer.h"
#include "./include/ADDS_21489_EzKit.h"
#include <sysreg.h>
#include "./include/spi.h"

extern int g_input_type;

static void LED_FLASH_P13(int value)
{
	sysreg_bit_set(sysreg_FLAGS,  FLG0O );
	
	if(value == 0)
	{	
		sysreg_bit_clr(sysreg_FLAGS,  FLG0 );		 
	}
	else
	{	
		sysreg_bit_set(sysreg_FLAGS,  FLG0 );		
	}	
}
  

/*
*		2016-11-24 
*		定时器程序，主要是做判断程序是否在运行。
*/
static void timer_interrupt(int sig_int)
{
	static int P13Count = 0;
	 
    // Clear the interrupt manually to continue using the timer.
    *pTMSTAT = TIM0IRQ;

    LED_FLASH_P13(P13Count);    P13Count ++;
    P13Count = P13Count % 2;	 
    //printf("------timer interrupt----------\n");
}


/*
*		
*		2016-11-24 
*		定时器初始化。
*
*/
void TIMER0( void )
{
    interrupt(SIG_GPTMR0,timer_interrupt);
    /* Using PWM Out mode as a core timer */
    *pTM0CTL 	= TIMODEPWM|     /* PWM Out Mode */
                                PRDCNT | /* Count to end of period */
                                IRQEN;
	//pclk = 133MHZ, 0x8000000 * 7.5ns = 1.006632960
    *pTM0PRD 	= 0x4000000;        /* Timer 0 period = 0x8000 */ //500ms   
    *pTM0W 		= 1;               /* Timer 0 Pulse width = 1 */
    *pTMSTAT 	= TIM0EN;            /* enable timer 0 */	
}





