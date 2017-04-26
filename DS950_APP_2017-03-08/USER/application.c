/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */
 		     	      
/**
 * @addtogroup STM32
 */
/*@{*/			   
	        
#include <board.h>
#include <rtthread.h>

#include "do_data.h"
#include "key_input.h"
#include "spi_instruction.h"
#include "enc28j60.h"
#include "arch/sys_arch.h"
#include "tcpserver.h" 
#include "spi_flash.h"
#include "tftp.h"
#include "dsp_reset.h"
#include "watchdog.h"
#include "cpu_usage.h"
#include "sha204a.h"

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "led.h"
#include "lcd1602.h"

extern void rt_spi_flash_device_init(void);

extern int g_input_type;
extern int g_volume;
extern int g_mute;

extern int g_set_volume_to_dsp_flag;//需要将pc端设置好的音量数据传给dsp

int g_key_input_pressed_flag = 0;//该标志用来判断是否需要存储数据。这个也包括检测到了音量数据有变化的检测。



typedef struct{
	unsigned char AD_input;			//为AD输入
	unsigned char AES_input;
	unsigned char RCA_input;
	unsigned char MIC_input;
   	unsigned char SPDIF_input;
	unsigned char COAX_input;
	unsigned char mute_select;
}KEY_SPI;

KEY_SPI g_key_spi;

/*
*		数据存储的数组数据:
* 			来了一个指令，就搜索一下数组，没有该指令，则存进去
*			g_i_code_number :已经有多少条指令
*			g_spi_code[]，存成一个数组形式，有多少条不同指令，存成多少个数组内容值即可。
*/
unsigned int g_spi_code[CODE_MAX];
int  g_i_code_number = 1; 

static struct rt_semaphore tcp_dsp_sem;  
//int g_set_volume_to_pc_flag = 0;//需要将数据发送到PC上的标志

//////////////////////////////////////////
//unsigned char arr[4096];



#define USE_KEY_INPUT 1
#if  USE_KEY_INPUT
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t key_input_stack[ 512 ];
static struct rt_thread key_input_thread;
//#define USE_DEBOUNCING 1
#define USE_RELEASE_KEY_INPUT 1

#define USE_INTERNAL_FLASH  0//定义是否使用内部flash空间
#define WRITE_INTERNAL_PLACE 0x0807F000

static int g_i_key1_press_ok = 0;
static int g_i_key2_press_ok = 0;
static int g_i_have_key_pressed = 0;

int g_i_key_led_handle_count = 0;//主要是计数，做音频信号输入的选择
static int g_i_lcd_show_value = -1; 	//是在lcd1602中使用的量

#define USE_KEY_APP_DEBUG 1
//使用信号量，这样可以将按键操作的内容发送给DSP和显示屏
/*
* 	key_value(stm32)---> dsp(adsp21489)
*			 			 lcd1602(stm32)
*/

#if USE_RELEASE_KEY_INPUT

void select_input_type(unsigned int c_type)
{
	switch(c_type)
	{
		case AD_INPUT:
			g_key_spi.AD_input = 1;
			g_key_spi.AES_input = 0;
			g_key_spi.RCA_input = 0;
			g_key_spi.MIC_input = 0;
			g_key_spi.SPDIF_input = 0;
			g_key_spi.COAX_input = 0;

			//SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			//rt_kprintf("g_key_spi.AD_input\r\n");
		break;
		case AES_INPUT:
			g_key_spi.AES_input = 1;

			g_key_spi.AD_input = 0;
			g_key_spi.RCA_input = 0;
			g_key_spi.MIC_input = 0;
			g_key_spi.SPDIF_input = 0;
			g_key_spi.COAX_input = 0;

			//SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			//rt_kprintf("g_key_spi.AES_input\r\n");
		break;
		case DVD_INPUT:
			g_key_spi.RCA_input = 1;

			g_key_spi.AES_input = 0;
			g_key_spi.AD_input = 0;
			g_key_spi.MIC_input = 0;
			g_key_spi.SPDIF_input = 0;
			g_key_spi.COAX_input = 0;

			//SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			//rt_kprintf("g_key_spi.RCA_input\r\n");
		break;
		case MIC_INPUT:
			g_key_spi.MIC_input = 1;

			g_key_spi.RCA_input = 0;
			g_key_spi.AES_input = 0;
			g_key_spi.AD_input = 0;
			g_key_spi.SPDIF_input = 0;
			g_key_spi.COAX_input = 0;

			//SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			//rt_kprintf("g_key_spi.MIC_input\r\n");
		break;
		case SPDIF_INPUT:
			g_key_spi.SPDIF_input = 1;

			g_key_spi.MIC_input = 0;
			g_key_spi.RCA_input = 0;
			g_key_spi.AES_input = 0;
			g_key_spi.AD_input = 0;
			g_key_spi.COAX_input = 0;

			//SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			//rt_kprintf("g_key_spi.SPDIF_input\r\n");
		break;
		case COAX_INPUT:
			g_key_spi.COAX_input = 1;

			g_key_spi.SPDIF_input = 0;
			g_key_spi.MIC_input = 0;
			g_key_spi.RCA_input = 0;
			g_key_spi.AES_input = 0;
			g_key_spi.AD_input = 0;

			//SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			//rt_kprintf("g_key_spi.COAX_input\r\n");
		break;	
		default:
		break;
	}
}

