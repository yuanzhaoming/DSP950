#include "dsp_reset.h"

#define dsp_reset_rcc                    RCC_APB2Periph_GPIOC
#define dsp_reset_gpio                   GPIOC
#define dsp_reset_pin                    (GPIO_Pin_11)

#include <rtthread.h>
#include <stm32f10x.h>


void rt_hw_dsp_reset_init(void)
{								 
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(dsp_reset_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = dsp_reset_pin;
    GPIO_Init(dsp_reset_gpio, &GPIO_InitStructure);

	GPIO_ResetBits(dsp_reset_gpio, dsp_reset_pin);
}   


void rt_hw_dsp_reset_on( )
{
   GPIO_ResetBits(dsp_reset_gpio, dsp_reset_pin);
}


void rt_hw_dsp_reset_off( )
{
   GPIO_SetBits(dsp_reset_gpio, dsp_reset_pin);
}



