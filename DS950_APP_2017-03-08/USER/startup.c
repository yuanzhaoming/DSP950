/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-08-31     Bernard      first implementation
 */
  	      
/*
* 		
*  	基本task说明:
*	1.LED2 task
*		硬件:PA2 ,做工作状态显示，平均一秒闪烁一次即可。
*	2.KEY1 task
*		硬件:PA12,做工作模式切换，对应的各个显示灯亮。
*  	3.
*
*/
 
#include <rthw.h>
#include <rtthread.h>
#include "shell.h" 

#include "board.h"



/**
 * @addtogroup STM32
 */

/*@{*/

extern int  rt_application_init(void);

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#elif __ICCARM__
#pragma section="HEAP"
#else
extern int __bss_end;
#endif

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
    rt_kprintf("\n\r Wrong parameter value detected on\r\n");
    rt_kprintf("       file  %s\r\n", file);
    rt_kprintf("       line  %d\r\n", line);

    while (1) ;
}

	 

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* init board */
    rt_hw_board_init();

    /* show version */
    rt_show_version();

#ifdef RT_USING_HEAP
	#if STM32_EXT_SRAM
    	rt_system_heap_init((void*)STM32_EXT_SRAM_BEGIN, (void*)STM32_EXT_SRAM_END);
	#else
		#ifdef __CC_ARM
    		rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)STM32_SRAM_END);
		#elif __ICCARM__
    	rt_system_heap_init(__segment_end("HEAP"), (void*)STM32_SRAM_END);
		#else
    	/* init memory system */
   	 	rt_system_heap_init((void*)&__bss_end, (void*)STM32_SRAM_END);
		#endif
	#endif  /* STM32_EXT_SRAM */
#endif /* RT_USING_HEAP */

    /* init scheduler system */
    rt_system_scheduler_init();

    /* initialize timer */
    rt_system_timer_init();

    /* init timer thread */
    rt_system_timer_thread_init();

    /* init application */
    rt_application_init();

	#ifdef RT_USING_FINSH
	/* init finsh */
	finsh_system_init(); //初始化FINSH线程
	finsh_set_device("uart1"); //设置FINSH使用的输入输出设备为 "uart1"
	#endif

   



    /* init idle thread */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return ;
}		  

int main(void)
{
    /* disable interrupt first */
	rt_uint32_t level;
	(void)level;//防止编译器报警告
   	level =  rt_hw_interrupt_disable();
	//rt_hw_interrupt_enable(level);
    /* startup RT-Thread RTOS */
    rtthread_startup();

    return 0;
}

/*@}*/
