/******************************************************************************
Copyright (c) 2006-2011 Analog Devices, Inc.  All Rights Reserved.
This software is proprietary and confidential to Analog Devices,
Inc. and its licensors.
*******************************************************************************

$Revision: 17683 $
$Date: 2011-12-02 14:10:18 +1100 (Fri, 02 Dec 2011) $

Project:    AC-3 Decoder
Title:      API definitions
Author:     HIF

Description:
            This file contains all API macro definitions, typedefs and
            function prototypes.

References:
            [1] Dolby Digital (AC-3) 5.1 Decoder Developers Guide,
                KT-25, rev. 0.8, Analog Devices, June 2007
            [2] Digital Audio Compression Standard (AC-3), Doc A/52,
                20 Dec 95.

******************************************************************************/

#ifndef ADI_AC3_DECODER_HEADER_H
#define ADI_AC3_DECODER_HEADER_H

/******************************************************************************
**          #defines
******************************************************************************/

// Maximum number of channels supported by the decoder.
#define ADI_AC3D_MAX_NUM_OUTPUT_CHANS 6

// Maximum number of PCM samples output by the decoder on a given call.
#define ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN 256

// Minimum length of the input bit-stream buffer.
#define ADI_AC3D_MIN_BITSTREAM_BUFFER_NUMBYTES (3840+256)

// Size of the frame header preceding each AC-3 frame.
#define ADI_AC3D_MIN_FRAME_HEADER_NUMBYTES 20

// Size of the memory block to be used for saving "state" information
// by each decoder instance.
#define ADI_AC3D_FAST_PRIO0_STATE_MEM_NUM_CHARS 27124/4

// Size of the memory block to be used for temporary storage by each
// decoder instance.
#define ADI_AC3D_FAST_PRIO1_SCRATCH_MEM_NUM_CHARS 2168/4

// Macro used to check header file consistency at link-time
#define adi_ac3d_create adi_ac3d_create_3_0

/******************************************************************************
**          Typedefs/Enumerations
******************************************************************************/

// Data type of the instance handle object.
typedef struct adi_ac3d_instance_t adi_ac3d_instance_t;

// Data type of the input bit-stream.
typedef int adi_ac3d_bitstream_data_t;

// Data type of the output reconstructed PCM samples.
typedef float adi_ac3d_audio_data_t;

// Enumerations for those configuration parameters that are allowed to
// be reconfigured on the fly.
typedef enum
{
    ADI_AC3D_DYN_RANGE_SCALE_LO  = 1,
    ADI_AC3D_DYN_RANGE_SCALE_HI  = 2,
    ADI_AC3D_PCM_SCALE_FACTOR    = 3,
    ADI_AC3D_COMPRESSION_MODE    = 4,
    ADI_AC3D_STEREO_DOWNMIX_MODE = 5,
    ADI_AC3D_DUAL_MONO_MODE      = 6,
    ADI_AC3D_KARAOKE_MODE        = 7,
    ADI_AC3D_LFE_ON              = 8,
    ADI_AC3D_OUTPUT_CHAN_CONFIG  = 9,
    ADI_AC3D_ALL_RECONFIG_PARAMS = 10,
} adi_ac3d_config_item_t;

// Enumerations for the output channel configuration.  Also used in
// the status information returned by the decoder to indicate the
// channel configuration present in the input bit-stream.  See [1] and
// [2].
typedef enum
{
    ADI_AC3D_0_0_CHAN_CONFIG = 0, // 1+1 (ch1, ch2)
    ADI_AC3D_1_0_CHAN_CONFIG = 1, // 1/0 (center)
    ADI_AC3D_2_0_CHAN_CONFIG = 2, // 2/0 (left, right)
    ADI_AC3D_3_0_CHAN_CONFIG = 3, // 3/0 (left, center, right)
    ADI_AC3D_2_1_CHAN_CONFIG = 4, // 2/1 (left, right, surround)
    ADI_AC3D_3_1_CHAN_CONFIG = 5, // 3/1 (left, center, right, surround)
    ADI_AC3D_2_2_CHAN_CONFIG = 6, // 2/2 (left, right, left surr., right surr.)
    ADI_AC3D_3_2_CHAN_CONFIG = 7, // 3/2 (left, center, right, left s., right s.)
} adi_ac3d_chan_config_t;