/*
*		该部分为按键处理程序的应用层程序
*
*		应用层处理逻辑为:
*			 两个按键为互斥关系，需要一个按键处理完成，才能处理另外一个。
*
*		这个在底层驱动中，是没有要求互斥关系的。
*/
static void key_input_thread_entry(void* parameter)
{
	int key_input_number;

	static int i_first_mute = 0;
/* key init start */
#ifdef RT_USING_FINSH
//	rt_kprintf("key_input init\r\n");
#endif
	rt_hw_key_init();
	rt_hw_led_init();

	//rt_hw_led_all_on();
    while (1)
    {
		//检测是那一个按键按下了。
		key_input_number = rt_handle_key_input();

		if(g_i_have_key_pressed == 0)
		{
			if(g_i_key1_press_ok == 0)
			{
				if(	key_input_number == 1)
				{
					//rt_kprintf("key1_input ok\r\n");
					g_i_key1_press_ok = 1; 	
					g_i_have_key_pressed = 1;
				}

				if(	key_input_number == 2)
				{
					g_i_key2_press_ok = 1;
					//rt_kprintf("key2_input ok\r\n");
					g_i_have_key_pressed = 1; 	
				}
			}
		}
		else
		{
			if(g_i_key1_press_ok == 1)
			{
				g_i_key2_press_ok = 0;
			 	//第一个按键按下了,等待释放
				if(rt_read_key_value(1) == 1)
				{
					//释放了，才做处理
					#if USE_KEY_APP_DEBUG
					//rt_kprintf("key1_input release\r\n");
					#endif
					g_i_key1_press_ok = 0;
					g_i_have_key_pressed = 0;

					//这是防止设置成MIC后不能恢复，一般情况下，是不能进入到这个部分的
					if(g_i_key_led_handle_count	== MIC_INPUT)
					{
						g_i_key_led_handle_count = INIT_INPUT;	
					} 

					g_i_key_led_handle_count += (1<<8);
					if(	g_i_key_led_handle_count == MIC_INPUT)
						g_i_key_led_handle_count = INIT_INPUT;

					g_input_type = 	g_i_key_led_handle_count;
					//g_i_key_led_handle_count = g_i_key_led_handle_count % 6;

				//	rt_kprintf("key1_input release:0x%08x\r\n",g_i_key_led_handle_count);

					//后面spi指令集根据该标志来发送数据
					select_input_type(g_i_key_led_handle_count);
					
					//加一个设置按键按下的标志
					g_key_input_pressed_flag = 1;
						
				}	
			}
			
			if(g_i_key2_press_ok == 1)
			{
				g_i_key1_press_ok = 0;
				if(rt_read_key_value(2) == 1)
				{
					//释放了，才做处理
					#if USE_KEY_APP_DEBUG
					//rt_kprintf("key2_input release\r\n");
					#endif
					g_i_key2_press_ok = 0;
					g_i_have_key_pressed = 0;

					{
						static int i_mute = 1;
						i_mute ++;
						i_mute = i_mute % 2;
						g_mute = i_mute;
					}
					g_key_spi.mute_select = 1;

					i_first_mute = 1;

				  	//加一个设置按键按下的标志,按键2不保存，因此，不用设置标志位了。
					//g_key_input_pressed_flag = 1;

				}
			}	
		}
	   	
		rt_hw_led_mutex((((g_i_key_led_handle_count) >> 8) & 0xf) - 1);

		if(	i_first_mute == 0)
		{
			led(5,0);	 	
		}
		else
		{
			//mute灯情况
			if(g_mute == 1)
				led(5,0);
			else
				led(5,1);
		}

		rt_thread_delay( RT_TICK_PER_SECOND / 100);
		#endif
    }
}
#endif




ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;

