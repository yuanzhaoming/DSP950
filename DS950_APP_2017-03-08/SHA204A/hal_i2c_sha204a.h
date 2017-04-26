/**
  ******************************************************************************
  * @file    hal_i2c_sha204a.h
  * @author  wanghb
  * @version V0.0.0
  * @date    2016-08-29
  * @brief   ATSHA204A�����շ��ӿ�(I2C),ʹ����ͨGPIOģ��I2C�ӿ�
  */
 
#ifndef __HAL_I2C_SHA204A_H
#define __HAL_I2C_SHA204A_H

#include <stdio.h>
#include <stm32f10x.h>

#if 0
/***************************************************************
 *ģ��I2C�ӿڵ�ͨ��GPIO�ܽ�:
 *IIC_SCL    PC2
 *IIC_SDA    PC3
****************************************************************/

#define SIIC_GPIO_Cmd			RCC_APB2PeriphClockCmd
#define SIIC_GPIO_CLK			RCC_APB2Periph_GPIOC

#define SIIC_GPIO_PORT			GPIOC
#define SIIC_SCLPin				GPIO_Pin_2
#define SIIC_SDAPin				GPIO_Pin_3

#else

/***************************************************************
 *ģ��I2C�ӿڵ�ͨ��GPIO�ܽ�:
 *IIC_SCL    PB8
 *IIC_SDA    PB9
****************************************************************/

#define SIIC_GPIO_Cmd			RCC_APB2PeriphClockCmd
#define SIIC_GPIO_CLK			RCC_APB2Periph_GPIOB

#define SIIC_GPIO_PORT			GPIOB
#define SIIC_SCLPin				GPIO_Pin_8
#define SIIC_SDAPin				GPIO_Pin_9

#endif


void SI2C_Set_Slave_7BitAddress(uint8_t SlaveAddr7Bit);


void SI2C_Init(void);

/*
 *���²�������ATSHA204A��صĽӿ�,�����Ǳ�׼��I2CЭ��,��Ҫ����ʵ�ֶ�д
 */

void I2C_SHA204A_Wake(void);

uint8_t I2C_SHA204A_Read(uint8_t *RxBytes, uint8_t RxByteLength);
uint8_t I2C_SHA204A_Write(uint8_t WordAddress, uint8_t *TxBytes, uint8_t TxByteLength);

uint8_t I2C_SHA204A_Resync(void);

#endif /*__HAL_I2C_SHA204A_H*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
