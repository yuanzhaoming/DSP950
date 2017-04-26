#include "IIC_CS4385.h"
#include "stm32f10x.h"

#if 0
static void Delay_us(int value)
{
	int i = 0,j; 
	for(i = 0 ; i < 5 ; i ++)
		for(j = 0 ; j < value ; j ++)
			;
}
	    
//////////////////////////////////////////////////////////////////////////
//			 	函数名		:		IIC_Init	
//			   	功能		:		初始化i2c
//				参数		:		void
//				作者		:		wit_yuan
//				时间		:		2014-11-07
//				
//				硬件连线	:       PB8 SCL
//									PB9 SDA
////////////////////////////////////////////////////////////////////////////
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;   //开漏输出

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8); 	//PA8,PA9 输出高
	GPIO_SetBits(GPIOB,GPIO_Pin_9); 	//PA8,PA9 输出高	    
}
//////////////////////////////////////////////////////////////////////////
//			 	函数名		:		IIC_Start	
//			   	功能		:		i2c起始信号
//				参数		:		void
//				作者		:		wit_yuan
//				时间		:		2014-11-07
////////////////////////////////////////////////////////////////////////////
void IIC_Start(void)
{
	SDA_OUT();     	//由于上一个SCL状态是0或者是1，要让SDA稳定输出，都可以设置为1。
	IIC_SDA=1;
	Delay_us(2);
		  	  
	IIC_SCL=1;	    //让SCL发生变化，为0不变，为1则变化，表明数据可以变动了。
	Delay_us(2);

 	IIC_SDA=0;		//SDA线数据变化，从而可以保证发出的是start信号。 
	Delay_us(2);
	
	IIC_SCL=0;		//将I2C总线钳住，下一个时间SDA可以输出高低电平。
	Delay_us(4);
}	  
//////////////////////////////////////////////////////////////////////////
//			 	函数名		:		IIC_Stop	
//			   	功能		:		i2c结束信号
//				参数		:		void
//				作者		:		wit_yuan
//				时间		:		2014-11-07
////////////////////////////////////////////////////////////////////////////
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SDA=0;		//上一个状态的SCL=0,这个状态设置SDA=0,目的是让状态能反转
	Delay_us(2);	

	IIC_SCL=1;		//该时刻设置SCL=1,然后就可以让数据稳定在改状态下。
 	Delay_us(2);	

	IIC_SDA=1;
	Delay_us(4);							   	
}
//////////////////////////////////////////////////////////////////////////
//			 	函数名		:		IIC_Wait_Ack	
//			   	功能		:		等待i2c的应答信号
//				参数		:		void
//				作者		:		wit_yuan
//				时间		:		2014-11-07
////////////////////////////////////////////////////////////////////////////
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;

	IIC_SCL=0;
	Delay_us(2);							  		   		

	SDA_IN();      				//SDA设置为输入  	   
		 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;			//超时，表明数据传输有问题
		}
	}
	IIC_SCL=1;
	Delay_us(1);


	IIC_SCL=0;//时钟输出0 
	Delay_us(2);
					   
	return 0;  
} 
//////////////////////////////////////////////////////////////////////////
//			 	函数名		:		IIC_Ack	
//			   	功能		:		产生i2c的ack应答信号
//				参数		:		void
//				作者		:		wit_yuan
//				时间		:		2014-11-07
////////////////////////////////////////////////////////////////////////////
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	Delay_us(2);

	//added by wit_yuan 2016-09-16
	SDA_OUT();
	IIC_SDA = 1;
	Delay_us(1);


	IIC_SDA=0;
	Delay_us(1);


	IIC_SCL=1;
	Delay_us(1);
	IIC_SCL=0;
	Delay_us(4);

	//////add 2016-09-16 by wit_yuan///////////
	IIC_SDA = 1;
	Delay_us(1);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	SDA_OUT();
	IIC_SCL=0;
	Delay_us(2);

	IIC_SDA=1;
	Delay_us(2);

	IIC_SCL=1;
	Delay_us(2);
	IIC_SCL=0;
	Delay_us(2);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;
	//Delay_us(1);  
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
	Delay_us(2);
    for(t=0;t<8;t++)
    {
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		Delay_us(2);   
		IIC_SCL=1;
		Delay_us(2); 
		IIC_SCL=0;	
		Delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
unsigned char IIC_Read_Byte( void )
{
	unsigned char i,u_receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        Delay_us(4);
		IIC_SCL=1;
		Delay_us(1);
        u_receive<<=1;
        if(READ_SDA)
			u_receive++;   
    }

    return u_receive;
}			  