//对应DSP输入方式有:MIC,AD,AES,SPDIF,RCA.
//有两个LED,MUTE(LED1)和状态指示灯(LED2).
static void led_thread_entry(void* parameter)
{
    unsigned int count=0;
	//硬件初始化
    rt_hw_led_init();
	
    while (1)
    {
        /* led1 on */
#ifndef RT_USING_FINSH
        rt_kprintf("led on, count : %d\r\n",count);
#endif
        count++;
        rt_hw_led_on(6);
        rt_thread_delay( RT_TICK_PER_SECOND * 1 ); /* sleep 0.5 second and switch to other thread */
		//rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
		//rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
        /* led1 off */
#ifndef RT_USING_FINSH
        rt_kprintf("led off\r\n");
#endif
        rt_hw_led_off(6);
        rt_thread_delay( RT_TICK_PER_SECOND * 1);
    }
}




#define USE_LCD1602 1
#if  USE_LCD1602
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t lcd1602_stack[ 512 ];
static struct rt_thread lcd1602_thread;
#define USE_RELEASE_LCD1602 1
static void lcd1602_thread_entry(void* parameter)
{
   	int i_show_value = 0;
	static int i_show_flag = 0;
	int i_lcd_show_value = 0;

	(void)i_show_value;//防止编译器报警告
	(void)i_show_flag;//防止编译器报警告
/* lcd1602 start */
#ifdef RT_USING_FINSH
	//rt_kprintf("lcd1602 init\r\n");
#endif

	
    while (1)
    {
		#if USE_RELEASE_LCD1602

		if(g_i_lcd_show_value!= g_i_key_led_handle_count)
		{
			//显示现在的值
			g_i_lcd_show_value = g_i_key_led_handle_count;

			i_lcd_show_value = 	(((g_i_key_led_handle_count) >> 8) & 0xf) - 1;
			//AES
			if(	i_lcd_show_value == 0)
			{
				LCD_Clear();
				LCD_Write_String(0,0,"AES input");
			}
			//SPDIF
			if(	i_lcd_show_value == 1)
			{
				LCD_Clear();
				LCD_Write_String(0,0,"SPDIF input");		
			}
			//COAX
			if(	i_lcd_show_value == 2)
			{
				LCD_Clear();
				LCD_Write_String(0,0,"COAX input");		
			}
			//DVD
			if(	i_lcd_show_value == 3)
			{
				LCD_Clear();
				LCD_Write_String(0,0,"DVD input");		
			}
			//AD
			if(	i_lcd_show_value == 4)
			{
				LCD_Clear();
				LCD_Write_String(0,0,"AD input");		
			}
			//COAX
			if(	i_lcd_show_value == 5)
			{
				LCD_Clear();
				LCD_Write_String(0,0,"MIC input");		
			}
		}

#if 0
		{
		 	int i_sign;
			int i_ten;
			int i_single;
			int i_vol;
			static int i_volume_now;
			static int i_volume_old;
			//g_volume
			i_volume_now = do_get_data(1,8);
			if(	i_volume_now !=  i_volume_old)
			{
				i_volume_old = i_volume_now;
				i_vol = (i_volume_old >> 16) - 90;
				if(	i_vol < 0){
					i_sign = 1;
					i_vol = -i_vol;
				}
				else
					i_sign = 0;

				i_ten = i_vol / 10;
				i_single = i_vol % 10;

				if(i_sign == 1)
				{
					LCD_Write_Char(13,1,'-');
				}
				else
				{
					LCD_Write_Char(13,1,' ');
				}
				if((i_ten == 0))
				{
					LCD_Write_Char(14,1,' ');
				}
				else
				{
					LCD_Write_Char(14,1,'0'+i_ten);
				}
				LCD_Write_Char(15,1,'0'+i_single);
				//rt_kprintf("VOLUME:%0x\r\n",i_volume_old);
			}
		}
#endif		

		#if 0
        LCD_Write_Char(0,0,i_show_value+'0');
        //rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */  
        //rt_thread_delay( RT_TICK_PER_SECOND/2 );
		i_show_value ++;
		i_show_value = i_show_value % 10;
		#endif

		rt_thread_delay( RT_TICK_PER_SECOND / 20);
		#endif
    }
}
#endif