// Enumerations for the codes returned by all the API functions.
typedef enum
{
    ADI_AC3D_SUCCESS                        =  0,
    ADI_AC3D_TIMEOUT_REACHED                =  1,
    ADI_AC3D_CRC1_FAILED                    =  2,
    ADI_AC3D_CRC2_FAILED                    =  3,
    ADI_AC3D_INVALID_BITSTREAM_SIDE_INFO    =  4,
    ADI_AC3D_DECODING_AUDIO_BLOCK_FAILED    =  5,
    ADI_AC3D_FRAME_BOUNDARY_OVERRUN         =  6,
    ADI_AC3D_INVALID_CONFIG_PARAMS          =  11,
    ADI_AC3D_NEED_MORE_INPUT                =  100,
    ADI_AC3D_INVALID_SYNC                   = -102,
    ADI_AC3D_INVALID_HEADER                 = -101,
    ADI_AC3D_INVALID_BITSTREAM              = -100,
} adi_ac3d_return_code_t;


// Configuration parameters to control the rendering of the output
// audio.
typedef struct
{
    float dyn_range_scale_lo;
    float dyn_range_scale_hi;
    float pcm_scale_factor;
    int compression_mode;
    int stereo_downmix_mode;
    int dual_mono_mode;
    int karaoke_mode;
    int lfe_on;
    adi_ac3d_chan_config_t output_chan_config;
} adi_ac3d_audio_config_t;

// Frame properties extracted from the frame header preceding each AC-3
// frame. See [1] and [2].
typedef struct
{
    int sample_rate;
    int frame_size;
    int frame_size_five_eights;
    unsigned int bs_config;
    unsigned int bs_info;
    unsigned int dialogue_normalization;
    unsigned int language_code;
    unsigned int audio_production_info;
    unsigned int extended_bs_info_part_one;
    unsigned int extended_bs_info_part_two;
} adi_ac3d_frame_properties_t;

// Complete set of configuration parameters.
typedef struct
{
    adi_ac3d_audio_config_t audio_render_params;
    int num_prev_block_repeats;
    int use_circular_bitstream_buffer;
    int use_unpacked_input_buffer;
    int circular_buffer_length;
    adi_ac3d_bitstream_data_t *circular_buffer_base;
    adi_ac3d_frame_properties_t *frame_properties_ptr;
} adi_ac3d_config_t;

// Status information returned by the main decoder routine
// adi_ac3d_decoder().
typedef struct
{
    int num_output_samples_per_chan;
    int num_input_bytes_consumed;
    unsigned int warning_info;
    unsigned int frame_completed;
    unsigned int active_output_chans;
    adi_ac3d_bitstream_data_t *unconsumed_input_data_ptr;
    int unconsumed_input_byte_offset;
} adi_ac3d_output_status_t;


/* Per-instance "state" memory block pointers */
typedef struct
{
    struct
    {
        void *prio0;
    } fast;
} adi_ac3d_state_mem_t;

/* Per-instance "scratch" memory block pointers */
typedef struct
{
    struct
    {
        void *prio0;
    } fast;
} adi_ac3d_scratch_mem_t;

/* Complete set of per-instance memory block pointers */
typedef struct
{
    adi_ac3d_state_mem_t state;
    adi_ac3d_scratch_mem_t scratch;
} adi_ac3d_mem_t;


// Library version information accessible at run-time.
typedef struct
{
    unsigned int version_num;
    char *version_text;
    char *certification_id;
} adi_ac3d_module_info_t;

extern adi_ac3d_module_info_t adi_ac3d_module_info;

