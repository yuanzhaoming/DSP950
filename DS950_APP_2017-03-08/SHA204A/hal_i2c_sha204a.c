#include "hal_i2c_sha204a.h"
#include "inc.h"
#include "delay.h"
#include <rtthread.h>

#define SLAVE_ADDR_DEFAULT	(0xC8 >> 1)

static uint8_t slave_address = 0;

#define SLAVE_ADDR		(slave_address)


#define SCL_DELAY_US	5
#define SDA_DELAY_US	(SCL_DELAY_US - 1)	    

#define IDLE_DELAY_US	(SDA_DELAY_US / 2)

enum SDA_Pin_Directions {
	SDA_IN,
	SDA_OUT,
};

#define SDA_SET(BitVal)	GPIO_WriteBit(SIIC_GPIO_PORT, SIIC_SDAPin, (BitAction)BitVal)
#define SCL_SET(BitVal)	GPIO_WriteBit(SIIC_GPIO_PORT, SIIC_SCLPin, (BitAction)BitVal)


static uint8_t SI2C_Send_Byte(uint8_t TxByte);

/*************************************************************************
  * @brief  SIIC slave driver address configuration
  * @param  None
  * @retval None   
  * @date   20160830
**************************************************************************/
void SI2C_Set_Slave_7BitAddress(uint8_t SlaveAddr7Bit)
{
	//设置设备I2C地址(7位地址)
	SLAVE_ADDR = SlaveAddr7Bit;
}

/*************************************************************************
  * @brief  SIIC SDA pin directions configuration
  * @param  None
  * @retval None   
  * @date   20160830
**************************************************************************/
static void SI2C_SDA_Pin_Directions(enum SDA_Pin_Directions SDA_Directions)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	SIIC_GPIO_Cmd(SIIC_GPIO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SIIC_SDAPin;
	
	if (SDA_Directions == SDA_IN)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置为浮空输入
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //设置为开漏输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	}
	
	GPIO_Init(SIIC_GPIO_PORT, &GPIO_InitStructure);
}

#define SDA_IN()	SI2C_SDA_Pin_Directions(SDA_IN)
#define SDA_OUT()	SI2C_SDA_Pin_Directions(SDA_OUT)

#define SDA_GET()		GPIO_ReadInputDataBit(SIIC_GPIO_PORT, SIIC_SDAPin)

/*********************************************************************
  * @brief  SIIC Init (SIIC GPIO configuration)
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
void SI2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	SIIC_GPIO_Cmd(SIIC_GPIO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SIIC_SCLPin | SIIC_SDAPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //设置为开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SIIC_GPIO_PORT, &GPIO_InitStructure);

	//设置默认状态SCL和SDA为高
	GPIO_SetBits(SIIC_GPIO_PORT, SIIC_SCLPin | SIIC_SDAPin);
	
	//设置设备I2C地址(7位地址)
	SLAVE_ADDR = SLAVE_ADDR_DEFAULT;
}

/*********************************************************************
  * @brief  SIIC起始信号
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
static void SI2C_Start()
{
	SDA_OUT();
	SDA_SET(1);
	Delay_us(SDA_DELAY_US);
	
	SCL_SET(1);
	Delay_us(SCL_DELAY_US);
	
	SDA_SET(0);
	Delay_us(SDA_DELAY_US);
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
}

/*********************************************************************
  * @brief  SIIC结束信号
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
static void SI2C_Stop()
{
	SDA_OUT();
	SDA_SET(0);
	Delay_us(SDA_DELAY_US);
	
	SCL_SET(1);
	Delay_us(SCL_DELAY_US);
	
	SDA_SET(1);
	Delay_us(SDA_DELAY_US);
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
}

/*********************************************************************
  * @brief  SIIC应答信号
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
#if 0
static void SI2C_Ack()
{
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	SDA_OUT();
	SDA_SET(0);
	Delay_us(SDA_DELAY_US);
	
	SCL_SET(1);
	Delay_us(SCL_DELAY_US);
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
}
#else
static void SI2C_Ack()
{
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	SDA_OUT();
	SDA_SET(1);
	Delay_us(SDA_DELAY_US);
	
	SDA_SET(0);
	Delay_us(SDA_DELAY_US);
	
	SCL_SET(1);
	Delay_us(SCL_DELAY_US);
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	SDA_SET(1);
	Delay_us(SDA_DELAY_US);
}
#endif

/*********************************************************************
  * @brief  SIIC非应答信号
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
static void SI2C_NAck()
{
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	SDA_OUT();
	SDA_SET(1);
	Delay_us(SDA_DELAY_US);
	
	SCL_SET(1);
	Delay_us(SCL_DELAY_US);
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
}

/*********************************************************************
  * @brief  SIIC等待从设备响应信号
  * @param  None
  * @retval    
  * @date   20160830
***********************************************************************/
static uint8_t SI2C_Wait_Ack()
{
	uint8_t is_time_out = 0;
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	SDA_IN();
	
	while (SDA_GET())
	{
		++is_time_out;
		if (is_time_out > 250)
		{
			SI2C_Stop();
			
			return 1;
		}
	}
	
	SCL_SET(1);
	Delay_us(SCL_DELAY_US);
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	return 0;
}

