/******************************************************************************
Copyright (c) 2006-2009 Analog Devices, Inc.  All Rights Reserved.
This software is proprietary and confidential to Analog Devices,
Inc. and its licensors.
*******************************************************************************

$Revision: 17585 $
$Date: 2011-11-08 11:49:32 +1100 (Tue, 08 Nov 2011) $

Project:    AC3 Decoder
Title:      File Based Demonstration
Author:     LM

Description:
            Provides declarations of functions and data types used in
            the file-IO-based application example for AC3 decoder.

References:
            [1] "Dolby Digital (AC-3) 5.1 Decoder Developer's Guide",
                KT-25, rev. 0.8

******************************************************************************/
#include <stdio.h>
#include <cycle_count.h>
#include "adi_ac3_decoder.h"
#include "adi_wav.h"
#include "pcm_conversion.h"
#include "ADDS_21489_EzKit.h"

#ifndef FILE_IO_H
#define FILE_IO_H

// Size of the input buffer, set to the maximum possible frame size in words,
#define BITSTREAM_BUFFER_NUMWORDS \
    ((ADI_AC3D_MIN_BITSTREAM_BUFFER_NUMBYTES + 3) / 4)

#define NUM_AUDIO_BLOCKS_PER_FRAME 6
#define MAX_NUM_PCM_SAMPLES_PER_BLOCK \
    (ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN*ADI_AC3D_MAX_NUM_OUTPUT_CHANS)

/******************************************************************************
**          Typedefs/Enumerations
******************************************************************************/

enum { WARNING, FATAL, MESSAGE };
enum {SYNC_WORD = 0x0b77};

typedef enum
{
    DISABLE_CIRCULAR_BUFFER = 0,
    ENABLE_CIRCULAR_BUFFER  = 1,
} circular_buffer_flag_t;

typedef enum
{
    DISABLE_UNPACKED_BUFFER = 0,
    ENABLE_UNPACKED_BUFFER  = 1,
} unpacked_buffer_flag_t;

typedef struct
{
    // FILE *ac3file;
    circular_buffer_flag_t use_circular_buffer;
    unpacked_buffer_flag_t use_unpacked_buffer;
    unsigned int buffer_length;
    adi_ac3d_bitstream_data_t *buffer_base;
    adi_ac3d_bitstream_data_t *bitstream_ptr;
    adi_ac3d_bitstream_data_t *unconsumed_bitstream_ptr;
    int input_byte_offset;
    int num_bytes_in_buffer;
} bitstream_info_t;

typedef struct
{
    FILE *wavfile;
    adi_ac3d_audio_data_t *channel_ptrs[ADI_AC3D_MAX_NUM_OUTPUT_CHANS];
} wav_info_t;

typedef enum
{
    LEFT        = 0,
    RIGHT       = 1,
    LEFT_SURR   = 2,
    RIGHT_SURR  = 3,
    CENTER      = 4,
    LFE         = 5,
} chan_idx_t;

typedef enum
{
    MS_LEFT,
    MS_RIGHT,
    MS_CENTER,
    MS_LFE,
    MS_LEFT_SURR,
    MS_RIGHT_SURR,
} ms_chan_idx_t;

typedef enum
{
    KARAOKE_MODE_NO_VOCAL = 0,
    KARAOKE_MODE_LEFT_VOCAL = 1,
    KARAOKE_MODE_RIGHT_VOCAL = 2,
    KARAOKE_MODE_BOTH_VOCALS = 3,
} karaoke_mode_t;

typedef enum
{
    COMPRESSION_MODE_CUSTOM_ANALOG = 0,
    COMPRESSION_MODE_CUSTOM_DIGITAL = 1,
    COMPRESSION_MODE_LINE_OUT = 2,
    COMPRESSION_MODE_RF_REMOD = 3,
} compression_mode_t;

typedef enum
{
    LFE_OFF = 0,
    LFE_ON = 1,
} lfe_mode_t;

typedef enum
{
    OUTPUT_MODE_RESERVED = 0,
    OUTPUT_MODE_1_0 = 1,
    OUTPUT_MODE_2_0 = 2,
    OUTPUT_MODE_3_0 = 3,
    OUTPUT_MODE_2_1 = 4,
    OUTPUT_MODE_3_1 = 5,
    OUTPUT_MODE_2_2 = 6,
    OUTPUT_MODE_3_2 = 7,
} output_mode_t;

typedef enum
{
    STEREO_MODE_AUTO_DETECT = 0,
    STEREO_MODE_DOLBY_SURROUND = 1,
    STEREO_MODE_STERO = 2,
} stereo_mode_t;

typedef enum
{
    DUAL_MONO_STEREO = 0,
    DUAL_MONO_LEFT_MONO = 1,
    DUAL_MONO_RIGHT_MONO = 2,
    DUAL_MONO_MIXED_MONO = 3,
} dual_mono_t;



/*用于控制处理dma接受到的SPDIF ac3码流的结构*/
typedef struct
{
    char *pblockbuf;
    int blocklength;
    int nextstartoffset;
} ac3block_t;

extern int ac3framecount;
extern int SPDIFstream_state_cur;
//Pointer to the blocks
//码流信息结构体指针
extern bitstream_info_t bitstream_info;
extern ac3block_t ac3block_m;
extern int DecodedPCMTxBlock_readpointer;
extern int DecodedPCMTxBlock_writepointer;
extern int TxBlock_SDRAM0[DECODEDPCMDATABUFFNUM][NUM_SAMPLES *NUM_TX_SLOTS];

extern int *p_adi_sc3decoder_crackbreak;
extern int firestflag;
extern int TxBlock_readpointer_tmp;
/******************************************************************************
**          Function Declarations
******************************************************************************/
void print_wav_header_info(adi_wav_t *wav);

static void fill_config_params(bitstream_info_t *bitstream_info,
                               adi_ac3d_frame_properties_t *frame_properties,
                               adi_ac3d_config_t *config_params);

int fill_bitstream_buffer(unsigned int num_words,
                          bitstream_info_t *bitstream_info);

int sync_cur_frame(unsigned int *, bitstream_info_t *);
int sync_next_frame(unsigned int *end_of_file,
                    bitstream_info_t *bitstream_info);

static void print_error(unsigned int frame_num,
                        unsigned int block_num,
                        adi_ac3d_return_code_t prev_status,
                        adi_ac3d_return_code_t status,
                        FILE *logfile);

static void log_cycle_counts(unsigned int frame_num,
                             cycle_t frame_cycle_cnt,
                             adi_ac3d_frame_properties_t *frame_properties,
                             FILE *cycle_cnt_file);



int setac3block(ac3block_t *, void *, int);
int readfromac3block(adi_ac3d_bitstream_data_t *, int, int, ac3block_t *);
int ifac3blockend(ac3block_t *);
int readfromac3block_16(adi_ac3d_bitstream_data_t *, int, int, ac3block_t *);
int pcm_sdramwriteblock_float_to_fixed(float **, int*, int, int, int);
int zeroTxBlock_SDRAM0(void);
#endif


/*
**
** EOF: $HeadURL: http://adasvn.spd.analog.com/audio/trunk/ac3-decoder/sharc/example/Source/file_io.h $
**
*/
