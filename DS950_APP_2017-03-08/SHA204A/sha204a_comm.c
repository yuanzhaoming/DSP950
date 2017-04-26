#include "sha204A_comm.h"
#include "stdio.h"
#include "inc.h"
#include "hal_i2c_sha204a.h"
#include <rtthread.h>
#include "delay.h"
#include "string.h"	    


											       
#define sha_debug 0

#if sha_debug
	#define sha_debug_fun(fmt, ...)	  rt_kprintf("[%s][%d]"fmt"\n",__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define sha_debug_fun(fmt, ...)
#endif


//static void print_buffer(uint8_t *header, uint8_t *buffer);

void SHA204Ac_calculate_crc(uint8_t length, uint8_t *data, uint8_t *crc)
{
	uint8_t counter;
	uint16_t crc_register = 0;
	uint16_t polynom = 0x8005;
	uint8_t shift_register;
	uint8_t data_bit, crc_bit;

	for (counter = 0; counter < length; counter++)
	{
		for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1)
		{
			data_bit = (data[counter] & shift_register) ? 1 : 0;
			crc_bit = crc_register >> 15;

			// Shift CRC to the left by 1.
			crc_register <<= 1;

			if ((data_bit ^ crc_bit) != 0)
				crc_register ^= polynom;
		}
	}
	crc[0] = (uint8_t) (crc_register & 0x00FF);
	crc[1] = (uint8_t) (crc_register >> 8);
}

uint8_t SHA204Ac_check_crc(uint8_t *response)
{
	uint8_t crc[SHA204_CRC_SIZE] = {0};
	uint8_t count = response[0];

	count -= SHA204_CRC_SIZE;
	SHA204Ac_calculate_crc(count, response, crc);

	return ((crc[0] == response[count]) && (crc[1] == response[count + 1])) ? 0 : 1;
}


uint8_t SHA204Ac_wakeup(void)
{
	uint8_t read_buf[SHA204_CMD_SIZE_MIN] = {0};
	
	I2C_SHA204A_Wake();
	
	if (I2C_SHA204A_Read(read_buf, SHA204_CMD_SIZE_MIN) != 0)
	{
		sha_debug_fun("I2C_SHA204A_Read error\r\n");
		
		return 1;
	}
	
	if (SHA204Ac_check_crc(read_buf) != 0)
	{
		sha_debug_fun("sha204c_check_crc: data CRC error\r\n");
	}
	
	// Verify status response.
	if (read_buf[SHA204_BUFFER_POS_COUNT] != SHA204_RSP_SIZE_MIN)
		return 1;
	else if (read_buf[SHA204_BUFFER_POS_STATUS] != SHA204_STATUS_BYTE_WAKEUP)
		return 1;
	else {
		if ((read_buf[SHA204_RSP_SIZE_MIN - SHA204_CRC_SIZE] != 0x33)
			|| (read_buf[SHA204_RSP_SIZE_MIN + 1 - SHA204_CRC_SIZE] != 0x43))
			return 1;
	}
	
	return 0;
}

uint8_t SHA204Ac_idle(void)
{
	return I2C_SHA204A_Write(SHA204_I2C_PACKET_FUNCTION_IDLE, NULL, 0);
}

uint8_t SHA204Ac_sleep(void)
{
	return I2C_SHA204A_Write(SHA204_I2C_PACKET_FUNCTION_SLEEP, NULL, 0);
}

uint8_t SHA204Ac_reset(void)
{
	return I2C_SHA204A_Write(SHA204_I2C_PACKET_FUNCTION_RESET, NULL, 0);
}

uint8_t SHA204Ac_send_cmd(uint8_t *CmdBuffer, uint8_t CmdLength)
{
	return I2C_SHA204A_Write(SHA204_I2C_PACKET_FUNCTION_NORMAL, CmdBuffer, CmdLength);
}

uint8_t SHA204Ac_recv_response(uint8_t *ResponseBuffer, uint8_t ResponseLength)
{
	return I2C_SHA204A_Read(ResponseBuffer, ResponseLength);
}

uint8_t SHA204Ac_Resync()
{
	uint8_t flag = 0;
	if (I2C_SHA204A_Resync() != 0)
	{
		flag +=1;
		goto err;
	}
	
	// Try to send a Reset IO command if re-sync succeeded.
	if (SHA204Ac_reset() != 0)
	{
		flag <<= 1;
		flag += 1;
		goto err;
	}
	
	if (SHA204Ac_sleep() != 0)
	{
		flag <<= 1;
		flag += 1;
		goto err;
	}
	
	if (SHA204Ac_wakeup() != 0)
	{
		flag <<= 1;
		flag += 1;
		goto err;
	}

	sha_debug_fun("SHA204Ac_Resync ok\r\n");
	return 0;

	err:
	sha_debug_fun("[%02X]SHA204Ac_Resync error\r\n", flag);
	return 1;
}

#if 0								 
static void print_buffer(uint8_t *header, uint8_t *buffer)
{
#ifdef DEBUG_GLOBAL
	uint8_t i = 0;
	uint8_t count = 0;
	
	if ((header == NULL) || (buffer == NULL))
		return;
	
	count = buffer[0];
	sha_debug_fun("%s(0X):", header);
	for (i = 0; i < count; ++i)
	{
		sha_debug_fun(" %02X", buffer[i]);
	}
	sha_debug_fun("\r\n");
#endif /* MYSQL_DEBUG */
}
#endif

uint8_t SHA204Ac_send_and_recv(uint8_t *TxBuffer, uint8_t *RxBuffer, uint8_t RxLength, uint32_t ExecutionDelay_us)
{
	uint8_t count = 0;
	uint8_t count_minus_crc = 0;
	
	// Append CRC
	count = TxBuffer[SHA204_BUFFER_POS_COUNT];
	count_minus_crc = count - SHA204_CRC_SIZE;
	SHA204Ac_calculate_crc(count_minus_crc, TxBuffer, TxBuffer + count_minus_crc);
	if (SHA204Ac_check_crc(TxBuffer) != 0)
	{
		sha_debug_fun("TxBuffer CRC error\r\n");
		
		return 1;
	}
	
	//print_buffer((uint8_t *)"TxBuffer", TxBuffer);
	
	count = TxBuffer[0];
	if (SHA204Ac_send_cmd(TxBuffer, count) != 0)
	{
		sha_debug_fun("SHA204Ac_send_cmd error\r\n");
		
		return 1;
	}
	
	//此处必须有延时(打印调试信息也算延时),否则读取会出错,经测试,延时至少为4000us,为了保证正确读取,延时4500us
#if 0
//	Delay_us(5000);
#else
	sha_debug_fun("SHA204Ac_send_cmd ok");
#endif
	
	//此处延时为指令执行时间(由官方源码可知),可以为0us
//	Delay_us(ExecutionDelay_us);
	
//	Delay_ms(500);
	Delay_ms(12 * (5 + 5)); //官方源代码延时 * (SDA延时 + SCL延时)
	
	memset(RxBuffer, 0, RxLength);
	if (SHA204Ac_recv_response(RxBuffer, RxLength) != 0)
	{
		sha_debug_fun("SHA204Ac_recv_response error\r\n");
		
		return 1;
	}
	else
	{
		if (SHA204Ac_check_crc(RxBuffer) != 0)
		{
			sha_debug_fun("RxBuffer CRC error\r\n");
			
			return 1;
		}
		
		//print_buffer((uint8_t *)"Response", RxBuffer);
		return 0;
	}
}



