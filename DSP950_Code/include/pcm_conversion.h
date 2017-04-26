/******************************************************************************
Copyright (c) 2009 Analog Devices, Inc. All Rights Reserved. This software
is proprietary and confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************

$Revision: 14025 $
$Date: 2010-07-23 11:01:02 +1000 (Fri, 23 Jul 2010) $

Project:    WAV PCM Utilities
Title:      PCM Utilities
Author:     SK

Description:
            This file contains all public typedefs and function
            prototypes

******************************************************************************/
#ifndef PCM_CONVERSION
#define PCM_CONVERSION

#include <stdio.h>


/******************************************************************************
**          Typedefs/Enumerations
******************************************************************************/

typedef enum
{
    PACKED_PCM_16 = 16,
    PACKED_PCM_24 = 24,
    PACKED_PCM_32 = 32,
    UNKNOWN_FORMAT = 0
} adi_pcm_format_t;


/******************************************************************************
**      Function Declarations
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

    /*
    **
    ** Function:            adi_pcm_float_to_fixed
    **
    ** Description:         a function to take an array of floating pt
    **                      samples and convert to fixed pt format, where
    **                      the output format is packed 16/24/32 bits and
    **                      the output buffer is an array of 32 bit ints
    **
    ** Arguments:
    **
    **  int *output_buffer
    **                      a pointer to the output buffer
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  float **input_buffer_ptrs
    **                      an array of pointers to the input buffer for
    **                      each channel
    **
    **  int *input_buffer_strides
    **                      an array which specifies the stride for each
    **                      input channel
    **
    **  int num_channels
    **                      the number of input channels
    **
    **  int num_samples
    **                      the number of floating pt samples to be
    **                      converted
    **
    **
    ** Outputs:             "output_buffer" contains fixed pt samples in the
    **                      format specified by "pcm_format"
    **
    ** Return value:        returns the number of 32 bit int elements used
    **                      to pack the output samples
    **
    **
    **
    */
    int adi_pcm_float_to_fixed(int *output_buffer,
                               adi_pcm_format_t pcm_format,
                               float **input_buffer_ptrs,
                               int *input_buffer_strides,
                               int num_channels,
                               int num_samples);


    /*
    **
    ** Function:            adi_pcm_fixed_32bit_to_fixed
    **
    ** Description:         a function to take an array of 32 bit fixed pt
    **                      samples and convert to a specified fixed pt
    **                      format, where the output format is packed
    **                      16/24/32 bits and the output buffer is an
    **                      array of 32 bit ints
    **
    ** Arguments:
    **
    **  int *output_buffer
    **                      a pointer to the output buffer
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  int **input_buffer_ptrs
    **                      an array of pointers to the input buffer for
    **                      each channel
    **
    **  int *input_buffer_strides
    **                      an array which specifies the stride for each
    **                      input channel
    **
    **  int num_channels
    **                      the number of input channels
    **
    **  int num_samples
    **                      the number of 32 bit fixed pt samples to be
    **                      converted
    **
    **
    ** Outputs:             "output_buffer" contains fixed pt samples in the
    **                      format specified by "pcm_format"
    **
    ** Return value:        returns the number of 32 bit int elements used
    **                      to pack the output samples
    **
    **
    **
    */
    int adi_pcm_fixed_32bit_to_fixed(int *output_buffer,
                                     adi_pcm_format_t pcm_format,
                                     int **input_buffer_ptrs,
                                     int *input_buffer_strides,
                                     int num_channels,
                                     int num_samples);


    /*
    **
    ** Function:            adi_pcm_fixed_to_float
    **
    ** Description:         a function to take an array of fixed pt
    **                      samples and convert to floating pt format,
    **                      where the input format is packed 16/24/32 bits
    **                      and the output buffer is an array of floats
    **
    ** Arguments:
    **
    **  float **output_buffer_ptrs
    **                      an array of pointers to the output buffer for
    **                      each channel
    **
    **  int *output_buffer_strides
    **                      an array which specifies the stride for each
    **                      output channel
    **
    **  int num_channels
    **                      the number of output channels
    **
    **  int *input_buffer
    **                      a pointer to the input buffer
    **
    **  int *input_byte_offset
    **                      a pointer to the input bit offset
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  int num_samples
    **                      the number of fixed pt samples to be converted
    **
    **
    ** Outputs:             "output_buffer" contains floating pt samples
    **
    ** Return value:        returns number of 32 bit float values written
    **                      to the output buffer
    **
    **
    **
    */
    int adi_pcm_fixed_to_float(float **output_buffer_ptrs,
                               int *output_buffer_strides,
                               int num_channels,
                               int *input_buffer,
                               int *input_byte_offset,
                               adi_pcm_format_t pcm_format,
                               int num_samples);


    /*
    **
    ** Function:            adi_pcm_fixed_to_fixed_32bit
    **
    ** Description:         a function to take an array of fixed pt
    **                      samples and convert to 32 but fixed pt format,
    **                      where the input format is packed 16/24/32 bits
    **                      and the output buffer is an array of 32 bit
    **                      integers
    **
    ** Arguments:
    **
    **  int **output_buffer_ptrs
    **                      an array of pointers to the output buffer for
    **                      each channel
    **
    **  int *output_buffer_strides
    **                      an array which specifies the stride for each
    **                      output channel
    **
    **  int num_channels
    **                      the number of output channels
    **
    **  int *input_buffer
    **                      a pointer to the input buffer
    **
    **  int *input_byte_offset
    **                      a pointer to the input byte offset
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  int num_samples
    **                      the number of fixed pt samples to be converted
    **
    **
    ** Outputs:             "output_buffer" contains the output samples
    **
    ** Return value:        returns number of 32 bit integer values written
    **                      to the output buffer
    **
    **
    **
    */
    int adi_pcm_fixed_to_fixed_32bit(int **output_buffer_ptrs,
                                     int *output_buffer_strides,
                                     int num_channels,
                                     int *input_buffer,
                                     int *input_byte_offset,
                                     adi_pcm_format_t pcm_format,
                                     int num_samples);


    /*
    ** Function:            adi_pcm_fwrite_float_to_fixed
    **
    ** Description:         a function to take an array of floating pt
    **                      samples and convert to fixed pt format, where
    **                      the output format is packed 16/24/32 bits and
    **                      the output is written to a specified file
    **
    ** Arguments:
    **
    **  float **input_buffer_ptrs
    **                      an array of pointers to the input buffer for
    **                      each channel
    **
    **  int *input_buffer_strides
    **                      an array which specifies the stride for each
    **                      input channel
    **
    **  int num_channels
    **                      the number of input channels
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  int num_samples
    **                      the number of floating pt samples to be
    **                      converted
    **  FILE *output_file
    **                      the file pointer to the file where the packed
    **                      output samples will be written to
    **
    **
    ** Outputs:             the file pointed to by "output_file" contains
    **                      fixed pt samples in the format specified by
    **                      "pcm_format"
    **
    ** Return value:        returns the number of 32 bit int elements
    **                      written to the output file
    **
    **
    **
    */
    int adi_pcm_fwrite_float_to_fixed(float **input_buffer_ptrs,
                                      int *input_buffer_strides,
                                      int num_channels,
                                      adi_pcm_format_t pcm_format,
                                      int num_samples,
                                      FILE *output_file);


    /*
    ** Function:            adi_pcm_fwrite_fixed_32bit_to_fixed
    **
    ** Description:
    **
    **                      a function to take an array of 32 bit fixed pt
    **                      samples and convert to fixed pt format, where
    **                      the output format is packed 16/24/32 bits and
    **                      the output is written to a specified file
    **
    ** Arguments:
    **
    **  int **input_buffer_ptrs
    **                      an array of pointers to the input buffer for
    **                      each channel
    **
    **  int *input_buffer_strides
    **                      an array which specifies the stride for each
    **                      input channel
    **
    **  int num_channels
    **                      the number of input channels
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  int num_samples
    **                      the number of 32 bit fixed pt samples to be
    **                      converted
    **
    **  FILE *output_file
    **                      the file pointer to the file where the packed
    **                      output samples will be written to
    **
    **
    ** Outputs:             the file pointed to by "output_file" contains
    **                      fixed pt samples in the format specified by
    **                      "pcm_format"
    **
    ** Return value:        returns the number of 32 bit int elements
    **                      written to the output file
    **
    **
    **
    */
    int adi_pcm_fwrite_fixed_32bit_to_fixed(int **input_buffer_ptrs,
                                            int *input_buffer_strides,
                                            int num_channels,
                                            adi_pcm_format_t pcm_format,
                                            int num_samples,
                                            FILE *output_file);


    /*
    ** Function:            adi_pcm_fread_fixed_to_float
    **
    ** Description:         a function to take an array of fixed pt
    **                      samples from file and convert to floating pt
    **                      format, where the input format is packed
    **                      16/24/32 bits and the output buffer is an
    **                      array of floats
    **
    ** Arguments:
    **
    **  float **output_buffer_ptrs
    **                      an array of pointers to the output buffer for
    **                      each channel
    **
    **  int *output_buffer_strides
    **                      an array which specifies the stride for each
    **                      output channel
    **
    **  int num_channels
    **                      the number of output channels
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  int num_samples
    **                      the number of fixed pt samples to be converted
    **
    **  FILE *input_file
    **                      the file pointer to the file where the packed
    **                      input samples will be read from
    **
    **  int *input_byte_offset
    **                      a pointer to the input bit offset
    **
    **
    ** Outputs:             "output_buffer" contains floating pt samples
    **
    ** Return value:        returns number of 32 bit float values written
    **                      to the output buffer
    **
    **
    **
    */
    int adi_pcm_fread_fixed_to_float(float **output_buffer_ptrs,
                                     int *output_buffer_strides,
                                     int num_channels,
                                     adi_pcm_format_t pcm_format,
                                     int num_samples,
                                     FILE *input_file,
                                     int *input_byte_offset);


    /*
    ** Function:            adi_pcm_fread_fixed_to_fixed_32bit
    **
    ** Description:         a function to take an array of fixed pt
    **                      samples from file and convert to 32 but fixed
    **                      pt format, where the input format is packed
    **                      16/24/32 bits and the output buffer is an
    **                      array of 32 bit integers
    **
    ** Arguments:
    **
    **  int **output_buffer_ptrs
    **                      an array of pointers to the output buffer for
    **                      each channel
    **
    **  int *output_buffer_strides
    **                      an array which specifies the stride for each
    **                      output channel
    **
    **  int num_channels
    **                      the number of output channels
    **
    **  adi_pcm_format_t pcm_format
    **                      specifies the format of the fixed pt samples
    **
    **  int num_samples
    **                      the number of fixed pt samples to be converted
    **
    **  FILE *input_file
    **                      the file pointer to the file where the packed
    **                      input samples will be read from
    **
    **  int *input_byte_offset
    **                      a pointer to the input byte offset
    **
    **
    ** Outputs:             "output_buffer" contains the output samples
    **
    ** Return value:        returns number of 32 bit integer values written
    **                      to the output buffer
    **
    **
    **
    */
    int adi_pcm_fread_fixed_to_fixed_32bit(int **output_buffer_ptrs,
                                           int *output_buffer_strides,
                                           int num_channels,
                                           adi_pcm_format_t pcm_format,
                                           int num_samples,
                                           FILE *input_file,
                                           int *input_byte_offset);


#if defined(__cplusplus)
}
#endif

#endif /* PCM_CONVERSION */

/*
**
** EOF: $HeadURL: http://adasvn.spd.analog.com/audio/tags/common/components/pcm-utils/1.1.0/src/pcm_conversion.h $
**
*/
