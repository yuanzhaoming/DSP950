/******************** (C) COPYRIGHT 2016***************************
 * 文件名  ：key.c
 * 描述    ：
**********************************************************************************/
#include "key_input.h" 

#define KEY_MAX_NUMBER 2
#define USE_HARDWARE_DEBUG_KEY_INPUT 0
//static int g_i_key_press_count[KEY_MAX]= 0;	//检测到按键按下的次数
static int g_i_key_press_time[KEY_MAX_NUMBER] = {0}; //检测到按键按下的时间
static int g_i_key_press_flag[KEY_MAX_NUMBER] = {0}; //检测到按键按下的标志
static int g_i_key_pressed[KEY_MAX_NUMBER]	= {0};	//按键确实按下了的标志


#define key2_rcc                    RCC_APB2Periph_GPIOA
#define key2_gpio                   GPIOA
#define key2_pin                    (GPIO_Pin_12)

#define key1_rcc                    RCC_APB2Periph_GPIOC
#define key1_gpio                   GPIOC
#define key1_pin                    (GPIO_Pin_12)

void rt_hw_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(key1_rcc|key2_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = key1_pin;
    GPIO_Init(key1_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = key2_pin;
    GPIO_Init(key2_gpio, &GPIO_InitStructure);
}

int rt_read_key_value(int i_key_number)
{
	if(i_key_number > KEY_MAX_NUMBER)
		return -1;
						 
	if(	i_key_number == 1)
		return  GPIO_ReadInputDataBit(key1_gpio, key1_pin);
	if(	i_key_number == 2)
		return  GPIO_ReadInputDataBit(key2_gpio, key2_pin);

	return -1;
}


/*
*
* 	本部分为处理按键输入的底层驱动。
*		主要功能为:检测按键按下，对于长按等操作，暂时不做支持。
*
*	上层驱动，只需要做逻辑上的判断即可
*
*
*	按键部分处理程序		
*	首次编写时间:	2016-10-22
*	作者		: 	袁兆铭
*		
*	该部分内容的处理逻辑为:
*		只要检测到一个按键按下，则计时程序开始计时，到一定的时长之后，查看按键是否按下。
*		若该按键按下，则表明按键确实按下，程序需要等待按键释放才能执行后面的程序。
*		若该按键未被按下，则认为为按键抖动，清除按键按下标志。
*/
int rt_handle_key_input(void)
{
	int i = 0;

	//通过变量值来判断是否按下按键了,注意要判断时间
	for(i = 0 ; i < KEY_MAX_NUMBER ; i ++)
	{
		if(rt_read_key_value(i+1) == 0)
		{
			g_i_key_press_flag[i] = 1;
			#if USE_HARDWARE_DEBUG_KEY_INPUT
		   	rt_kprintf("key_%d pressing\r\n",i+1);
			#endif
		}	

		if(g_i_key_press_flag[i])
		{
			//这一步是用检测按键按下的时长
			g_i_key_press_time[i] ++;	

	   		if(g_i_key_press_time[i] >= 5)//过了一定的时间之后，再来判断按键是否按下
			{
				if(rt_read_key_value(i+1) == 0)
				{
					g_i_key_pressed[i] = 1;
					#if USE_HARDWARE_DEBUG_KEY_INPUT
					rt_kprintf("key_%d really pressed\r\n",i+1);
					#endif
				}
			}

			//等待按键释放
			if((rt_read_key_value(i+1) == 1))
			{
				g_i_key_press_flag[i] = 0;
				g_i_key_press_time[i] = 0;
				g_i_key_pressed[i] = 0; 	
			}
		}
	}
   	
	for(i = 0 ; i < KEY_MAX_NUMBER ; i ++)
	{
		if(g_i_key_pressed[i] == 1)
		{
		 	return (i + 1);
		}
	}
	return 0;
}


/******************* (C) COPYRIGHT 2016 QinJing *****END OF FILE****/
