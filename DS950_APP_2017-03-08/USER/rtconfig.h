/* RT-Thread config file */
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/* RT_NAME_MAX*/
#define RT_NAME_MAX	8

/* RT_ALIGN_SIZE*/
#define RT_ALIGN_SIZE	4

/* PRIORITY_MAX */
#define RT_THREAD_PRIORITY_MAX	32

/* Tick per Second */
#define RT_TICK_PER_SECOND	100

/* SECTION: RT_DEBUG */
/* Thread Debug */
#define RT_DEBUG
#define RT_THREAD_DEBUG

#define RT_USING_OVERFLOW_CHECK

/* Using Hook */
#define RT_USING_HOOK

/* Using Software Timer */
/* #define RT_USING_TIMER_SOFT */
#define RT_TIMER_THREAD_PRIO		4
#define RT_TIMER_THREAD_STACK_SIZE	512
#define RT_TIMER_TICK_PER_SECOND	10

/* SECTION: IPC */
/* Using Semaphore*/
#define RT_USING_SEMAPHORE


/* Using Mutex */
#define RT_USING_MUTEX

/* Using Event */
#define RT_USING_EVENT

/* Using MailBox */
#define RT_USING_MAILBOX

/* Using Message Queue */
#define RT_USING_MESSAGEQUEUE

/* SECTION: Memory Management */
/* Using Memory Pool Management*/
#define RT_USING_MEMPOOL

/* Using Dynamic Heap Management */
#define RT_USING_HEAP

/* Using Small MM */
#define RT_USING_SMALL_MEM

// <bool name="RT_USING_COMPONENTS_INIT" description="Using RT-Thread components initialization" default="true" />
#define RT_USING_COMPONENTS_INIT
#define RT_USING_UART1
/* SECTION: Device System */
/* Using Device System */
#define RT_USING_DEVICE
// <bool name="RT_USING_DEVICE_IPC" description="Using device communication" default="true" />
#define RT_USING_DEVICE_IPC
// <bool name="RT_USING_SERIAL" description="Using Serial" default="true" />
#define RT_USING_SERIAL

/* SECTION: Console options */
#define RT_USING_CONSOLE
/* the buffer size of console*/
#define RT_CONSOLEBUF_SIZE	        128
// <string name="RT_CONSOLE_DEVICE_NAME" description="The device name for console" default="uart1" />
#define RT_CONSOLE_DEVICE_NAME	    "uart1"

/* SECTION: finsh, a C-Express shell */
#define RT_USING_FINSH
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION

#define RT_USING_SPI
/* SECTION: device filesystem */
//#define RT_USING_DFS
//#define RT_USING_DFS_ELMFAT
/* Reentrancy (thread safe) of the FatFs module.  */
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_WORD_ACCESS 
/* Number of volumes (logical drives) to be used. */
//#define RT_DFS_ELM_DRIVES			1
#define RT_DFS_ELM_USE_LFN			0
/* #define RT_DFS_ELM_CODE_PAGE			936 */
//#define RT_DFS_ELM_MAX_LFN			255
/* Maximum sector size to be handled. */
#define RT_DFS_ELM_MAX_SECTOR_SIZE  4096
/* the max number of mounted filesystem */
#define DFS_FILESYSTEMS_MAX			2
/* the max number of opened files 		*/
#define DFS_FD_MAX					4

/* SECTION: lwip, a lighwight TCP/IP protocol stack */
 #define RT_USING_LWIP
/* LwIP uses RT-Thread Memory Management */
#define RT_LWIP_USING_RT_MEM
/* Enable ICMP protocol*/
#define RT_LWIP_ICMP
/* Enable UDP protocol*/
#define RT_LWIP_UDP
/* Enable TCP protocol*/
#define RT_LWIP_TCP
/* Enable DNS */
#define RT_LWIP_DNS

/* the number of simulatenously active TCP connections*/
#define RT_LWIP_TCP_PCB_NUM	 5

/* Using DHCP */
/* #define RT_LWIP_DHCP */

//#define RT_USING_DFS_NFS  
//#define RT_NFS_HOST_EXPORT	"192.168.25.10:/"

#define LWIP_TCP_KEEPALIVE	1
/* ip address of target*/
#define RT_LWIP_IPADDR0	192
#define RT_LWIP_IPADDR1	168
#define RT_LWIP_IPADDR2	25
#define RT_LWIP_IPADDR3	15

/* gateway address of target*/
#define RT_LWIP_GWADDR0	192
#define RT_LWIP_GWADDR1	168
#define RT_LWIP_GWADDR2	25
#define RT_LWIP_GWADDR3	1

