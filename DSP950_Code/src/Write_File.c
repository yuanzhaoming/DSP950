#include "./include/write_file.h"
#include <stdio.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sysreg.h>
#include "math.h"

static int g_iCount = 0;

#define USE_WRITE_FILE 0

#define WRITE_FILE_SIZE 40*1024

#if USE_WRITE_FILE
unsigned char g_buf[WRITE_FILE_SIZE];
#endif
 
void New_File()
{
#if USE_WRITE_FILE
	FILE* file_p;
	int  i;	
		
	file_p = fopen("../a.wav","wb");	
	if(file_p == NULL)
	{
		printf("file open bad\n");
	}
	else
	{
		printf("file open ok\n");		
	}
	
	printf("----------write file start------");

	//写文件
	fwrite(g_buf,1,WRITE_FILE_SIZE,file_p);	
	
	fclose(file_p);	
	
	printf("----------write file end-----");
#endif	
			
}


void Write_File(unsigned char *buf,unsigned int len)
{
#if USE_WRITE_FILE
	static int g_iCount = 0;
	static int g_iCountNumber = 0;//作为标志量。写文件开始的标志
	int i = 0;
	
	
	//由于和stm32要同步，因此需要延迟一会儿，再去读取数据，写文件
	static int i_times= 0;
	
	
	if(i_times <= 1000)
	{
		i_times ++;
		return;	
	}
	
	g_iCount ++ ;
	
	//前期只需要复制数组内容即可
	if(g_iCountNumber == 0)
	{
		if(g_iCount < 1 * WRITE_FILE_SIZE / len)
		{
			for(i = 0 ;i < len; i ++)
			{
				g_buf[ i + g_iCount * len ] = buf[i] >> 16;	
			}			
		}
		else
		{
			g_iCountNumber = 1;	
		}
				
	}
	//可以写文件
	if(g_iCountNumber == 1)
	{
	#if 0
		for(i = 1000 ;i < 1100; i ++)
		{
			printf("0x%0x ",g_buf[i]);	
		}		
	#endif
		New_File();
		//写完文件，等待即可
		while(1);	
	}
#endif
}

