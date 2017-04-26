#include "sha204a.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "inc.h"
//#include "hal_uart.h"
#include "hal_i2c_sha204a.h"
#include "sha204A_comm.h"
#include "sha204a_comm_marshaling.h"
#include "sha204_data.h"
#include "sha204_authentication.h"
#include "delay.h"
#include "string.h"
#include <rtthread.h>


#define sha204_debug 0

#if sha204_debug
	#define sha204_debug_fun(fmt, ...)	  rt_kprintf("[%s][%d]"fmt"\n",__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define sha204_debug_fun(fmt, ...)
#endif



#if 0
uint8_t challenge[MAC_CHALLENGE_SIZE] = { //challenge default, 实际有效的是20字节
		0xF8, 0xFE, 0xEE, 0x02, 0x88, 0xAF, 0xEE, 0x02, \
		0x4B, 0x47, 0x47, 0x73, 0x6B, 0x67, 0x79, 0x35, \
		0x78, 0x47, 0x67, 0x34, 0x88, 0xAF, 0xEE, 0x02, \
		0x0E, 0x00, 0x00, 0x0E, 0x04, 0x9E, 0x00, 0x00};
#else
uint8_t challenge[MAC_CHALLENGE_SIZE] = { \
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, \
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, \
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, \
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF \
	};
#endif
uint8_t (*key)[32] = key_dat;
uint8_t response[32] = {0};


#if 0
static void sha204c_show_data(uint8_t *header, uint8_t *buffer, uint8_t len)
{
	uint8_t i = 0;

	if ((header == NULL) || (buffer == NULL) || (len == 0))
		return;
	
	sha204_debug_fun("%s(0X):", header);
	for (i = 0; i < len; ++i) {
		sha204_debug_fun("%02X ", buffer[i]);
	}
	rt_kprintf("\n");
}
#endif

static int sha204a_mac_random(uint8_t *tx_buffer, uint8_t *rx_buffer)
{
	uint8_t mode = 0;

	uint16_t key_id = 0x0C;

	uint8_t *mac_res_data = NULL;
	struct sha204h_mac_in_out mac_in_out_param = {0};

#if 1
	//使用加密芯片产生随机数
	mode = RANDOM_SEED_UPDATE;
	if (sha204m_random(tx_buffer, rx_buffer, mode) != 0) {
		sha204_debug_fun("sha204m_random error");
	} else {
		sha204_debug_fun("sha204m_random ok\n");

		//判断产生随机数是否成功,成功则更新challenge供MAC使用
		if (rx_buffer[SHA204_BUFFER_POS_COUNT] == RANDOM_RSP_SIZE) {
			memcpy(challenge, &rx_buffer[SHA204_BUFFER_POS_DATA], MAC_CHALLENGE_SIZE);
			sha204_debug_fun("challenge updat\n");
		} else {
			sha204_debug_fun("challenge not update\n");
		}
	}
#endif
	//mac command
	mode = 0;
	if (ATSHA204m_mac(tx_buffer, rx_buffer, mode, key_id, challenge) != 0) {
		sha204_debug_fun("sha204m_mac error");
		
		return -1;
	} else {
		//判断产生随机数是否成功,成功则更新challenge供MAC使用
		if (rx_buffer[SHA204_BUFFER_POS_COUNT] != MAC_RSP_SIZE) {
			sha204_debug_fun("sha204m_mac: response size error");

			return -1;
		}
	}

	//local generates an SHA-256 digest
	mac_in_out_param.mode = mode;
	mac_in_out_param.key_id = key_id;
	mac_in_out_param.challenge = challenge;
	mac_in_out_param.key = key[key_id];
	mac_in_out_param.otp = NULL;
	mac_in_out_param.sn = NULL;
	mac_in_out_param.response = response;
	mac_in_out_param.temp_key = NULL;

	if (sha204h_mac(&mac_in_out_param) < 0) {
		sha204_debug_fun("sha204m_mac error");

		return -1;
	}

	//比较MAC返回值和本地计算的值是否一致
	mac_res_data = &rx_buffer[SHA204_BUFFER_POS_DATA];

	//sha204c_show_data("RX", mac_res_data, 32);
	//sha204c_show_data("CC", response, 32);

	if (memcmp(mac_res_data, response, 32) == 0) {
		sha204_debug_fun("Crypto Authentication Ok !!!\n");

		return 0;
	} else {
		sha204_debug_fun("Crypto Authentication Failed !!!\n");
		
		return -1;
	}

	 
}

#define READ_WRITE_ADDR(b, offset) ((b << 3) | (offset & 0x07))
		       
int sha204_authentication( void )
{
	uint8_t tx_buffer[SHA204_CMD_SIZE_MAX] = {0};
	uint8_t rx_buffer[SHA204_RSP_SIZE_MAX] = {0};

	SI2C_Init();

#if 0
	SHA204Ac_wakeup();
	//	 SHA204_ZONE_CONFIG   	 SHA204_ZONE_DATA		 READ_WRITE_ADDR(0x0F, 0x00)
	if (ATSHA204m_read(tx_buffer, rx_buffer, SHA204_ZONE_CONFIG, 0x15) != 0)
	{
		error("ATSHA204m_read error");
		goto err;
	}

	sha204c_show_data("Read", rx_buffer, rx_buffer[0]);

	while(1)
		;
#endif

	while (1)
	{
		if (SHA204Ac_wakeup() != 0) {
			sha204_debug_fun("sha204c_wakeup error");
	
			goto err;
		}
		sha204_debug_fun("sha204c_wakeup ok\n");
	
		if (sha204a_mac_random(tx_buffer, rx_buffer) < 0) {
			sha204_debug_fun("sha204a_mac_random error\n");
	
			goto err;
		}

		//end
		if (SHA204Ac_sleep() != 0) {
			sha204_debug_fun("sha204c_sleep error\n");
		}
	 
		return 0;

err:
		if (SHA204Ac_sleep() != 0) {
			sha204_debug_fun("sha204c_sleep error\n");
		}

		Delay_ms(500);

		while (1)
		{
			Delay_ms(500);
		}
	}

	 
}