#define USE_IIC_CS4385 0

#if USE_IIC_CS4385

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t IIC_CS4385_stack[ 512 ];
static struct rt_thread IIC_CS4385_thread;

static void IIC_CS4385_thread_entry(void* parameter)
{
    unsigned int count=0;	

	rt_thread_delay( RT_TICK_PER_SECOND );

#define USE_CS4385_IIC 0
#if USE_CS4385_IIC
	{
		int i_CS4385_ret;
		int i_using_mode = 1;//1为tdm模式,0为I2S模式
		IIC_Init();
		i_CS4385_ret = cs4385_set_iic_mode(i_using_mode);//使用tdm模式
		if(i_CS4385_ret == 0)
			rt_kprintf("CS4385 initialized ok! using %s!!!\r\n",i_using_mode?"tdm":"I2S");
		else
			rt_kprintf("CS4385 initialized failed , using %s!!!\r\n",0?"tdm":"I2S");
	}
#endif
	
    while (1)
    {


        rt_thread_delay( RT_TICK_PER_SECOND * 2);
    }
}

#endif


#define USE_SPI_DSP_THREAD 1

#if USE_SPI_DSP_THREAD

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t SPI_DSP_stack[ 512 ];
static struct rt_thread SPI_DSP_thread;

#define Tranverse32(X)                 ((((unsigned int)(X) & 0xff000000) >> 24) | \
                                                           (((unsigned int)(X) & 0x00ff0000) >> 8) |\
                                                           (((unsigned int)(X) & 0x0000ff00) << 8) |\
                                                           (((unsigned int)(X) & 0x000000ff) << 24))

//获取当前的输入类型
void get_input_type()
{
	switch(	g_i_key_led_handle_count)
	{
	case AD_INPUT:
   		rt_kprintf("AD input\r\n");
	break;
	case AES_INPUT:
		rt_kprintf("AES input\r\n");
	break;
	case DVD_INPUT:
		rt_kprintf("DVD input\r\n");
	break;
	case SPDIF_INPUT:
	 	rt_kprintf("SPDIF input\r\n");
	break;
	case COAX_INPUT:
	  	rt_kprintf("COAX input\r\n");
	break;
	case MIC_INPUT:
		rt_kprintf("MIC input\r\n");
	break;
	default:
	break;
	}
}
FINSH_FUNCTION_EXPORT(get_input_type, get the input type now);



void list_input()
{
	rt_kprintf("\r\n input_type            code\r\n");
	rt_kprintf("   AES                     1 \r\n");	
	rt_kprintf("   SPDIF                   2 \r\n");	
	rt_kprintf("   COAX                    3 \r\n");		
	rt_kprintf("   DVD                     4 \r\n");		
	rt_kprintf("   AD                      5 \r\n");		
	rt_kprintf("   MIC                     6 \r\n");
}
FINSH_FUNCTION_EXPORT(list_input, list input type and code);
	         


