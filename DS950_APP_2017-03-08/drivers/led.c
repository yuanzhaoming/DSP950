/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */	  
#include <rtthread.h>
#include <stm32f10x.h>
#include "led.h"
			  
// led define
#ifdef STM32_SIMULATOR
#define led1_rcc                    RCC_APB2Periph_GPIOA
#define led1_gpio                   GPIOA
#define led1_pin                    (GPIO_Pin_2)

#define led2_rcc                    RCC_APB2Periph_GPIOA
#define led2_gpio                   GPIOA
#define led2_pin                    (GPIO_Pin_2)

#else

#define led1_rcc                    RCC_APB2Periph_GPIOA
#define led1_gpio                   GPIOA
#define led1_pin                    (GPIO_Pin_1)

#define led2_rcc                    RCC_APB2Periph_GPIOA
#define led2_gpio                   GPIOA
#define led2_pin                    (GPIO_Pin_2)

#define led3_rcc                    RCC_APB2Periph_GPIOA
#define led3_gpio                   GPIOA
#define led3_pin                    (GPIO_Pin_3)

#define led4_rcc                    RCC_APB2Periph_GPIOB
#define led4_gpio                   GPIOB
#define led4_pin                    (GPIO_Pin_0)

#define led5_rcc                    RCC_APB2Periph_GPIOB
#define led5_gpio                   GPIOB
#define led5_pin                    (GPIO_Pin_1)

#define led6_rcc                    RCC_APB2Periph_GPIOA
#define led6_gpio                   GPIOA
#define led6_pin                    (GPIO_Pin_8)

#define led7_rcc                    RCC_APB2Periph_GPIOA
#define led7_gpio                   GPIOA
#define led7_pin                    (GPIO_Pin_11)

#endif // led define #ifdef STM32_SIMULATOR

static int g_led_init = 0;

