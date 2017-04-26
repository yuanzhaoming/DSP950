/******************************************************************************
Copyright (c) 2009 Analog Devices, Inc. All Rights Reserved. This software
is proprietary and confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************

$Revision: 14025 $
$Date: 2010-07-23 11:01:02 +1000 (Fri, 23 Jul 2010) $

Project:    WAV PCM Utilities
Title:      WAV Utilities
Author:     SK

Description:
            This file contains all public typedefs and function
            prototypes

References:
            [1] "Multimedia Programming Interface and Data
                Specifications", 1.0, IBM Corporation and Microsoft
                Corporation, August 1991

            [2] "Standards Update: New Multimedia Data Types and Data
                Techniques", 3.0, Microsoft Corporation, April 15 1994

            [3] "Multiple Channel Audio Data and WAVE Files",
                 http://www.microsoft.com/whdc/device/audio/multichaud.mspx,
                 March 7 2007

******************************************************************************/
#ifndef ADI_WAV_H
#define ADI_WAV_H

/******************************************************************************
**          #defines
******************************************************************************/

/* speaker bitmap */
#define SPEAKER_FRONT_LEFT             0x00000001
#define SPEAKER_FRONT_RIGHT            0x00000002
#define SPEAKER_FRONT_CENTER           0x00000004
#define SPEAKER_LOW_FREQUENCY          0x00000008
#define SPEAKER_BACK_LEFT              0x00000010
#define SPEAKER_BACK_RIGHT             0x00000020
#define SPEAKER_FRONT_LEFT_OF_CENTER   0x00000040
#define SPEAKER_FRONT_RIGHT_OF_CENTER  0x00000080
#define SPEAKER_BACK_CENTER            0x00000100
#define SPEAKER_SIDE_LEFT              0x00000200
#define SPEAKER_SIDE_RIGHT             0x00000400
#define SPEAKER_TOP_CENTER             0x00000800
#define SPEAKER_TOP_FRONT_LEFT         0x00001000
#define SPEAKER_TOP_FRONT_CENTER       0x00002000
#define SPEAKER_TOP_FRONT_RIGHT        0x00004000
#define SPEAKER_TOP_BACK_LEFT          0x00008000
#define SPEAKER_TOP_BACK_CENTER        0x00010000
#define SPEAKER_TOP_BACK_RIGHT         0x00020000
#define SPEAKER_RESERVED               0x80000000


/******************************************************************************
**          Private Typedefs/Enumerations
******************************************************************************/

typedef enum
{
    NONE = 0,
    READ_RIFF_CHUNK_ID,
    READ_RIFF_CHUNK_SIZE,
    READ_RIFF_FORMAT,
    READ_CHUNK_ID,
    READ_UNKNOWN_CHUNK_SIZE,
    READ_FORMAT_CHUNK_SIZE,
    READ_FORMAT_TAG,
    READ_NUM_CHANNELS,
    READ_SAMPLES_RATE,
    READ_AVERAGE_BYTES_PER_SEC,
    READ_BLOCK_ALIGN,
    READ_BITS_PER_SAMPLE,
    READ_EXTRA_SIZE_BYTES,
    READ_VALID_BITS_PER_SAMPLE,
    READ_CHANNEL_MASK,
    READ_SUBFORMAT_GUID_PART_1,
    READ_SUBFORMAT_GUID_PART_2,
    READ_SUBFORMAT_GUID_PART_3,
    READ_SUBFORMAT_GUID_PART_4,
    READ_DATA_CHUNK_SIZE,
    COMPLETED,
} adi_wav_state_t;

typedef struct
{
    int var;
    unsigned char byte_index;
} adi_wav_state_var_t;

typedef struct
{
    adi_wav_state_t wav_state;
    adi_wav_state_var_t state_var;
    int stream_offset;
    unsigned int format_ex_size;
} adi_wav_private_t;


/******************************************************************************
**          Public Typedefs/Enumerations
******************************************************************************/

typedef enum
{
    WAVE_FORMAT_UNSUPPORTED = 0,
    WAVE_FORMAT_PCM = 0x0001,
    WAVE_FORMAT_IEEE_FLOAT = 0x0003,
    WAVE_FORMAT_EXTENSIBLE = 0xFFFE
} adi_wav_format_t;

typedef enum
{
    FORMAT_SUBTYPE_UNSUPPORTED = 0,
    FORMAT_SUBTYPE_PCM = 0x00000001,
    FORMAT_SUBTYPE_IEEE_FLOAT = 0x00000003
} adi_wav_format_subtype_t;

typedef struct
{
    unsigned short sample_valid_num_bits;
    unsigned int channel_mask;
    adi_wav_format_subtype_t wav_format_subtype;
} adi_wav_extensible_t;