//这个主要是用来将指令输出到dsp上，便于调试，因为前期没有前面板 2016-11-29
void code_to_dsp(int i_code_number)
{
	switch(i_code_number)
	{
		case 1:	   
			g_i_key_led_handle_count = 	AES_INPUT;
			g_input_type = 	AES_INPUT;
			select_input_type(g_input_type);
			//加一个设置按键按下的标志
			g_key_input_pressed_flag = 1;
			SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			rt_kprintf("AES instruction\r\n");
		break;
		case 2:
			g_i_key_led_handle_count = 	SPDIF_INPUT;
			g_input_type = 	SPDIF_INPUT;
			select_input_type(g_input_type);
			//加一个设置按键按下的标志
			g_key_input_pressed_flag = 1;
			SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			rt_kprintf("SPDIF instruction\r\n");
		break;
		case 3:
			g_i_key_led_handle_count = 	COAX_INPUT;
			g_input_type = 	COAX_INPUT;
			select_input_type(g_input_type);
			//加一个设置按键按下的标志
			g_key_input_pressed_flag = 1;
			SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			rt_kprintf("COAX instruction\r\n");
		break;
		case 4:
			g_i_key_led_handle_count = 	DVD_INPUT;			
			g_input_type = 	DVD_INPUT;
			select_input_type(g_input_type);
			//加一个设置按键按下的标志
			g_key_input_pressed_flag = 1;
			SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			rt_kprintf("DVD instruction\r\n");
		break;
		case 5:
			g_i_key_led_handle_count = 	AD_INPUT;
			g_input_type = 	AD_INPUT;
			select_input_type(g_input_type);
			//加一个设置按键按下的标志
			g_key_input_pressed_flag = 1;
			SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			rt_kprintf("AD instruction\r\n");
		break;
		case 6:
			g_i_key_led_handle_count = 	MIC_INPUT;
			g_input_type = 	MIC_INPUT;
			select_input_type(g_input_type);
			//加一个设置按键按下的标志
			g_key_input_pressed_flag = 1;
			SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
			rt_kprintf("MIC instruction\r\n");
		break;				
			
	}	
}
FINSH_FUNCTION_EXPORT(code_to_dsp, send spi code to dsp);


/*
* 		2016-12-26 新添加的命令，用来获取当前的输入类型等
*
*		author : yuan
*
*/
void dsp_input()
{
	unsigned int i_value = 0;
	i_value = 0x00002006;
	SPI_DSP_SendBytes((unsigned char *)&i_value,4);
	rt_kprintf("get dsp input type\r\n");	
}
FINSH_FUNCTION_EXPORT(dsp_input, get dsp input );


void release_sem( void)
{
	rt_sem_release(&tcp_dsp_sem); 
}

#define HTONL(v)	 ( ((v) << 24) | (((v) >> 24) & 0xff) | (((v) << 8) & 0xff0000) | (((v) >> 8) & 0xff00) )


static void SPI_DSP_thread_entry(void* parameter)
{
    //unsigned int count=0;
	//unsigned char buff[4] = {0x12,0x34,0x56,0x70};
	unsigned int c_read_data;
	rt_err_t result;  
	static int i_press_count = 0;	
//	unsigned char c_internal_value = 0;
	//初始化DSP指令相关驱动程序
//	rt_hw_stm32_dsp_instruction_init();
#if 0  //给dsp启动的程序，需要在开启任务之前做，因此移到创建任务之前。
	SPI_DSP_Init();
	rt_kprintf("SPI_DSP init\r\n");

	//将程序发送给dsp，使用slave boot来启动程序。
	//tftp_send_dsp();
#endif

	//刚开始启动，要将dsp初始化，也就是说，要把音量，输入类型的数据传给dsp.
	rt_thread_delay( RT_TICK_PER_SECOND);

	SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);
	rt_thread_delay( RT_TICK_PER_SECOND / 50);
	SPI_DSP_SendBytes((unsigned char *)&g_volume,4);

    while (1)
    {
		#if	 1
		//tcp将命令发送给dsp
		result = rt_sem_take(&tcp_dsp_sem, 1); 
		if (result == RT_EOK)
		{	
			//rt_thread_delay( RT_TICK_PER_SECOND  / 10);
			
			tcp_send_instruction_to_dsp();			
		}    		  

		if(g_key_spi.mute_select == 1)
		{
	   		g_key_spi.mute_select = 0;
			{
				
				//int i_value = 0x0000E005 | (g_mute << 8); 
				int i_value = 0x05E00000 | (g_mute << 16);
				//rt_thread_delay( RT_TICK_PER_SECOND  / 10); 
				SPI_DSP_SendBytes((unsigned char *)&i_value,4);
			}
			i_press_count ++;
			//rt_kprintf("mute selected :%d,%d\n",g_mute,i_press_count);
		}

		if(g_set_volume_to_dsp_flag == 1)
		{
			rt_thread_delay( RT_TICK_PER_SECOND  / 10);
			//将音量数据传给dsp,主要是由于pc端有动作
			g_set_volume_to_dsp_flag = 0;
			{
				int volume_temp = do_get_data(1,8); 
				volume_temp = Tranverse32(volume_temp);//Tranverse32(g_volume);
				SPI_DSP_SendBytes((unsigned char *)&(volume_temp),4);
//				rt_kprintf("send dsp the volume to synchronize : 0x%08x \n",volume_temp);
			}
		}
		#endif
		//rt_thread_delay( RT_TICK_PER_SECOND  / 10);
		//在tcp端有命令，就需要发送出去。
		//tcp_send_instru_to_dsp_via_spi();
		
		#if 1
		//rt_thread_delay( RT_TICK_PER_SECOND  / 100);
		c_read_data = SPI_DSP_ReadByte();
		if((c_read_data != 0) && (c_read_data != 0xffffffff))
		{
			//c_read_data = Tranverse32(c_read_data);
			//rt_kprintf("read:0x%0x\n",c_read_data);
			//rt_kprintf("read:0x%0x \n",c_read_data);

			do_set_cmd_entry(SET_ENTRY_CMD, &c_read_data);
			//发送给网络	
			send_instru_via_socket( (&c_read_data));

			c_read_data = 0;
		   	//也需要过1秒才能存储
			g_key_input_pressed_flag = 1;
	
		}
		#endif
	//	rt_kprintf("data:%0x\n",SPI_DSP_ReadByte());
        //rt_thread_delay( RT_TICK_PER_SECOND  / 5);
    }
}
#endif