/* mask address of target*/
#define RT_LWIP_MSKADDR0	255
#define RT_LWIP_MSKADDR1	255
#define RT_LWIP_MSKADDR2	255
#define RT_LWIP_MSKADDR3	0

/* tcp thread options */
#define RT_LWIP_TCPTHREAD_PRIORITY		12
#define RT_LWIP_TCPTHREAD_MBOX_SIZE		10
#define RT_LWIP_TCPTHREAD_STACKSIZE		1024 //1024

/* ethernet if thread options */
#define RT_LWIP_ETHTHREAD_PRIORITY		15
#define RT_LWIP_ETHTHREAD_MBOX_SIZE		10
#define RT_LWIP_ETHTHREAD_STACKSIZE		512	 //512

/* TCP sender buffer space */
#define RT_LWIP_TCP_SND_BUF	8192
/* TCP receive window. */
#define RT_LWIP_TCP_WND		8192


/* SECTION: RT-Thread/GUI */
/* #define RT_USING_RTGUI */

/* name length of RTGUI object */
//#define RTGUI_NAME_MAX		12
/* support 16 weight font */
//#define RTGUI_USING_FONT16
/* support Chinese font */
//#define RTGUI_USING_FONTHZ
/* use DFS as file interface */
//#define RTGUI_USING_DFS_FILERW
/* use font file as Chinese font */
//#define RTGUI_USING_HZ_FILE
/* use Chinese bitmap font */
//#define RTGUI_USING_HZ_BMP
/* use small size in RTGUI */
//#define RTGUI_USING_SMALL_SIZE
/* use mouse cursor */
/* #define RTGUI_USING_MOUSE_CURSOR */
/* default font size in RTGUI */
//#define RTGUI_DEFAULT_FONT_SIZE	16

/* image support */
/* #define RTGUI_IMAGE_XPM */
/* #define RTGUI_IMAGE_BMP */

// <bool name="RT_USING_CMSIS_OS" description="Using CMSIS OS API" default="true" />
// #define RT_USING_CMSIS_OS
// <bool name="RT_USING_RTT_CMSIS" description="Using CMSIS in RTT" default="true" />
#define RT_USING_RTT_CMSIS
// <bool name="RT_USING_BSP_CMSIS" description="Using CMSIS in BSP" default="true" />
// #define RT_USING_BSP_CMSIS 

/*
*		���ݴ洢����������:
* 			����һ��ָ�������һ�����飬û�и�ָ�����ȥ
*			g_i_code_number :�Ѿ��ж�����ָ��
*			g_spi_code[]�����һ��������ʽ���ж�������ָͬ���ɶ��ٸ���������ֵ���ɡ�
*/
#define CODE_MAX 2048

#define VOLUME_INPUT_BASE_ADDR  (0x10000)

//�������ݵ���ʼλ�� 2016-11-09.
#define SPI_CODE_BASE_ADDR 		(0x000000)//��������淶Ҫ�ο�readme.txt.
#define REBOOT_TIMES_ADDR  		(0x060000)	

		     
//dsp�������ص�λ��
#define TFTP_ADDR 		0x20000
#define TFTP_DATA_ADDR 	0x21000

 

//�������
#define INPUT_PARA_SIZE (1+1+1+8+1+1)
//ͨ�ò���
#define GENERAL_PARA_SIZE (1+1+1+1+24+24+24+1+1+1+1+1+24*24+1+1+1)
//��������
#define OTHER_PARA_SIZE (1)
//��������
#define SOUND_PARA_SIZE ( 1 )
//�������
#define OUTPUT_PARA_SIZE 	(24*3+30+24+24+24+24+24+30*31)
//��Ƶ����
#define FREQDIV_PARA_SIZE 	(24 * 6)

#define SPI_CODE_SIZE 		(INPUT_PARA_SIZE * 7 + GENERAL_PARA_SIZE + OTHER_PARA_SIZE + SOUND_PARA_SIZE +\
									 OUTPUT_PARA_SIZE + FREQDIV_PARA_SIZE )
/////��������һ��Ԥ��,ָ����ôȥ�洢flash
#if ((SPI_CODE_SIZE * 4)<= 4*1024)
	#error "less than 4k ..."
	#define SECTOR_NUMBER 1

#elif(((SPI_CODE_SIZE * 4) > 4*1024) && ((SPI_CODE_SIZE * 4) <= 8*1024))
	#error "larger than 4k,but less than 8k..."
	#define SECTOR_NUMBER 2

#elif(((SPI_CODE_SIZE * 4) > 8*1024) && ((SPI_CODE_SIZE * 4) <= 12*1024))
	//#error "larger than 8k,but less than 12k..."
	#define SECTOR_NUMBER 3

#else  
	#error "larger than 12k..."
	#define SECTOR_NUMBER 4


#endif


#endif