typedef struct
{
    /* public wave properties info */
    adi_wav_format_t wav_format;
    unsigned short num_channels;
    unsigned int sample_rate;
    unsigned int bytes_per_sec;
    unsigned short block_align;
    unsigned short sample_num_bits;
    unsigned int num_samples_per_channel;
    int data_unit_stream_offset;
    adi_wav_extensible_t extensible;

    /* circular buffering config */
    unsigned int use_circular_bitstream_buffer;
    int circular_buffer_length;
    unsigned int *circular_buffer_base;

    /* parsing status info */
    int wav_parsed;
    int format_chunk_parsed;
    int next_object_length_num_bytes;
    int num_input_bytes_consumed;
    unsigned int *unconsumed_input_data_ptr;
    int unconsumed_input_byte_offset;

    /* write file header status info */
    int num_output_bytes_produced;

    /* private state info */
    adi_wav_private_t wav_private;
} adi_wav_t;

typedef enum
{
    ADI_WAV_SUCCESS = 0,
    ADI_WAV_OUTPUT_BUFFER_TOO_SMALL = 0x0011,
    ADI_WAV_INVALID_HEADER = 0x1100,
    ADI_WAV_UNKNOWN_OBJECT = 0x1105,
} adi_wav_return_code_t;

/******************************************************************************
**      Function Declarations
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

    /*
    **
    ** Function:            adi_wav_parse_init
    **
    ** Description:         A function to initialize a WAV instance.
    **
    ** Arguments:
    **
    **  adi_wav_t *wav
    **                      a pointer to the WAV instance.  Instance
    **                      initialized on return.
    **
    **
    ** Outputs:             See parameter "wav"
    **
    ** Return value:
    **                      One of the following codes is returned:
    **                          ADI_WAV_SUCCESS
    **
    **
    */
    adi_wav_return_code_t adi_wav_parse_init(adi_wav_t *wav);


    /*
    **
    ** Function:            adi_wav_parser
    **
    ** Description:         This function is the main parsing routine
    **
    ** Arguments:
    **
    **  adi_wav_t *wav
    **                      a pointer to the WAV instance.  Instance
    **                      updated on return.
    **
    **  int num_input_bytes
    **                      Number of bytes in the input buffer
    **                      "input_bitstream".  Depending on the state of
    **                      the parser, it may consume nil, some or all of
    **                      the bytes from the buffer.  The number of
    **                      bytes consumed as well as the pointer to the
    **                      next unconsumed bitstream element is returned
    **                      by the parser in the status fields of the
    **                      instance structure.
    **
    **  unsigned int *input_bitstream
    **
    **                      Input buffer containing the WAV input
    **                      bitstream.  This buffer must contain at least
    **                      "num_input_bytes".
    **
    **  int input_byte_offset
    **
    **                      Tells the parser the byte position within the
    **                      memory word where the bitstream data starts
    **                      within the memory location pointed to by
    **                      "input_bitstream".
    **
    **
    ** Outputs:             See parameter "wav"
    **
    ** Return value:
    **                      One of the following codes is returned:
    **                          ADI_WAV_SUCCESS
    **                          ADI_WAV_INVALID_HEADER
    **                          ADI_WAV_UNKNOWN_OBJECT
    **
    **
    */
    adi_wav_return_code_t adi_wav_parser(adi_wav_t *wav, int num_input_bytes,
                                         unsigned int *input_bitstream,
                                         int input_byte_offset);


    /*
    **
    ** Function:            adi_wav_write_fileheader
    **
    ** Description:         This function is to be used to write a WAV
    **                      header to an output buffer.
    **
    ** Arguments:
    **
    **  adi_wav_t *wav
    **                      a pointer to the WAV instance.
    **
    **  int output_buffer_length_bytes
    **
    **                      Specifies the maximum length of the output
    **                      buffer, "output_bitstream".
    **
    **  unsigned int *output_bitstream
    **
    **                      Output buffer where WAV header will be
    **                      written.  The size of the header will be
    **                      returned in the instance structure variable,
    **                      "num_output_bytes_produced"
    **
    **
    ** Outputs:             "output_bitstream" will contain a WAV header
    **
    ** Return value:
    **                      One of the following codes is returned:
    **                          ADI_WAV_SUCCESS
    **                          ADI_WAV_OUTPUT_BUFFER_TOO_SMALL
    **
    **
    */
    adi_wav_return_code_t adi_wav_write_fileheader(adi_wav_t *wav,
            int output_buffer_length_bytes,
            unsigned int *output_bitstream);

#if defined(__cplusplus)
}
#endif

#endif /* ADI_WAV_H */

/*
**
** EOF: $HeadURL: http://adasvn.spd.analog.com/audio/tags/common/components/pcm-utils/1.1.0/src/adi_wav.h $
**
*/