#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif
						 
#ifdef  RT_USING_FINSH
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
	
	  

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
        rt_kprintf("File System initialzation failed!\n");
	{
		dfs_init();
		elm_init(); 
#ifdef RT_USING_DFS_ELMFAT	
	    /* mount sd card fat partition 1 as root directory */  
	    if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)  
	    {  
	        rt_kprintf("flash0 mount to /.\n");  
	    }  
	    else  
	        rt_kprintf("flash0 mount to / failed.\n"); 
#endif  
    }    
#endif  /* RT_USING_DFS */



//读取内部flash的值，进而可以设置一些变量的值
#if 0//USE_INTERNAL_FLASH
{
	unsigned int i_internal_value = *(volatile unsigned long *)WRITE_INTERNAL_PLACE;

	rt_kprintf("old input type:%d\n",i_internal_value);

	if((i_internal_value>5)||(i_internal_value<0))
		i_internal_value = 0;		

	g_i_key_led_handle_count = i_internal_value;

	select_input_type(g_i_key_led_handle_count); 

	rt_kprintf("inter input type:%d\n",i_internal_value);
}
#endif


	//rt_hw_rtc_init();
#ifdef RT_USING_LWIP
		eth_system_device_init();

		rt_hw_stm32_eth_init();

		lwip_sys_init();
//		rt_kprintf("TCP/IP initialized!\n");

#if defined(RT_USING_DFS) && defined(RT_USING_LWIP) && defined(RT_USING_DFS_NFS)
	{
		/* NFSv3 Initialization */
		rt_kprintf("begin init NFSv3 File System ...\n");
		nfs_init();
 		rt_kprintf("nfs init File System ...\n");

		if (dfs_mount(RT_NULL, "/", "nfs", 0, RT_NFS_HOST_EXPORT) == 0)
			rt_kprintf("NFSv3 File System initialized!\n");
		else
			rt_kprintf("NFSv3 File System initialzation failed!\n");
	}
#endif
		//netio_init();
		//开启服务器程序，接收数据开始
		tcpserv((void *) NULL);
#endif
	
	rt_thread_delay(50);
}



#define USE_SPI_FLAHS_THREAD 1

#if USE_SPI_FLAHS_THREAD

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t spi_flash_stack[ 512 ];
static struct rt_thread spi_flash_thread;

