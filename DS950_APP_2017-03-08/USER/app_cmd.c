#include "app_cmd.h"
#include <finsh.h>
#include "rtconfig.h"
#include "inc.h"
#include "spi_instruction.h"
#include "spi_flash.h"
#include "app_cmd.h"

/*	 	    
* 		time	:	2016-12-06 
*		author	:	yuan
*	   	function:   用户命令，将配置文件中的命令发送给dsp
*
*/		   
void send_config_dsp( void )
{
	send_data_to_dsp();
}
FINSH_FUNCTION_EXPORT(send_config_dsp, send all configurations to dsp);



unsigned char c_sectors_name[4][15]={"spi code","volume","dsp file","reboot times"};	   
/*
* 		time	:	2016-12-08 
*		author	:	yuan
*	   	function:   用户命令，将flash上的存储位置信息打印出来
*
*/
void flash_list( void )
{
	rt_kprintf("\n--list flash sectors--\n");
	rt_kprintf("\n  setctor name                  start addr          end addr\n");
	
	rt_kprintf("%20s            0x%06x                0x%06x\n",c_sectors_name[0],SPI_CODE_BASE_ADDR,
						(SPI_CODE_BASE_ADDR + 0x1000) + SECTOR_NUMBER * 0x1000);

	rt_kprintf("%20s            0x%06x                0x%06x\n",c_sectors_name[1],VOLUME_INPUT_BASE_ADDR,
						(VOLUME_INPUT_BASE_ADDR + 0x1000) + 0x1000);

							
	rt_kprintf("%20s            0x%06x                0x%06x\n",c_sectors_name[2],TFTP_ADDR,
						(TFTP_ADDR + 0x1000) + 0x1000);		
	rt_kprintf("%20s            0x%06x                0x%06x\n",c_sectors_name[3],REBOOT_TIMES_ADDR,
						(REBOOT_TIMES_ADDR + 0x1000));		
			 
}


FINSH_FUNCTION_EXPORT(flash_list, list flash sectors for reading);


	   

void flash_erase_sec( unsigned char *c_name )
{
	int i = 0;
	int j = 0;
	
	for(i = 0 ;  i < 3 ; i ++)
	{			 		  
		if(strncmp((const char *)c_name,(const char *)c_sectors_name[i],strlen((const char *)c_sectors_name[i])) == 0)
		{
			rt_kprintf("\n--erasing %s--\n",c_sectors_name[i]);	
			break ;
		}   
	}
	
	//spi code
	if(i == 0)
	{
		for(j = 0 ; j < SECTOR_NUMBER * 2 + 2 ; j ++)
		{
			SPI_FLASH_SectorErase(SPI_CODE_BASE_ADDR + j * 0x1000);	
		}
	}
	//volume
	if(i == 1)
	{
		for(j = 0 ; j < 2 * 2 ; j ++)
		{
			SPI_FLASH_SectorErase(VOLUME_INPUT_BASE_ADDR + j * 0x1000);	
		}
	}

	//dsp file
	if(i == 2)
	{
		for(j = 0 ; j < 2 ; j ++)
		{
			SPI_FLASH_SectorErase(TFTP_ADDR + j * 0x1000);	
		}
	}		
 	//reboot times
	if(i == 3)
	{
		for(j = 0 ; j < 1 ; j ++)
		{
			SPI_FLASH_SectorErase(REBOOT_TIMES_ADDR + j * 0x1000);	
		}
	}			 
}
FINSH_FUNCTION_EXPORT(flash_erase_sec, erasing flash using names..param("name"));
/*
* 		time	:	2016-12-10 
*		author	:	yuan
*	   	function:   用户命令，给dsp发送mute信号
*
*/
extern int g_mute;	
void mute(int i_value)
{
	int i_temp = 0x05E00000 | (i_value << 16); 
	g_mute = i_value; 
	SPI_DSP_SendBytes((unsigned char *)&i_temp,4);
	rt_kprintf("mute:%d \n",g_mute);
}
FINSH_FUNCTION_EXPORT(mute, mute the dsp or not);


extern int FlashID,DeviceID;

void flash_info( void )
{
	rt_kprintf("\r\n FlashID: 0x%X\r\n", FlashID);
	rt_kprintf(" Manufacturer Device ID: 0x%X\r\n",DeviceID);
}
FINSH_FUNCTION_EXPORT(flash_info, the flash info);


/*
* 		time	:	2016-12-15 
*		author	:	yuan
*	   	function:   用户命令，擦除stm32的flash片区
*
*/
void flash_erase(int i_start_addr,int i_sector_num)
{
	int i = 0;
	//写如参数数量不正确，进入if部分
	if(i_sector_num > 0x8000000)
	{
		  rt_kprintf("\r\n param error %0x,%0x\n",i_start_addr,i_sector_num);
		  return;
	}

	for(i = 0 ; i < i_sector_num ; i ++)
	{
		SPI_FLASH_SectorErase(i_start_addr + i * 0x1000);	
	}
}
FINSH_FUNCTION_EXPORT(flash_erase, erase sectors.param one(addr)..param two(sectors)...);


void clear(void)
{
	rt_kprintf("\033[H\033[J");
}
FINSH_FUNCTION_EXPORT(clear, clear crt...null(args));


int g_i_debug = 0;

int get_debug_value(void)
{
	return g_i_debug;
}   


void list_debug(void)
{
	rt_kprintf("network debug 0x01 \r\n");		
	rt_kprintf("flash   debug 0x02 \r\n");	

}
FINSH_FUNCTION_EXPORT(list_debug, list debug info...null(args));
		  

void net_debug(int i_on)
{
	if(	i_on )
	{
		g_i_debug |= (1<<0);
	}
	else
	{
		g_i_debug &= ~(1<<0);
	}	
}
FINSH_FUNCTION_EXPORT(net_debug, list net debug info...one(args));


void flash_debug(int i_on)
{
	if(i_on)
		g_i_debug |= (1<<1);
	else
		g_i_debug &= ~(1<<1);		
}
FINSH_FUNCTION_EXPORT(flash_debug, list flash debug info...one(args));



void clear_debug( )
{
	g_i_debug = 0;	
}
FINSH_FUNCTION_EXPORT(clear_debug, list net debug info...null(args));











