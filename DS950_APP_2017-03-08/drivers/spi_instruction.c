#include "spi_instruction.h"
#include <stm32f10x.h>
#include <rtthread.h>

#include "rt_stm32f10x_spi.h"
#include "rtdef.h"

struct spi_instruction_device
{
    struct rt_device                flash_device;
    struct rt_mutex                 lock;
};

static struct spi_instruction_device  spi_instruction_device;  

static void flash_lock(struct spi_instruction_device * instruction)
{
    rt_mutex_take(&instruction->lock, RT_WAITING_FOREVER);
}

static void flash_unlock(struct spi_instruction_device * instruction)
{
    rt_mutex_release(&instruction->lock);
}



/*
* 		执行与DSP沟通的指令
*		硬件接线:	
*				PB12/SPI2_NSS	DSP_CS
*				PB13/SPI2_SCK	DSP_SCK
*				PB14/SPI2_MISO	DSP_MISO
*				PB15/SPI2_MOSI	DSP_MOSI
*/
#define USE_SPI_DSP 1
		    

#define USE_SPI_INTERRUPT 0

#if  USE_SPI_DSP
#include "rt_stm32f10x_spi.h"

//5 DI data in
//2 DO data out
/*******************************************************************************
*******************************************************************************/
void SPI_DSP_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;					 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);		
	/*!< Configure SPI_FLASH_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*!< Configure SPI_FLASH_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*!< Configure SPI_FLASH_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*!< Configure SPI_FLASH_SPI_CS_PIN pin: Card CS pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Deselect the FLASH: Chip Select high */
	SPI_INSTRUCTION_CS_HIGH();
	/* SPI2 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;//SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);


#if USE_SPI_INTERRUPT
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

	SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);
}
#endif
	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);

#if USE_SPI_INTERRUPT
	SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
#endif

    //if (rt_mutex_init(&spi_instruction_device.lock, "code", RT_IPC_FLAG_FIFO) != RT_EOK)
    //{
    //    rt_kprintf("init instruction lock mutex failed\n");
    //     
    //}

}



/*
* 	stm32硬件spi发送数据
*
*
*/
unsigned char SPI_DSP_SendByte(unsigned char byte)
{

	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(SPI2, byte);
	
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

/*
* 		读取DSP传输过来的32位的数据，采用轮询方式
*
*/
unsigned int SPI_DSP_ReadByte(void)
{	    
 	int i = 0;
	unsigned int c_rcv1;
	unsigned int c_rcv2;
	unsigned int c_rcv3;
	unsigned int c_rcv4;
	
	//flash_lock(&spi_instruction_device);
				    
	SPI_INSTRUCTION_CS_LOW();		    
	for(i = 0 ; i < 10 ;i ++);	
	c_rcv1 = SPI_DSP_SendByte(0xff); 
	c_rcv2 = SPI_DSP_SendByte(0xff); 
	c_rcv3 = SPI_DSP_SendByte(0xff); 
	c_rcv4 = SPI_DSP_SendByte(0xff); 

	SPI_INSTRUCTION_CS_HIGH();
	for(i = 0 ; i < 10 ;i ++);	
	//flash_unlock(&spi_instruction_device);

  	return (c_rcv1 << 24) | (c_rcv2 << 16 ) | (c_rcv3 << 8) | c_rcv4;
}

/*		   
*
*
*  	 使用硬件spi给stm32发送数据
*
*/
void SPI_DSP_SendBytes(unsigned char *buffer, unsigned int len)
{
	int i = 0;		 
	unsigned char c_value;

	//flash_lock(&spi_instruction_device);

  	SPI_INSTRUCTION_CS_LOW();
	for(i = 0 ; i < 10 ;i ++);
	(void)c_value; //防止编译器报警告
	for(i = 0 ; i < len ; i ++)
	{
		c_value = SPI_DSP_SendByte(buffer[i]);
	}
	SPI_INSTRUCTION_CS_HIGH();
	for(i = 0 ; i < 20000 ;i ++);	
	//flash_unlock(&spi_instruction_device);
}


void SPI_DSP_SendBytes_using_tftp(unsigned char *buffer, unsigned int len)
{
	int i = 0;		 
	unsigned char c_value;
  	SPI_INSTRUCTION_CS_LOW();
	
	(void)c_value; //防止编译器报警告
	for(i = 0 ; i < len ; i ++)
	{
		c_value = SPI_DSP_SendByte(buffer[i]);
	}
	SPI_INSTRUCTION_CS_HIGH();
		
	 
}

#endif