//对应DSP输入方式有:MIC,AD,AES,SPDIF,RCA.
//有两个LED,MUTE(LED1)和状态指示灯(LED2).
static void spi_flash_thread_entry(void* parameter)
{  
#if 0
	int FlashID,DeviceID;
	//rt_spi_flash_device_init(); 	               
	SPI_FLASH_Init( );
	DeviceID = SPI_FLASH_ReadDeviceID( );
	FlashID = SPI_FLASH_ReadID( );
	
	rt_kprintf("\r\n FlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);

#ifdef RT_USING_FINSH
        rt_kprintf("spi_flash thread start\r\n");
#endif
#endif	
    while (1)
    {

        rt_thread_delay( RT_TICK_PER_SECOND * 2);
    }
}
#endif



#define USE_IDLE_TIMER 1

#if USE_IDLE_TIMER

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t idle_timer_stack[ 512 ];
static struct rt_thread idle_timer_thread;

int g_i_store_flag = 0;


unsigned int g_system_runtime = 0;

//空闲运转的线程
static void idle_timer_thread_entry(void* parameter)
{  
	
	static int i_count = 0;
	static int i_input_event = 0;
	static int i_type_count = 0;

	static int i_dog_init = 0;

	static int i_time_count = 0;
	
	//rt_kprintf("\r\n idle timer thread \r\n");

	//空闲任务，做看门狗操作
	Wdt_Init();				   
		
    while (1)
    {
		i_dog_init ++;

		if(i_dog_init >= 1)
		{
		 	Kick_Dog();
		}
		
		//清除标志，如果为0的能达到两次，说明可以做存储了。
	   	if(g_key_input_pressed_flag == 1)
		{	
			g_key_input_pressed_flag = 0;

			i_input_event = 1;

		}
		else
		{
			if(i_input_event == 1)
			{
				i_count ++;
				i_type_count ++;	
			}		
		}
		//250ms变化一次
		if(	i_type_count == 1)
		{
			i_type_count = 0;
			//将输入类型设置发送给DSP
			SPI_DSP_SendBytes((unsigned char *)&g_input_type,4);			
		}

		if(i_count >= 2 * 4)
		{
			i_input_event = 0;
			i_count = 0;

			g_i_store_flag = 1;
			//rt_kprintf("\r\n ..timer timeout,need to store flash,input:0x%08x\r\n",g_input_type);

			//即可存储数据
			store_w25q16_volume_input_type();

		}
		//系统运行时间
		i_time_count ++;
	 
		if(i_time_count >= 4)
		{
			i_time_count = 0;
			g_system_runtime ++;	
		}
		rt_thread_delay( RT_TICK_PER_SECOND / 4);
    }
}
#endif


void time()
{
	int i_hour,i_minute,i_sec,i_day;
	i_day =  g_system_runtime / 3600 / 24;
	i_hour = (g_system_runtime - i_day * 3600 * 24 )/3600;
	i_minute = (g_system_runtime - i_hour * 3600 - i_day * 3600 * 24) / 60;
	i_sec = g_system_runtime - i_day * 3600 * 24 - i_hour * 3600 - i_minute * 60;
	rt_kprintf("time eclapse:%02d day,%02d:%02d:%02d \n",i_day,i_hour,i_minute,i_sec);
}
FINSH_FUNCTION_EXPORT(time, system run time);

#if 0
__asm int tt(int result)
{
	MOV r1,r0
	add r1,r1,1
	mov r0,r1
	BX LR
}
#endif

int FlashID,DeviceID;


int rt_application_init(void)
{
    rt_thread_t init_thread;
    rt_err_t result;

	//首先给DSP复位		 
	rt_hw_dsp_reset_init();
{
	int i = 0;
	for(i = 0 ;i < 100000; i ++);//大约算10ms
	rt_hw_dsp_reset_on();
	for(i = 0 ;i < 100000; i ++);//大约算10ms
	rt_hw_dsp_reset_off();
}
			  

	//显示cpu的使用率
	cpu_usage_init();

//验证加密芯片
	//ATSHA204m_mac_test();
	sha204_authentication();


	#if  USE_RELEASE_LCD1602
	rt_hw_lcd1602_init();
	LCD_Write_String_hard(2,1,"DSP950 V0.1");
	#endif

#if 0
	int t;
	t = tt(2);
	rt_kprintf("t: %d \n",t);
#endif

	//给dsp的升级程序，注意要加上一定的延时
	{
		int i = 0, j= 0;
		
	
		for(i = 0 ; i < 100000 ;i ++)
			for(j = 0 ; j < 100; j ++)
				;
		//rt_spi_flash_device_init(); 	               
		SPI_FLASH_Init( );
		DeviceID = SPI_FLASH_ReadDeviceID(  );
		FlashID = SPI_FLASH_ReadID( );
		
		//rt_kprintf("\r\n FlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
	
		SPI_DSP_Init();
		//rt_kprintf("SPI_DSP init\r\n");
		//将程序发送给dsp，使用slave boot来启动程序。
		tftp_send_dsp();
		//程序发送完毕，需要再等待一会儿
		for(i = 0 ; i < 100000 ;i ++)
			for(j = 0 ; j < 100; j ++)
				;
	}	    

		 

	// 等待
    /* init led thread */
    result = rt_thread_init(&led_thread,
                            "led",
                            led_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&led_stack[0],
                            sizeof(led_stack),
                            20,
                            5);

    if (result == RT_EOK)
    {
        rt_thread_startup(&led_thread);
    }
	else
		rt_kprintf("led thread failed!\n");

    /* init lcd thread */
    result = rt_thread_init(&lcd1602_thread,
                            "lcd",
                            lcd1602_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&lcd1602_stack[0],
                            sizeof(lcd1602_stack),
                            17,
                            10);
    if (result == RT_EOK)
    {
        rt_thread_startup(&lcd1602_thread);
    }  
	else
		rt_kprintf("lcd thread failed!\n");

#if USE_SPI_DSP_THREAD
   	 

	result = rt_sem_init(&tcp_dsp_sem, "tcp_dsp", 0, RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)  
    {  
        rt_kprintf("init tcp_dsp semaphore failed. \n");  
        return -1;  
    } 

    /* init spi_dsp thread */
    result = rt_thread_init(&SPI_DSP_thread,
                            "SPI_DSP",
                            SPI_DSP_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&SPI_DSP_stack[0],
                            sizeof(SPI_DSP_stack),
                            19,
                            10);
    if (result == RT_EOK)
    {
        rt_thread_startup(&SPI_DSP_thread);
    }
	else
		rt_kprintf("SPI-DSP thread failed!\n");

#endif

    /* init key thread */
    result = rt_thread_init(&key_input_thread,
                            "key",
                            key_input_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&key_input_stack[0],
                            sizeof(key_input_stack),
                            18,
                            10);
    if (result == RT_EOK)
    {
        rt_thread_startup(&key_input_thread);
    }
	else
		rt_kprintf("key thread failed!\n");


#if USE_IIC_CS4385
    /* init CS4385 thread */
    result = rt_thread_init(&IIC_CS4385_thread,
                            "CS4385-SHA",
                            IIC_CS4385_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&IIC_CS4385_stack[0],
                            sizeof(IIC_CS4385_stack),
                            16,
                            10);
    if (result == RT_EOK)
    {
        rt_thread_startup(&IIC_CS4385_thread);
    }
	else
		rt_kprintf("CS4385-SHA failed!\n");
#endif

#if USE_SPI_FLAHS_THREAD
    /* init spi_flash thread */
    result = rt_thread_init(&spi_flash_thread,
                            "SPI_FLASH",
                            spi_flash_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&spi_flash_stack[0],
                            sizeof(spi_flash_stack),
                            15,
                            10);
    if (result == RT_EOK)
    {
        rt_thread_startup(&spi_flash_thread);
    }
	else
		rt_kprintf("spi_flash thread failed!\n");
#endif


#if USE_IDLE_TIMER
    /* init spi_flash thread */
    result = rt_thread_init(&idle_timer_thread,
                            "IDLE_TIMER",
                            idle_timer_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&idle_timer_stack[0],
                            sizeof(idle_timer_stack),
                            13,
                            10);
    if (result == RT_EOK)
    {
        rt_thread_startup(&idle_timer_thread);
    }
	else
		rt_kprintf("idle_timer thread failed!\n");
#endif

//	rt_kprintf("File System initialized!\n");
//	usr_echo_init();	

//下面为动态创建的线程，我们不用。
#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);
	else
		rt_kprintf("init_thread failed!\n");


    return 0;
}
/*@}*/



