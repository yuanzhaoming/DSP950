#include "sha204_authentication.h"
#include "sha204a_comm_marshaling.h"    // definitions and declarations for the Command module
#include "sha256_atmel.h"
#include <string.h>                    // needed for memcpy()



/** \brief This is a wrapper function for SHA256 algorithm.
 *
 *         User can modify this function depending on their SHA256 implementation.
 *
 * \param [in,out] param Structure for input/output parameters. Refer to sha204h_calculate_sha256_in_out.
 */
static void sha204h_calculate_sha256(struct sha204h_calculate_sha256_in_out *param)
{
	// This is the "free/open-source" implementation
	// sha256(param->message, param->length, param->digest);
	
	// This is Atmel's implementation
	create_sha256(param->length, param->message, param->digest);
}


/** \brief This function generates an SHA-256 digest (MAC) of a key, challenge, and other informations.
 *
 *         The resulting digest will match with those generated in the Device by MAC opcode.
 *         The TempKey (if used) should be valid (temp_key.valid = 1) before executing this function.
 *
 * \param [in,out] param Structure for input/output parameters. Refer to sha204h_mac_in_out.
 * \return status of the operation.
 */
int sha204h_mac(struct sha204h_mac_in_out *param)
{
	// Local Variables
	struct sha204h_calculate_sha256_in_out calculate_sha256_param;
	uint8_t temporary[SHA204_MSG_SIZE_MAC];
	uint8_t i;
	uint8_t *p_temp;

	// Check parameters
	if (	!param->response
			|| ((param->mode & ~MAC_MODE_MASK) != 0)
			|| (((param->mode & MAC_MODE_BLOCK1_TEMPKEY) == 0) && !param->key)
			|| (((param->mode & MAC_MODE_BLOCK2_TEMPKEY) == 0) && !param->challenge)
			|| (((param->mode & MAC_MODE_USE_TEMPKEY_MASK) != 0) && !param->temp_key)
			|| (((param->mode & MAC_MODE_INCLUDE_OTP_64) != 0) && !param->otp)
			|| (((param->mode & MAC_MODE_INCLUDE_OTP_88) != 0) && !param->otp)
			|| (((param->mode & MAC_MODE_INCLUDE_SN) != 0) && !param->sn) )
		return -1;

	// Check TempKey fields validity if TempKey is used
	if (	((param->mode & MAC_MODE_USE_TEMPKEY_MASK) != 0) &&
			// TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
			(  (param->temp_key->check_flag != 0)
			|| (param->temp_key->valid != 1)
			// If either mode parameter bit 0 or bit 1 are set, mode parameter bit 2 must match temp_key.source_flag
			// Logical not (!) are used to evaluate the expression to TRUE/FALSE first before comparison (!=)
			|| (!(param->mode & MAC_MODE_SOURCE_FLAG_MATCH) != !(param->temp_key->source_flag)) ))
	{
		// Invalidate TempKey, then return
		param->temp_key->valid = 0;
		return -2;
	}

	// Start calculation
	p_temp = temporary;

	// (1) first 32 bytes
	if (param->mode & MAC_MODE_BLOCK1_TEMPKEY) {
		memcpy(p_temp, param->temp_key->value, 32);    // use TempKey.Value
		p_temp += 32;
	} else {
		memcpy(p_temp, param->key, 32);                // use Key[KeyID]
		p_temp += 32;
	}

	// (2) second 32 bytes
	if (param->mode & MAC_MODE_BLOCK2_TEMPKEY) {
		memcpy(p_temp, param->temp_key->value, 32);    // use TempKey.Value
		p_temp += 32;
	} else {
		memcpy(p_temp, param->challenge, 32);          // use challenge
		p_temp += 32;
	}

	// (3) 1 byte opcode
	*p_temp++ = SHA204_MAC;

	// (4) 1 byte mode parameter
	*p_temp++ = param->mode;

	// (5) 2 bytes keyID
	*p_temp++ = param->key_id & 0xFF;
	*p_temp++ = (param->key_id >> 8) & 0xFF;

	// (6, 7) 8 bytes OTP[0:7] or 0x00's, 3 bytes OTP[8:10] or 0x00's
	if (param->mode & MAC_MODE_INCLUDE_OTP_88) {
		memcpy(p_temp, param->otp, 11);            // use OTP[0:10], Mode:5 is overridden
		p_temp += 11;
	} else {
		if (param->mode & MAC_MODE_INCLUDE_OTP_64) {
			memcpy(p_temp, param->otp, 8);         // use 8 bytes OTP[0:7] for (6)
			p_temp += 8;
		} else {
			for (i = 0; i < 8; i++) {             // use 8 bytes 0x00's for (6)
				*p_temp++ = 0x00;
			}
		}

		for (i = 0; i < 3; i++) {                 // use 3 bytes 0x00's for (7)
			*p_temp++ = 0x00;
		}
	}

	// (8) 1 byte SN[8] = 0xEE
	*p_temp++ = SHA204_SN_8;

	// (9) 4 bytes SN[4:7] or 0x00's
	if (param->mode & MAC_MODE_INCLUDE_SN) {
		memcpy(p_temp, &param->sn[4], 4);     //use SN[4:7] for (9)
		p_temp += 4;
	} else {
		for (i = 0; i < 4; i++) {            //use 0x00's for (9)
			*p_temp++ = 0x00;
		}
	}

	// (10) 2 bytes SN[0:1] = 0x0123
	*p_temp++ = SHA204_SN_0;
	*p_temp++ = SHA204_SN_1;

	// (11) 2 bytes SN[2:3] or 0x00's
	if (param->mode & MAC_MODE_INCLUDE_SN) {
		memcpy(p_temp, &param->sn[2], 2);     //use SN[2:3] for (11)
		p_temp += 2;
	} else {
		for (i = 0; i < 2; i++) {            //use 0x00's for (11)
			*p_temp++ = 0x00;
		}
	}

	// Calculate SHA256 to get the MAC digest
	calculate_sha256_param.length  = SHA204_MSG_SIZE_MAC;
	calculate_sha256_param.message = temporary;
	calculate_sha256_param.digest  = param->response;
	sha204h_calculate_sha256(&calculate_sha256_param);

	// Update TempKey fields
	if (param->temp_key) {
		param->temp_key->valid = 0;
	}

	return 0;
}

