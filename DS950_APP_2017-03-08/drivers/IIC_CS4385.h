#ifndef _i2c_H_
#define _i2c_H_

#include "stm32f10x.h"
#define countof(a)    (sizeof(a)/sizeof(*(a)))

#define GPIOA_IDR_Addr    (GPIOA_BASE + 8)  //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE + 8)  //


/*λ������,ʵ��51���Ƶ�GPIO���ƹ���*/
/*IO�ڲ����궨�� */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+ \
                              0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����
#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����
#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

//IO��������
#define SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=3<<4;}

//IO��������	 
#define IIC_SCL    PBout(8) //SCL---PB8
#define IIC_SDA    PBout(9) //SDA---PB9
#define READ_SDA   PBin(9)  //����SDA 


//#define	EEPROM_SLAVE_ADDRESS	(0x50) 		
#define	CS4385_SLAVE_ADDRESS_1	(0x18)
#define	CS4385_SLAVE_ADDRESS_2	(0x19)


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
unsigned char IIC_Read_Byte( void );//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

unsigned char cs4385_write(unsigned char c_slave_address7bit,unsigned char c_reg_address,unsigned char u_data);
unsigned char cs4385_read(unsigned char c_slave_address7bit,unsigned char c_reg_address);


#endif
