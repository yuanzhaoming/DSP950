#include "./include/volume.h"
#include <def21489.h> 
#include <cdef21489.h>
#include <math.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>

int g_volume_count_value = 0x30;
float volumegain=0.00001;
 
/*
*
*		FLAG2 : volume down
*		FLAG3 : volume up
*/
void Volume_Pin_Init(void)
{
	//设置FLAG2的中断，以响应输出音量旋钮操作
	sysreg_bit_set(sysreg_MODE2, IRQ2E);			/* enable interrupt */
	asm("nop;nop;nop;nop;nop;nop;nop;nop;");
	*pSYSCTL |= IRQ2EN;								/* set FLAG2 to IRQ2 */		
}

void Register_Volume_Interrupt(void)
{
	interrupt(SIG_IRQ2, knobvolume_SIGIRQ2);		/* setup alert handler */	
}



void knobvolume_SIGIRQ2(int sig_int)
{
	unsigned int i_flag_value = sysreg_read(sysreg_FLAGS);
	int dsp_upload_volume_value = 0;

	/*
	if((i_flag_value & FLG3) == 0)
	{
		printf("---flag3----\n");	
	}
	if((i_flag_value & FLG2) == 0)
	{
		printf("---flag2----\n");	
	}
	*/	
#if 0	
	if((i_flag_value & FLG3) == 0)
	{
		g_volume_count_value = g_volume_count_value + 1;
		if(g_volume_count_value >= USE_VOLUME_STEP)
		{
			g_volume_count_value = USE_VOLUME_STEP;	
		}
		//printf("----up--%d---\n",g_volume_count_value);
	}
    else
	{
		g_volume_count_value = g_volume_count_value - 1;
		if(g_volume_count_value <= 0)
		{
			g_volume_count_value = 0;		
		}	
    		//printf("----down--%d---\n",g_volume_count_value);
	}
#else
	if((i_flag_value & FLG3) == 0)
	{
		if((i_flag_value & FLG2) == 0)
		{
			g_volume_count_value = g_volume_count_value + 1;
			if(g_volume_count_value >= USE_VOLUME_STEP)
			{
				g_volume_count_value = USE_VOLUME_STEP;	
			}
			//printf("----up--%d---\n",g_volume_count_value-96);
		}
	}	
	else
	{
		if((i_flag_value & FLG2) == 0)
		{
			g_volume_count_value = g_volume_count_value - 1;
			if(g_volume_count_value <= 0)
			{
				g_volume_count_value = 0;		
			}	
	    	//printf("----down--%d---\n",g_volume_count_value-96);
		}
		 
	}
#endif

	//volumegain = powf(10,(g_volume_count_value - USE_VOLUME_STEP)/20.0);
	//*pTXSPI = g_volume_count_value;
	//*pTXSPI = g_volume_count_value;

   	knobvolume = 1.0 * g_volume_count_value;
   	knobvolumegainvalue	 = powf(10.0, (knobvolume - USE_VOLUME_STEP) / 20);
	dsp_upload_volume_value = (0<<30) | (1<<26) | (8 << 21) | (0 << 16) | (g_volume_count_value << 8) | ( 0 << 0); 
	
	*pTXSPI = dsp_upload_volume_value;
}

void Volume_Init(void)
{
	Volume_Pin_Init();	
	Register_Volume_Interrupt();
}