unsigned char cs4385_write(unsigned char c_slave_address7bit,unsigned char c_reg_address,unsigned char u_data)
{
	unsigned char u_wait_err = 0;

	IIC_Start();
	IIC_Send_Byte(c_slave_address7bit << 1);    
	u_wait_err |= IIC_Wait_Ack();

	IIC_Send_Byte(c_reg_address);   
	u_wait_err |= IIC_Wait_Ack();
	
	IIC_Send_Byte(u_data);
	u_wait_err |= IIC_Wait_Ack();

	IIC_Stop();	

	if(	u_wait_err == 0)
		return 0;
	return 1;
}

unsigned char cs4385_read(unsigned char c_slave_address7bit,unsigned char c_reg_address)
{	
	unsigned char u_temp;
	unsigned char u_wait_err = 0;

	IIC_Start();
	IIC_Send_Byte(c_slave_address7bit << 1);    
	u_wait_err |= IIC_Wait_Ack();

	IIC_Send_Byte(c_reg_address);    
	u_wait_err |= IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte((c_slave_address7bit << 1)+1);    
	u_wait_err |= IIC_Wait_Ack();
		
	u_temp = IIC_Read_Byte( );
	IIC_NAck();//不需要响应	

	IIC_Stop();

	return  u_temp;
}

/*
*
* 	该函数主要用来将CS4385设置为I2S模式还是TDM模式
*			0:I2S模式
*			1:TDM模式
*/
int cs4385_set_iic_mode(int i_chose_mode)
{
	int value;
	//先读取两个CS4385的地址，如果有问题，上报错误
	value = cs4385_read(CS4385_SLAVE_ADDRESS_1,0x01);
	if((value >> 3) != 0x01)
		return -1;
	value = cs4385_read(CS4385_SLAVE_ADDRESS_2,0x01);
	if((value >> 3) != 0x01)
		return -1;

	switch(i_chose_mode)
	{
		case 0:
			value = (1<< 7) | (1<<6) | (0<<0);
			cs4385_write(CS4385_SLAVE_ADDRESS_1,0x02,value);			
			cs4385_write(CS4385_SLAVE_ADDRESS_1,0x03,(0x01 << 4) | (3 << 0) );
			value = cs4385_read(CS4385_SLAVE_ADDRESS_1,0x02);
			cs4385_write(CS4385_SLAVE_ADDRESS_1,0x02,value & ~(1<<6));

			value = (1<< 7) | (1<<6) | (0<<0);
			cs4385_write(CS4385_SLAVE_ADDRESS_2,0x02,value);			
			cs4385_write(CS4385_SLAVE_ADDRESS_2,0x03,(0x01 << 4) | (3 << 0) );
			value = cs4385_read(CS4385_SLAVE_ADDRESS_2,0x02);
			cs4385_write(CS4385_SLAVE_ADDRESS_2,0x02,value & ~(1<<6));
		break;
		case 1:
			value = (1<< 7) | (1<<6) | (0<<0);
			cs4385_write(CS4385_SLAVE_ADDRESS_1,0x02,value);			
			cs4385_write(CS4385_SLAVE_ADDRESS_1,0x03,(0x0C << 4) | (3 << 0) );
			value = cs4385_read(CS4385_SLAVE_ADDRESS_1,0x02);
			cs4385_write(CS4385_SLAVE_ADDRESS_1,0x02,value & ~(1<<6));			

			value = (1<< 7) | (1<<6) | (0<<0);
			cs4385_write(CS4385_SLAVE_ADDRESS_2,0x02,value);			
			cs4385_write(CS4385_SLAVE_ADDRESS_2,0x03,(0x0C << 4) | (3 << 0) );
			value = cs4385_read(CS4385_SLAVE_ADDRESS_2,0x02);
			cs4385_write(CS4385_SLAVE_ADDRESS_2,0x02,value & ~(1<<6));
		break;
	}
	return 0;	
}
#endif


