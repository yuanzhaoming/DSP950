#include "sha204a_comm_marshaling.h"
#include "sha204a_comm.h"
#include <stdio.h>
#include <string.h>

uint8_t ATSHA204m_dev_rev(uint8_t *TxBuffer, uint8_t *RxBuffer)
{
	if (!TxBuffer || !RxBuffer)
		return SHA204_BAD_PARAM;

	TxBuffer[SHA204_COUNT_IDX] = DEVREV_COUNT;
	TxBuffer[SHA204_OPCODE_IDX] = SHA204_DEVREV;

	// Parameters are 0.
	TxBuffer[DEVREV_PARAM1_IDX] =
	TxBuffer[DEVREV_PARAM2_IDX] =
	TxBuffer[DEVREV_PARAM2_IDX + 1] = 0;
	
	rt_kprintf("DEVREV_DELAY=%d\r\n", DEVREV_DELAY);
	
	return SHA204Ac_send_and_recv(TxBuffer, RxBuffer, DEVREV_RSP_SIZE, DEVREV_DELAY);
}

uint8_t ATSHA204m_read(uint8_t *TxBuffer, uint8_t *RxBuffer, uint8_t zone, uint16_t address)
{
	uint8_t rx_size;

	if (!TxBuffer || !RxBuffer || ((zone & ~READ_ZONE_MASK) != 0)
				|| ((zone & READ_ZONE_MODE_32_BYTES) && (zone == SHA204_ZONE_OTP)))
		return SHA204_BAD_PARAM;

	TxBuffer[SHA204_COUNT_IDX] = READ_COUNT;
	TxBuffer[SHA204_OPCODE_IDX] = SHA204_READ;
	TxBuffer[READ_ZONE_IDX] = zone;
	TxBuffer[READ_ADDR_IDX] = (uint8_t) (address & SHA204_ADDRESS_MASK);
	TxBuffer[READ_ADDR_IDX + 1] = 0;

	rx_size = (zone & SHA204_ZONE_COUNT_FLAG) ? READ_32_RSP_SIZE : READ_4_RSP_SIZE;
	rt_kprintf("READ_DELAY=%d\r\n", READ_DELAY);

	return SHA204Ac_send_and_recv(TxBuffer, RxBuffer, rx_size, READ_DELAY);
}

uint8_t ATSHA204m_write(uint8_t *TxBuffer, uint8_t *RxBuffer,
			uint8_t zone, uint16_t address, uint8_t *new_value, uint8_t *mac)
{
	uint8_t *p_command;
	uint8_t count;

	if (!TxBuffer || !RxBuffer || !new_value || ((zone & ~WRITE_ZONE_MASK) != 0))
		return SHA204_BAD_PARAM;

	if (zone & SHA204_ZONE_DATA) {
		address >>= 2;
		if (address & 1)
			// If we would just mask this bit, we would
			// write to an address that was not intended.
			return SHA204_BAD_PARAM;
	}

	p_command = &TxBuffer[SHA204_OPCODE_IDX];
	*p_command++ = SHA204_WRITE;
	*p_command++ = zone;
	*p_command++ = (uint8_t) (address & SHA204_ADDRESS_MASK);
	*p_command++ = 0;

	count = (zone & SHA204_ZONE_COUNT_FLAG) ? SHA204_ZONE_ACCESS_32 : SHA204_ZONE_ACCESS_4;
	memcpy(p_command, new_value, count);
	p_command += count;

	if (mac != NULL)
	{
		memcpy(p_command, mac, WRITE_MAC_SIZE);
		p_command += WRITE_MAC_SIZE;
	}

	// Supply count.
	TxBuffer[SHA204_COUNT_IDX] = (uint8_t) (p_command - &TxBuffer[0] + SHA204_CRC_SIZE);

	return SHA204Ac_send_and_recv(&TxBuffer[0], &RxBuffer[0], WRITE_RSP_SIZE, WRITE_DELAY);
}

uint8_t ATSHA204m_mac(uint8_t *tx_buffer, uint8_t *rx_buffer,
			uint8_t mode, uint16_t key_id, uint8_t *challenge)
{
	if (!tx_buffer || !rx_buffer || ((mode & ~MAC_MODE_MASK) != 0)
				|| (((mode & MAC_MODE_BLOCK2_TEMPKEY) == 0) && !challenge))
		return SHA204_BAD_PARAM;

	tx_buffer[SHA204_COUNT_IDX] = MAC_COUNT_SHORT;
	tx_buffer[SHA204_OPCODE_IDX] = SHA204_MAC;
	tx_buffer[MAC_MODE_IDX] = mode;
	tx_buffer[MAC_KEYID_IDX] = key_id & 0xFF;
	tx_buffer[MAC_KEYID_IDX + 1] = key_id >> 8;
	if ((mode & MAC_MODE_BLOCK2_TEMPKEY) == 0)
	{
		memcpy(&tx_buffer[MAC_CHALLENGE_IDX], challenge, MAC_CHALLENGE_SIZE);
		tx_buffer[SHA204_COUNT_IDX] = MAC_COUNT_LONG;
	}

	return SHA204Ac_send_and_recv(&tx_buffer[0], &rx_buffer[0], MAC_RSP_SIZE, MAC_DELAY * 10);
}

uint8_t sha204m_random(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t mode)
{
	if (!tx_buffer || !rx_buffer || (mode > RANDOM_NO_SEED_UPDATE))
		return SHA204_BAD_PARAM;

	tx_buffer[SHA204_COUNT_IDX] = RANDOM_COUNT;
	tx_buffer[SHA204_OPCODE_IDX] = SHA204_RANDOM;
	tx_buffer[RANDOM_MODE_IDX] = mode;

	//parameter 2 is 0.
	tx_buffer[RANDOM_PARAM2_IDX] =
	tx_buffer[RANDOM_PARAM2_IDX + 1] = 0;

	return SHA204Ac_send_and_recv(tx_buffer, rx_buffer, RANDOM_RSP_SIZE, RANDOM_DELAY);
}

