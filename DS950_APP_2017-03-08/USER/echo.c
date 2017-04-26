#include "echo.h"  
#include "rtdef.h"
#include <rtthread.h>
	  
   
struct rx_msg  
{  
    rt_device_t dev;  
    rt_size_t   size;  
};  	    
    
static struct rt_messagequeue  rx_mq;  
static char uart_rx_buffer[64];  
static char msg_pool[2048];  
   
// 串口侦听回调函数  
rt_err_t uart_input(rt_device_t dev, rt_size_t size)  
{  
    struct rx_msg msg;  
    msg.dev = dev;  
    msg.size = size;  
     
        // 将接收内容放入消息队列  
    rt_mq_send(&rx_mq, &msg, sizeof(struct rx_msg));
	
	rt_kprintf("indicated echo\r\n");  
     
    return RT_EOK;  
}  
   
// 任务入口函数  
void usr_echo_thread_entry(void* parameter)  
{  
    struct rx_msg msg;  
     
    rt_device_t device;  
    rt_err_t result = RT_EOK;  
     
        // 从RT系统中获取串口1设备  
    device = (rt_device_t)rt_device_find("uart1");  
    if (device != RT_NULL)  
    {  
                           // 指定接收串口内容的回调函数  
        rt_device_set_rx_indicate(device, uart_input);  
                           // 以读写方式打开设备  
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR);  
    }  
     
    while(1)  
    {  
                           // 从消息队列中获取被回调函数放入消息队列中的内容  
        result = rt_mq_recv(&rx_mq, &msg, sizeof(struct rx_msg), 50);  
        if (result == -RT_ETIMEOUT)  
        {  
            // timeout, do nothing  
        }  
         
        if (result == RT_EOK)  
        {  
            rt_uint32_t rx_length;  
            
            rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ?  
                msg.size : sizeof(uart_rx_buffer) - 1;  
            
            rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0], rx_length);  
            uart_rx_buffer[rx_length] = '\0';  
            // 将内容写回到串口1  
            rt_device_write(device, 0, &uart_rx_buffer[0], rx_length); 
			
			
			 
        }  

		//rt_device_write(device, 0, "wewfwfwe\r\n", 10);  
    }  
}  
// 串口例程初始化函数  
void usr_echo_init()  
{  
    rt_thread_t thread ;  
     
    rt_err_t result;   
      // 创建消息队列，分配队列存储空间  
    result = rt_mq_init(&rx_mq, "mqt", &msg_pool[0], 128 - sizeof(void*), sizeof(msg_pool), RT_IPC_FLAG_FIFO);  
     
    if (result != RT_EOK)   
    {   
        rt_kprintf("init message queue failed.\n");   
        return;   
    }   
    // 创建任务线程  
    thread = (rt_thread_t)rt_thread_create("devt",  
        usr_echo_thread_entry, RT_NULL,  
        1024, 25, 7);  
    // 启动任务线程  
    if (thread != RT_NULL)  
        rt_thread_startup(thread);  

}  



