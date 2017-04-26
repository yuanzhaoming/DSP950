/**
  ******************************************************************************
  * @file    sha204a_comm.h
  * @author  wanghb
  * @version V0.0.0
  * @date    2016-08-31
  * @brief   ATSHA204A与主机通信的函数接口实现,以ATSHA204Ac为头.
  */

#ifndef __SHA204A_COMM_H
#define __SHA204A_COMM_H

#include <stdint.h>

//! minimum number of bytes in command (from count byte to second CRC byte)
#define SHA204_CMD_SIZE_MIN          ((uint8_t)  7)
//! maximum size of command packet (CheckMac)
#define SHA204_CMD_SIZE_MAX          ((uint8_t) 84)
//! number of CRC bytes
#define SHA204_CRC_SIZE              ((uint8_t)  2)

#define SHA204_RSP_SIZE_MIN          ((uint8_t)  4)  //!< minimum number of bytes in response
#define SHA204_RSP_SIZE_MAX          ((uint8_t) 35)  //!< maximum size of response packet

#define SHA204_BUFFER_POS_COUNT      (0)	//!< buffer index of count byte in command or response
#define SHA204_BUFFER_POS_DATA       (1)	//!< buffer index of data in response
#define SHA204_BUFFER_POS_STATUS     (1)	//! buffer index of status byte in status response

//! status byte after wake-up
#define SHA204_STATUS_BYTE_WAKEUP    ((uint8_t) 0x11)
//! command parse error
#define SHA204_STATUS_BYTE_PARSE     ((uint8_t) 0x03)
//! command execution error
#define SHA204_STATUS_BYTE_EXEC      ((uint8_t) 0x0F)
//! communication error
#define SHA204_STATUS_BYTE_COMM      ((uint8_t) 0xFF)

enum i2c_word_address {
	SHA204_I2C_PACKET_FUNCTION_RESET,  //!< Reset device.
	SHA204_I2C_PACKET_FUNCTION_SLEEP,  //!< Put device into Sleep mode.
	SHA204_I2C_PACKET_FUNCTION_IDLE,   //!< Put device into Idle mode.
	SHA204_I2C_PACKET_FUNCTION_NORMAL  //!< Write / evaluate data that follow this word address byte.
};

/*
 *SHA204 Library Return Code Definitions
 */
#define SHA204_SUCCESS              ((uint8_t)  0x00) //!< Function succeeded.
#define SHA204_PARSE_ERROR          ((uint8_t)  0xD2) //!< response status byte indicates parsing error
#define SHA204_CMD_FAIL             ((uint8_t)  0xD3) //!< response status byte indicates command execution error
#define SHA204_STATUS_CRC           ((uint8_t)  0xD4) //!< response status byte indicates CRC error
#define SHA204_STATUS_UNKNOWN       ((uint8_t)  0xD5) //!< response status byte is unknown
#define SHA204_FUNC_FAIL            ((uint8_t)  0xE0) //!< Function could not execute due to incorrect condition / state.
#define SHA204_GEN_FAIL             ((uint8_t)  0xE1) //!< unspecified error
#define SHA204_BAD_PARAM            ((uint8_t)  0xE2) //!< bad argument (out of range, null pointer, etc.)
#define SHA204_INVALID_ID           ((uint8_t)  0xE3) //!< invalid device id, id not set
#define SHA204_INVALID_SIZE         ((uint8_t)  0xE4) //!< Count value is out of range or greater than buffer size.
#define SHA204_BAD_CRC              ((uint8_t)  0xE5) //!< incorrect CRC received
#define SHA204_RX_FAIL              ((uint8_t)  0xE6) //!< Timed out while waiting for response. Number of bytes received is > 0.
#define SHA204_RX_NO_RESPONSE       ((uint8_t)  0xE7) //!< Not an error while the Command layer is polling for a command response.
#define SHA204_RESYNC_WITH_WAKEUP   ((uint8_t)  0xE8) //!< re-synchronization succeeded, but only after generating a Wake-up
#define SHA204_COMM_FAIL            ((uint8_t)  0xF0) //!< Communication with device failed. Same as in hardware dependent modules.
#define SHA204_TIMEOUT              ((uint8_t)  0xF1) //!< Timed out while waiting for response. Number of bytes received is 0.



void SHA204Ac_calculate_crc(uint8_t length, uint8_t *data, uint8_t *crc);
uint8_t SHA204Ac_check_crc(uint8_t *response);

uint8_t SHA204Ac_wakeup(void);
uint8_t SHA204Ac_idle(void);
uint8_t SHA204Ac_sleep(void);
uint8_t SHA204Ac_reset(void);

//向SHA204A发送指令接口
uint8_t SHA204Ac_send_cmd(uint8_t *CmdBuffer, uint8_t CmdLength);

//从SHA204A读取指令执行结果接口
uint8_t SHA204Ac_recv_response(uint8_t *ResponseBuffer, uint8_t ResponseLength);

//向SHA204A发送指令并且读取指令执行结果接口
uint8_t SHA204Ac_send_and_recv(uint8_t *TxBuffer, uint8_t *RxBuffer, uint8_t RxLength, uint32_t ExecutionDelay_us);


#endif /* __SHA204A_COMM_H */