/******************************************************************************
**      Function Declarations
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

    /*
    **
    ** Function: adi_ac3d_create()
    **
    ** Description:
    **  This function is to be used to create a single instance of the
    **  decoder.
    **
    ** Arguments:
    **  adi_ac3d_mem_t *per_instance_mem_blocks (INPUT)
    **      Per instance state and scratch module memory block pointers
    **
    ** Outputs:
    **  None.
    **
    ** Return value:
    **  On successful creation of the instance, the handle to the instance
    **  is returned, otherwise a NULL pointer is returned.
    **
    */

    adi_ac3d_instance_t *adi_ac3d_create(
        adi_ac3d_mem_t *per_instance_mem_blocks);

    /*
    **
    ** Function: adi_ac3d_init()
    **
    ** Description:
    **  This function is to be used to initialize a created decoder
    **  instance.  This function checks validity of supplied configuration
    **  parameters.  Invalid parameters are overriden with defaults.  The
    **  actual configuration parameters used are returned in the
    **  input/output structure parameter "config_params".
    **
    ** Arguments:
    **  adi_ac3d_instance_t *instance_handle (INPUT/OUTPUT)
    **      Handler to the decoder instance.  Instance updated on return.
    **
    **  adi_ac3d_config_t *config_params (INPUT/OUTPUT)
    **      Structure containing the input configuration parameters.  On
    **      return, this structure will contain the actual configuration
    **      parameters used.
    **
    ** Outputs:
    **  See parameters "instance_handle" and "config_params" above.
    **
    ** Return value:
    **  One of the following codes is returned:
    **      ADI_AC3D_SUCCESS
    **      ADI_AC3D_INVALID_CONFIG_PARAMS
    **
    */

    adi_ac3d_return_code_t adi_ac3d_init(
        adi_ac3d_instance_t *instance_handle,
        adi_ac3d_config_t *config_params);

    /*
    **
    ** Function: adi_ac3d_reconfigure()
    **
    ** Description:
    **  This function is to be used to reconfigure one or more of the
    **  configuration parameters used by the decoder.
    **
    ** Arguments:
    **  adi_ac3d_instance_t *instance_handle (INPUT/OUTPUT)
    **      Handler to the decoder instance.  Instance updated on return.
    **
    **  adi_ac3d_config_item_t config_item (INPUT)
    **      Configuration parameter to be updated.  To reconfigure all
    **      parameters with one call, set to ADI_AC3D_ALL_RECONFIG_PARAMS.
    **
    **  adi_ac3d_config_t *config_params (INPUT/OUTPUT)
    **      Structure containing the input configuration parameters.  On
    **      return, this structure will contain the actual values of
    **      configuration parameters used.
    **
    ** Outputs:
    **  See parameters "instance_handle" and "config_params" above.
    **
    ** Return value:
    **  One of the following codes is returned:
    **      ADI_AC3D_SUCCESS
    **      ADI_AC3D_INVALID_CONFIG_PARAMS
    **
    */

    adi_ac3d_return_code_t adi_ac3d_reconfigure(
        adi_ac3d_instance_t *instance_handle,
        adi_ac3d_config_item_t config_item,
        adi_ac3d_config_t *config_params);

    /*
    **
    ** Function: adi_ac3d_resynch()
    **
    ** Description:
    **  This function is to be used to reset the decoder in the event of
    **  an error.  Decoding must re-start on a new frame.
    **
    ** Arguments:
    **  adi_ac3d_instance_t *instance_handle (INPUT/OUTPUT)
    **      Handler to the decoder instance.  Instance updated on return.
    **
    ** Outputs:
    **  See parameter "instance_handle" above.
    **
    ** Return value:
    **  None.
    **
    */

    void adi_ac3d_resynch(
        adi_ac3d_instance_t *instance_handle);

    /*
    **
    ** Function: adi_ac3d_decoder()
    **
    ** Description:
    **  This function is the main decoder routine.  The function is
    **  designed to be called for each audio block to be decoded.  One
    **  audio block represents
    **  ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN PCM samples.
    **
    **  In the event of errors, the decoder enters "error concealment"
    **  mode and still produces that many samples in each channel,
    **  consisting either of zeros or of windowed repetitions of the
    **  previous block.  For detailed information, see [1].
    **
    ** Arguments:
    **  adi_ac3d_instance_t *instance_handle (INPUT/OUTPUT)
    **      Handler to the decoder instance.  Instance updated on return.
    **
    **  int num_input_bytes (INPUT)
    **      Number of bytes in the input buffer ("input_bitstream").
    **      Depending on the state of the decoder, it may consume nil,
    **      some or all of this many bytes from the buffer.  The number of
    **      bytes consumed as well as the pointer to the next unconsumed
    **      bit-stream element is returned by the decoder in the status
    **      structure ("decoder_status"), except in the case of errors.
    **
    **      'num_input_bytes' must be a multiple of 2.
    **
    **  adi_ac3d_bitstream_data_t *input_bitstream (INPUT)
    **      Input buffer containing the AC-3 input stream.  This buffer
    **      must contain at least "num_input_bytes".
    **
    **  int input_byte_offset (INPUT)
    **      Byte position within the memory word where the bitstream data
    **      starts within the memory location pointed to by
    **      input_bitstream
    **
    ** Outputs:
    **  adi_ac3d_output_status_t *decoder_status
    **      Structure containing status information returned by the
    **      decoder.
    **
    **  adi_ac3d_audio_data_t *output_pcm_data[]
    **      Array of pointers to PCM buffers.  Each element must contain
    **      the pointer to the start of a buffer of type
    **      "adi_ac3d_audio_data_t" where the decoder will write the PCM
    **      output samples for one channel.  Each buffer must be large
    **      enough to accomodate
    **      ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN PCM samples.  The
    **      channel pointers must be stored in the following order:
    **
    **      [0] - Left Front.
    **      [1] - Right Front.
    **      [2] - Left Surround.  This buffer is also used for single
    **            surround output.
    **      [3] - Right Surround.
    **      [4] - Center.
    **      [5] - Low frequency effects (LFE).
    **
    **      If one buffer is to be used for all channels, it must be of
    **      size:
    **
    **      ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN * \
    **          ADI_AC3D_MAX_NUM_OUTPUT_CHANS
    **
    **      The first pointer (Left Front) must be intialized to the start
    **      of this buffer.  The second (Right Front) to the address of
    **      ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN elements from the
    **      start of the buffer and so on for the remaining pointers.
    **
    ** Return value:
    **  One of the following codes is returned:
    **      ADI_AC3D_SUCCESS
    **      ADI_AC3D_NEED_MORE_INPUT
    **      ADI_AC3D_TIMEOUT_REACHED
    **      ADI_AC3D_INVALID_SYNC
    **      ADI_AC3D_INVALID_HEADER
    **      ADI_AC3D_INVALID_BITSTREAM
    **      ADI_AC3D_INVALID_BITSTREAM_SIDE_INFO
    **      ADI_AC3D_CRC1_FAILED
    **      ADI_AC3D_CRC2_FAILED
    **      ADI_AC3D_DECODING_AUDIO_BLOCK_FAILED
    **      ADI_AC3D_FRAME_BOUNDARY_OVERRUN
    **
    */

    /*adi_ac3d_return_code_t adi_ac3d_decoder(
        adi_ac3d_instance_t*,
        int,
        adi_ac3d_bitstream_data_t*,
        int,
        adi_ac3d_audio_data_t**,
        adi_ac3d_output_status_t*);*/

    adi_ac3d_return_code_t adi_ac3d_decoder(
        adi_ac3d_instance_t *instance_handle,
        int num_input_bytes,
        adi_ac3d_bitstream_data_t *input_bitstream,
        int input_byte_offset,
        adi_ac3d_audio_data_t *output_pcm_data[],
        adi_ac3d_output_status_t *decoder_status);

    /*
    **
    ** Function: adi_ac3d_get_frame_properties()
    **
    ** Description:
    **  This function is provided as a utility for extracting information
    **  about a frame.  This information is derived from the header of
    **  each AC-3 frame.  The maximum length of this header is 160 bits.
    **  Information returned includes frame size, sampling rate and bit
    **  rate.
    **
    ** Arguments:
    **  adi_ac3d_bitstream_data_t *input_bitstream (INPUT)
    **      Buffer containing the AC-3 bit-stream.  The first element in
    **      this buffer must contain the start of the desired frame
    **      header ("syncword").  This buffer must contain at least
    **      ADI_AC3D_MIN_FRAME_HEADER_NUMBYTES of bit-stream data.
    **
    **      This buffer cannot be circular (must be linear).  Use the
    **      function adi_ac3d_get_frame_properties_cb() below for a
    **      circular bit-stream buffer.
    **
    **  int input_byte_offset (INPUT)
    **      Tells the decoder the byte position within the memory word where the
    **      bitstream data starts within the memory location pointed to by
    **      input_bitstream
    **
    **  int use_unpacked_input_buffer (INPUT)
    **      Tells the decoder the bit-stream buffer is in "un-packed" mode, where
    **      LSB 16 bits are bit-stream while MSB 16 bits are rubbish data
    **
    ** Outputs:
    **  adi_ac3d_frame_properties_t *frame_properties
    **      Structure filled by the function with informatin extracted from the
    **      input bit-stream.  In the event the header contains errors, the
    **      information returned is this structure is meaningless.
    **
    ** Return value:
    **  One of the following codes is returned:
    **      ADI_AC3D_SUCCESS
    **      ADI_AC3D_NEED_MORE_INPUT
    **      ADI_AC3D_INVALID_SYNC
    **      ADI_AC3D_INVALID_HEADER
    **      ADI_AC3D_INVALID_BITSTREAM
    **
    */

    adi_ac3d_return_code_t adi_ac3d_get_frame_properties(
        adi_ac3d_bitstream_data_t *input_bitstream_ptr,
        adi_ac3d_frame_properties_t *frame_properties,
        int input_byte_offset,
        int use_unpacked_input_buffer);

    /*
    **
    ** Function: adi_ac3d_get_frame_properties_cb()
    **
    ** Description:
    **  Same as adi_ac3d_get_frame_properties(), except it operates on a
    **  circular bit-stream buffer.
    **
    ** Arguments:
    **  int circular_buffer_length (INPUT)
    **      Length of the bit-stream buffer.  This is not the number of
    **      actual bit-stream elements currently in the input buffer.
    **
    **  adi_ac3d_bitstream_data_t *circular_buffer_base (INPUT)
    **      Base address of the input buffer ("input_bitstream").
    **
    **  adi_ac3d_bitstream_data_t *input_bitstream (INPUT)
    **      See description of equivalent parameter for function
    **      adi_ac3d_get_frame_properties().
    **
    **  int input_byte_offset (INPUT)
    **      Tells the decoder the byte position within the memory word where the
    **      bitstream data starts within the memory location pointed to by
    **      input_bitstream
    **
    **  int use_unpacked_input_buffer (INPUT)
    **      Tells the decoder the bit-stream buffer is in "un-packed" mode, where
    **      LSB 16 bits are bit-stream while MSB 16 bits are rubbish data
    **
    ** Outputs:
    **  adi_ac3d_frame_properties_t *frame_properties
    **      See description for equivalent parameter for function
    **      adi_ac3d_get_frame_properties().
    **
    ** Return value:
    **  One of the following codes is returned:
    **      ADI_AC3D_SUCCESS
    **      ADI_AC3D_NEED_MORE_INPUT
    **      ADI_AC3D_INVALID_SYNC
    **      ADI_AC3D_INVALID_HEADER
    **      ADI_AC3D_INVALID_BITSTREAM
    **
    */

    adi_ac3d_return_code_t adi_ac3d_get_frame_properties_cb(
        int circular_buffer_length,
        adi_ac3d_bitstream_data_t *circular_buffer_base_ptr,
        adi_ac3d_bitstream_data_t *input_bitstream_ptr,
        adi_ac3d_frame_properties_t *frame_properties,
        int input_byte_offset,
        int use_unpacked_input_buffer);


#if defined(__cplusplus)
}
#endif

#endif /* ADI_AC3_DECODER_H */

/*
**
** EOF: $HeadURL: http://adasvn.spd.analog.com/audio/trunk/ac3-decoder/sharc/include/adi_ac3_decoder.h $
**
*/