/*********************************************************************
  * @brief  SI2C Send Byte
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
static uint8_t SI2C_Send_Byte(uint8_t TxByte)
{
	uint8_t i = 0;
	
	SDA_OUT();
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	for (i = 0; i < 8; ++i)
	{
		if (TxByte & 0x80)
		{
			SDA_SET(1);
		}
		else
		{
			SDA_SET(0);
		}
		
		TxByte <<= 1;
		Delay_us(SDA_DELAY_US);
		
		SCL_SET(1);
		Delay_us(SCL_DELAY_US);
		
		SCL_SET(0);
		Delay_us(SCL_DELAY_US);
	}
	

	if (SI2C_Wait_Ack() != 0)
	{
		//此处必须有延时,不然会出错,至少延时1ms,由于只有出错才会进入此处(次数少),可以多延时一点,取5ms
		Delay_us(5000);
		
		SI2C_Stop();
		
		return 1;
	}
	
	return 0;
}

/*********************************************************************
  * @brief  SI2C Recv Byte
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
static uint8_t SI2C_Recv_Byte()
{
	uint8_t i = 0;
	uint8_t recv = 0;
	
	SDA_IN();
	
	for (i = 0; i < 8; ++i)
	{
		SCL_SET(0);
		Delay_us(SCL_DELAY_US);
		
		SCL_SET(1);
		Delay_us(SCL_DELAY_US);
		
		recv <<= 1;
		if (SDA_GET())
		{
			++recv;
		}
	}
	
	SI2C_Ack();
	Delay_us(IDLE_DELAY_US);
	
	return recv;
}

/*
 *以下部分是与ATSHA204A相关的接口,它并非标准的I2C协议,需要重新实现读写
 */

/*********************************************************************
  * @brief  SI2C send many byte
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
static uint8_t SI2C_Send_Bytes(uint8_t *TxBytes, uint8_t TxByteCount)
{
	uint8_t i = 0;
	uint8_t data = 0;
	
	SDA_OUT();
	
	SCL_SET(0);
	Delay_us(SCL_DELAY_US);
	
	for (i = 0; i < TxByteCount; ++i)
	{
		data = TxBytes[i];
		if (SI2C_Send_Byte(data) != 0)
		{
			SI2C_Stop();
			
			return 1;
		}
	}
	
	return 0;
}

/*********************************************************************
  * @brief  SI2C recv many byte
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
static void SI2C_Recv_Bytes(uint8_t *RxBytes, uint8_t RxByteCount)
{
	uint8_t i = 0;

	for (i = 0; i < RxByteCount - 1; ++i)
	{
	//	Delay_us(IDLE_DELAY_US);
		RxBytes[i] = SI2C_Recv_Byte();
	}
	
	RxBytes[i] = SI2C_Recv_Byte();
	SI2C_NAck();
}


void I2C_SHA204A_Wake()
{
	SI2C_Start();
	Delay_us(85);
	
	SI2C_Send_Byte(0x00);
	Delay_us(60);
	SI2C_Stop();
	Delay_us(3800);
}

/*********************************************************************
  * @brief  SI2C Read many byte from slave device
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
uint8_t I2C_SHA204A_Read(uint8_t *RxBytes, uint8_t RxByteLength)
{
	uint8_t count = 0;
	
	SI2C_Start();
	if (SI2C_Send_Byte((SLAVE_ADDR << 1) + 1) != 0)
	{
	//	SI2C_Stop();
		rt_kprintf("1.send err\r\n");
		
		return 1;
	}

	//read count byte
	count = SI2C_Recv_Byte();
	if ((count < 4) || (count > RxByteLength))
	{
		rt_kprintf("2.[%d]count err\r\n", count);
		return 1;
	}
	
	RxBytes[0] = count;
	
	SI2C_Recv_Bytes(&RxBytes[1], count - 1);
	
	SI2C_Stop();
	
	return 0;
}

/*********************************************************************
  * @brief  SI2C Write many byte from slave device
  * @param  None
  * @retval None   
  * @date   20160830
***********************************************************************/
uint8_t I2C_SHA204A_Write(uint8_t WordAddress, uint8_t *TxBytes, uint8_t TxByteLength)
{
	uint8_t ret = 0;
	
	SI2C_Start();
	if (SI2C_Send_Byte(SLAVE_ADDR << 1) != 0)
	{
		SI2C_Stop();
		
		return 1;
	}
	
	//write WordAddress(data type)
	if (SI2C_Send_Bytes(&WordAddress, 1) != 0)
	{
		return 1;
	}
	
	if (TxByteLength == 0) {
		// We are done for packets that are not commands (Sleep, Idle, Reset).
		SI2C_Stop();
		
		return 0;
	}
	
	ret = SI2C_Send_Bytes(TxBytes, TxByteLength);
	SI2C_Stop();
	
	return ret;
}

uint8_t I2C_SHA204A_Resync()
{
	uint8_t nine_clocks = 0xFF;
	uint8_t ret_code;

	SI2C_Stop();
	Delay_ms(1);
	SI2C_Start();

	// Do not evaluate the return code that most likely indicates error,
	// since nine_clocks is unlikely to be acknowledged.
	if (SI2C_Send_Bytes(&nine_clocks, 1) != 0)
	{
		rt_kprintf("11111111111\r\n");
		return 1;
	}
	
	SI2C_Start();
	ret_code = SI2C_Send_Byte((SLAVE_ADDR << 1) + 1);
	SI2C_Stop();
	if (ret_code != 0)
	{
		rt_kprintf("22222222222\r\n");
		return 1;
	}

	return 0;
}