void rt_hw_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	if(g_led_init == 1)
		return;
	g_led_init = 1;

    RCC_APB2PeriphClockCmd(led1_rcc|led2_rcc|led3_rcc|led4_rcc|led5_rcc|led6_rcc|led7_rcc ,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = led1_pin;
    GPIO_Init(led1_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led2_pin;
    GPIO_Init(led2_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led3_pin;
    GPIO_Init(led3_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led4_pin;
    GPIO_Init(led4_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led5_pin;
    GPIO_Init(led5_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led6_pin;
    GPIO_Init(led6_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led7_pin;
    GPIO_Init(led7_gpio, &GPIO_InitStructure);




	//初次启动灯不会全部先亮了
	GPIO_SetBits(led1_gpio, led1_pin);
	GPIO_SetBits(led2_gpio, led2_pin);
	GPIO_SetBits(led3_gpio, led3_pin);
	GPIO_SetBits(led4_gpio, led4_pin);
	GPIO_SetBits(led5_gpio, led5_pin);
	GPIO_SetBits(led6_gpio, led6_pin);
	GPIO_SetBits(led7_gpio, led7_pin);

	//GPIO_ResetBits(led1_gpio, led1_pin); //工作
	//GPIO_ResetBits(led2_gpio, led2_pin); //AES
	//GPIO_ResetBits(led3_gpio, led3_pin);//光纤
	//GPIO_ResetBits(led4_gpio, led4_pin); //同轴
	//GPIO_ResetBits(led5_gpio, led5_pin);//DVD
	//GPIO_ResetBits(led6_gpio, led6_pin);//模拟
	//while(1);

}

void rt_hw_led_on(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
		//GPIO_ResetBits(led6_gpio, led6_pin);
		GPIO_ResetBits(led2_gpio, led2_pin);
        break;
    case 1:
		//GPIO_ResetBits(led2_gpio, led2_pin);
		GPIO_ResetBits(led3_gpio, led3_pin);
        break;
    case 2:
		//GPIO_ResetBits(led5_gpio, led5_pin);
		GPIO_ResetBits(led4_gpio, led4_pin);
        break;
    case 3:
		//GPIO_ResetBits(led3_gpio, led3_pin);
		GPIO_ResetBits(led5_gpio, led5_pin);
        break;
    case 4:
		//GPIO_ResetBits(led4_gpio, led4_pin);
		GPIO_ResetBits(led6_gpio, led6_pin);
        break;
    case 5:
		GPIO_ResetBits(led7_gpio, led7_pin);
        break;
    case 6:
		GPIO_ResetBits(led1_gpio, led1_pin);
        break;
    default:
        break;
    }
}

void rt_hw_led_off(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
		GPIO_SetBits(led2_gpio, led2_pin);
        break;
    case 1:
		GPIO_SetBits(led3_gpio, led3_pin);	
        break;
    case 2:
		GPIO_SetBits(led4_gpio, led4_pin);
        break;
    case 3:
		GPIO_SetBits(led5_gpio, led5_pin);
        break;
    case 4:
		GPIO_SetBits(led6_gpio, led6_pin);
        break;
    case 5:
		GPIO_SetBits(led7_gpio, led7_pin);
        break;
    case 6:
        GPIO_SetBits(led1_gpio, led1_pin);
        break;
    default:
        break;
    }
}

void rt_hw_led_all_on( )
{
	rt_hw_led_on(0);
	rt_hw_led_on(1);
	rt_hw_led_on(2);
	rt_hw_led_on(3);
	rt_hw_led_on(4);
	rt_hw_led_on(5);
}


void rt_hw_led_mutex(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
        rt_hw_led_on(0);
		rt_hw_led_off(1);
		rt_hw_led_off(2);
		rt_hw_led_off(3);
		rt_hw_led_off(4);
		rt_hw_led_off(5);
        break;
    case 1:
        rt_hw_led_off(0);
		rt_hw_led_on(1);
		rt_hw_led_off(2);
		rt_hw_led_off(3);
		rt_hw_led_off(4);
		rt_hw_led_off(5);
        break;
    case 2:
        rt_hw_led_off(0);
		rt_hw_led_off(1);
		rt_hw_led_on(2);
		rt_hw_led_off(3);
		rt_hw_led_off(4);
		rt_hw_led_off(5);
        break;
    case 3:
        rt_hw_led_off(0);
		rt_hw_led_off(1);
		rt_hw_led_off(2);
		rt_hw_led_on(3);
		rt_hw_led_off(4);
		rt_hw_led_off(5);
        break;
    case 4:
        rt_hw_led_off(0);
		rt_hw_led_off(1);
		rt_hw_led_off(2);
		rt_hw_led_off(3);
		rt_hw_led_on(4);
		rt_hw_led_off(5);
        break;
    case 5:
        rt_hw_led_off(0);
		rt_hw_led_off(1);
		rt_hw_led_off(2);
		rt_hw_led_off(3);
		rt_hw_led_off(4);
		rt_hw_led_on(5);
        break;
    default:
        break;
    }
}



#ifdef RT_USING_FINSH
#include <finsh.h>
static rt_uint8_t led_inited = 0;

void led(rt_uint32_t led, rt_uint32_t value)
{
    /* init led configuration if it's not inited. */
    if (!led_inited)
    {
        rt_hw_led_init();
        led_inited = 1;
    }

    if ( led == 0 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(0);
            break;
        case 1:
            rt_hw_led_on(0);
            break;
        default:
            break;
        }
    }

    if ( led == 1 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(1);
            break;
        case 1:
            rt_hw_led_on(1);
            break;
        default:
            break;
        }
    }

    if ( led == 5 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(5);
            break;
        case 1:
            rt_hw_led_on(5);
            break;
        default:
            break;
        }
    }
}
//FINSH_FUNCTION_EXPORT(led, set led[0 - 1] on[1] or off[0].)
#endif

