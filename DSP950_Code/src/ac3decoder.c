/******************************************************************************
Copyright (c) 2006-2009 Analog Devices, Inc.  All Rights Reserved.
This software is proprietary and confidential to Analog Devices,
Inc. and its licensors.
*******************************************************************************

$Revision: 135 $
$Date: 2006-06-05 11:09:54 +1000 (Mon, 05 Jun 2006) $

Project:    AC3 Decoder
Title:      File based demonstration
Author:     HIF

Description:
            This file contains a demonstration of the AC3 decoder API,
            centered around a file-based example.

References:
            [1] "Dolby Digital (AC-3) 5.1 Decoder Developer's Guide",
                KT-25, rev. 0.8

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <processor_include.h>
#include "lib/adi_ac3_decoder.h"
#include "lib/ac3decoder.h"
#include "playback_ctrl.h"
#include "ADDS_21489_EzKit.h"
//#include "DSP1Test.h"

/******************************************************************************
**          Switches
******************************************************************************/

/* Switch on to add playback control function. The playback_control
 * function executes the commands listed in "playback_cmd_list",
 * defined in "playback_control.c". */
//#define PLAYBACK_CONTROL

/* Switch on to enable linear buffer */
#define LINEAR_BUFFER

/******************************************************************************
**          #defines
******************************************************************************/

#define SYNC_WORD 0x0B77
//0x0B77
#define NUM_AUDIO_BLOCKS_PER_FRAME 6
#define NUM_BLOCK_REPEATS 6

#define BYTES_PER_CHAR              (CHAR_BIT / 8)
#define MAX_WAV_FILE_HEADER         (68 / BYTES_PER_CHAR) // in 32-bit words

#define NUM_PCM_SAMPLES_PER_FRAME \
    (ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN * ADI_AC3D_MAX_NUM_OUTPUT_CHANS)
#define NUM_PCM_SAMPLES_PER_FRAME_PACKED_2x16 (NUM_PCM_SAMPLES_PER_FRAME / 2)

const char *AC3_FILENAME = "../../../../Media/DolbyDigitalAC-3_5.1Decoder/ada_6_channels.ac3";
const char *WAV_FILENAME = "../../../../Media/DolbyDigitalAC-3_5.1Decoder/ada_6_channels.wav";
const char *RESULTS_FILENAME   = "results.txt";
const char *CYCLE_CNT_FILENAME = "cycle_counts.txt";

/******************************************************************************
**          Local variables
******************************************************************************/

//section("adi_fast_prio4_rw")
//section("seg_sdram0")
adi_ac3d_bitstream_data_t bitstream_buffer[BITSTREAM_BUFFER_NUMWORDS];

//section("adi_slow_noprio_rw")
//section("seg_sdram0")
adi_ac3d_audio_data_t output_buffer[
    ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN *ADI_AC3D_MAX_NUM_OUTPUT_CHANS];

//section("state_fastb1_prio0")
//section("seg_sdram0")
char instance_buffer[ADI_AC3D_FAST_PRIO0_STATE_MEM_NUM_CHARS];

//section("scratch_fast_prio2")
//section("seg_sdram0")
char scratch_buffer[ADI_AC3D_FAST_PRIO1_SCRATCH_MEM_NUM_CHARS];

//section("adi_slow_noprio_rw")
//section("seg_sdram0")
int temp_buffer[NUM_PCM_SAMPLES_PER_FRAME_PACKED_2x16];

// Buffer for WAVE header
unsigned int output_wav_header[MAX_WAV_FILE_HEADER];

section("adi_slow_noprio_r") int  *p_adi_sc3decoder_crackbreak; //wchg

adi_ac3d_mem_t adi_mem_block;

char *error_codes[] = {"ADI_AC3D_CRC1_FAILED",
                       "ADI_AC3D_CRC2_FAILED",
                       "ADI_AC3D_INVALID_BITSTREAM_SIDE_INFO",
                       "ADI_AC3D_DECODING_AUDIO_BLOCK_FAILED",
                       "ADI_AC3D_FRAME_BOUNDARY_OVERRUN",
                       "ADI_AC3D_INVALID_CONFIG_PARAMS",
                       "ADI_AC3D_NEED_MORE_INPUT",
                       "ADI_AC3D_INVALID_SYNC",
                       "ADI_AC3D_INVALID_HEADER",
                       "ADI_AC3D_INVALID_BITSTREAM",
                      };

char *chan_configs[] =
{
    "ADI_AC3D_0_0_CHAN_CONFIG", // 1+1 (ch1, ch2)
    "ADI_AC3D_1_0_CHAN_CONFIG", // 1/0 (center)
    "ADI_AC3D_2_0_CHAN_CONFIG", // 2/0 (left, right)
    "ADI_AC3D_3_0_CHAN_CONFIG", // 3/0 (left, center, right)
    "ADI_AC3D_2_1_CHAN_CONFIG", // 2/1 (left, right, surround)
    "ADI_AC3D_3_1_CHAN_CONFIG", // 3/1 (left, center, right, surround)
    "ADI_AC3D_2_2_CHAN_CONFIG", // 2/2 (left, right, left surr., right surr.)
    "ADI_AC3D_3_2_CHAN_CONFIG", // 3/2 (left, center, right, l.s., r.s.)
};

ac3block_t ac3block_m;

//extern int successcount;
//int statusset[300];

//初次设置或者出错后设置或切换场景后设置
int firestflag = 1;

/******************************************************************************
**          Main program
******************************************************************************/
unsigned int frame_num = 0;
unsigned int end_of_bitstream = 0;
unsigned int num_words_to_read;
unsigned int block_num;
unsigned int end_of_ac3block;
unsigned int num_words_read;
unsigned int num_words_in_buffer;
int i;
int playback_cmd_id = 2;
long current_time_us = 0;
int playback_status;
FILE *logfile;
FILE *cycle_cnt_file;

cycle_t start_cnt = 0;
cycle_t elapsed_cnt = 0;
cycle_t frame_cycle_cnt = 0;

adi_ac3d_instance_t *instance_handle;
adi_ac3d_config_t config_params;
adi_ac3d_frame_properties_t frame_properties;

//adi_mem_block.state.fast.prio0 = instance_buffer;
//adi_mem_block.scratch.fast.prio0 = scratch_buffer;

adi_ac3d_return_code_t status;
adi_ac3d_return_code_t prev_status = ADI_AC3D_SUCCESS;

adi_ac3d_output_status_t decoder_status;

bitstream_info_t bitstream_info;
wav_info_t wav_info;

adi_ac3d_bitstream_data_t *unconsumed_bitstream_ptr;
adi_ac3d_audio_data_t *out_buffers[ADI_AC3D_MAX_NUM_OUTPUT_CHANS];

adi_wav_t wav_out;

int words_written, total_samples_per_chan = 0;

int ch_strides[ADI_AC3D_MAX_NUM_OUTPUT_CHANS] = {1, 1, 1, 1, 1, 1};

int outChannelMap[ADI_AC3D_MAX_NUM_OUTPUT_CHANS]
/* AC3 Dec */     /*  WAVE File */
    = {LEFT,            /*    L       */
       RIGHT,           /*    R       */
       CENTER,          /*    C       */
       LFE,             /*   LFE      */
       LEFT_SURR,       /*    Ls      */
       RIGHT_SURR
 };     /*    Rs      */


int AC3Decoder_init()
{

    int i;
    adi_mem_block.state.fast.prio0 = instance_buffer;
    adi_mem_block.scratch.fast.prio0 = scratch_buffer;

    for(i = 0; i < ADI_AC3D_FAST_PRIO0_STATE_MEM_NUM_CHARS; i++)
        instance_buffer[i] = 0;

    for(i = 0; i < ADI_AC3D_FAST_PRIO1_SCRATCH_MEM_NUM_CHARS; i++)
        scratch_buffer[i] = 0;

    for(i = 0; i < ADI_AC3D_FAST_PRIO1_SCRATCH_MEM_NUM_CHARS; i++)
        temp_buffer[i] = 0;

    //print the AC3 Decoder library version
    //printf("Library version: %s \n", adi_ac3d_module_info.version_text);

    /*if((bitstream_info.ac3file = fopen(AC3_FILENAME, "rb")) == NULL){
        printf("\nUnable to open the input file '%s' for reading.\n",
               AC3_FILENAME);
        exit(1);
    }

    if((wav_info.wavfile = fopen(WAV_FILENAME, "wb")) == NULL){
        printf("\nUnable to open the output file '%s' for writing.\n",
               WAV_FILENAME);
        exit(1);
    }*/

    /*reserve space for the RIFF header in the output file */
    /*adi_wav_write_fileheader(&wav_out, MAX_WAV_FILE_HEADER * BYTES_PER_CHAR, output_wav_header);
    fseek(wav_info.wavfile, (wav_out.num_output_bytes_produced/BYTES_PER_CHAR), SEEK_SET);

    if((logfile = fopen(RESULTS_FILENAME, "wt")) == NULL){
        printf("\nUnable to open the log file '%s' for writing.\n",
               RESULTS_FILENAME);
        exit(1);
    }

    if((cycle_cnt_file = fopen(CYCLE_CNT_FILENAME, "wt")) == NULL){
        printf("\nUnable to open the log file '%s' for writing.\n",
               CYCLE_CNT_FILENAME);
        exit(1);
    }
    */

    // Create decoder instance.
    instance_handle = adi_ac3d_create(&adi_mem_block);

    if (instance_handle == NULL)
    {
        printf("\nFailed to create an instance of the decoder.\n");
        exit(1);
    }
    //printf("One instance of the decoder created successfully.\n");

    //Fill bitstream information.
#ifdef LINEAR_BUFFER
    bitstream_info.use_circular_buffer = DISABLE_CIRCULAR_BUFFER;
#else
    bitstream_info.use_circular_buffer = ENABLE_CIRCULAR_BUFFER;
#endif

    bitstream_info.use_unpacked_buffer = DISABLE_UNPACKED_BUFFER;
    bitstream_info.buffer_length = BITSTREAM_BUFFER_NUMWORDS;
    bitstream_info.buffer_base = bitstream_buffer;
    bitstream_info.bitstream_ptr = bitstream_buffer;
    bitstream_info.unconsumed_bitstream_ptr = bitstream_buffer;
    bitstream_info.num_bytes_in_buffer = 0;
    bitstream_info.input_byte_offset = 0;

    // Initialize pointers to channel buffers.
    for (i = 0; i < ADI_AC3D_MAX_NUM_OUTPUT_CHANS; i++)
    {
        out_buffers[i] = &output_buffer[
                             ADI_AC3D_MAX_OUTPUT_PCM_SAMPLES_PER_CHAN * i];
        wav_info.channel_ptrs[outChannelMap[i]] = out_buffers[i];
    }

    // Initialize config params with default values.
    fill_config_params(&bitstream_info, &frame_properties, &config_params);

    // Initialize decoder instance.
    status = adi_ac3d_init(instance_handle, &config_params);

    adi_ac3d_resynch(instance_handle);

    if (status == ADI_AC3D_INVALID_CONFIG_PARAMS)
    {
        printf("\nOne or more of the audio rendering configuration "
               "parameters were invalid.\n");
    }

    /*printf("\nThe decoder instance initialized with the following audio "
           "rendering configuration parameters:\n");
    printf("\nRender param: dyn_range_scale_lo:  %f",
           config_params.audio_render_params.dyn_range_scale_lo);
    printf("\nRender param: dyn_range_scale_ho:  %f",
           config_params.audio_render_params.dyn_range_scale_hi);
    printf("\nRender param: pcm_scale_factor:    %f",
           config_params.audio_render_params.pcm_scale_factor);
    printf("\nRender param: compression_mode:    %d",
           config_params.audio_render_params.compression_mode);
    printf("\nRender param: stereo_downmix_mode: %d",
           config_params.audio_render_params.stereo_downmix_mode);
    printf("\nRender param: dual_mono_mode:      %d",
           config_params.audio_render_params.dual_mono_mode);
    printf("\nRender param: karaoke_mode:        %d",
           config_params.audio_render_params.karaoke_mode);
    printf("\nRender param: lfe_on:              %d",
           config_params.audio_render_params.lfe_on);
    printf("\nRender param: output_chan_config:  %s",
           chan_configs[config_params.audio_render_params.output_chan_config]);
    */

    return 0;

}

void AC3Decoder_resetbuff()
{

    int i;

    for(i = 0; i < BITSTREAM_BUFFER_NUMWORDS; i++)
        bitstream_buffer[i] = 0;

    bitstream_info.use_unpacked_buffer = DISABLE_UNPACKED_BUFFER;
    bitstream_info.buffer_length = BITSTREAM_BUFFER_NUMWORDS;
    bitstream_info.buffer_base = bitstream_buffer;
    bitstream_info.bitstream_ptr = bitstream_buffer;
    bitstream_info.unconsumed_bitstream_ptr = bitstream_buffer;
    bitstream_info.num_bytes_in_buffer = 0;
    bitstream_info.input_byte_offset = 0;

    //adi_ac3d_resynch(instance_handle);
}

int AC3Decoder_getproperties()
{

    static int getpropflag = 0;
    static int countsyncframe = 0;
    int prevalue = bitstream_info.num_bytes_in_buffer;

    // Read the bit stream from the input file and save in the circular buffer
    num_words_to_read = BITSTREAM_BUFFER_NUMWORDS;
    end_of_ac3block = 0;
    end_of_ac3block = fill_bitstream_buffer(num_words_to_read, &bitstream_info);


    //if(bitstream_info.num_bytes_in_buffer>0x1300)
    //printf("bitstream_info.num_bytes_in_buffer>0x1000\n");
    int noframinblock = sync_cur_frame(&end_of_ac3block, &bitstream_info);
    if(noframinblock)
        return 1;

    countsyncframe++;

    //frame_properties.sample_rate = 48;
    //frame_properties.frame_size = 0x300;

    status = ADI_AC3D_SUCCESS;
    getpropflag = 0;
    while(getpropflag == 0) //(getpropflag==0){
    {
        // Get frame properties from input bit stream. Make sure the input
        // stream is synchronized before calling this function.
        if(bitstream_info.use_circular_buffer)
        {
            // for circular buffer
            status = adi_ac3d_get_frame_properties_cb(
                         bitstream_info.buffer_length,
                         bitstream_info.buffer_base,
                         bitstream_info.unconsumed_bitstream_ptr,
                         &frame_properties,
                         bitstream_info.input_byte_offset,
                         bitstream_info.use_unpacked_buffer);
        }
        else
        {
            // for linear buffer
            status = adi_ac3d_get_frame_properties(
                         bitstream_info.unconsumed_bitstream_ptr,
                         &frame_properties,
                         bitstream_info.input_byte_offset,
                         bitstream_info.use_unpacked_buffer);
        }

        if(status != ADI_AC3D_SUCCESS)
        {
            //printf("Unable to get frame properties\n");
            //exit(1);
            int noframinblock = sync_next_frame(&end_of_ac3block, &bitstream_info);
            if(noframinblock)
                return 1;
        }
        else
        {
            /*printf("\n\nFrame Properties:\n");
            printf("  sample_rate            = %d\n", frame_properties.sample_rate * 1000);
            printf("  frame_size             = %d (16-bit words)\n", frame_properties.frame_size);
            printf("  bs_info                = 0x%x\n", frame_properties.bs_info);
            printf("  bs_config              = 0x%x\n", frame_properties.bs_config);
            printf("  dialogue_normalization = 0x%x\n", frame_properties.dialogue_normalization);
            printf("  language_code          = 0x%x\n", frame_properties.language_code);
            printf("  audio_production_info  = 0x%x\n", frame_properties.bs_config);
            */
            getpropflag = 1;
            break;
        }
    }
    /*fprintf(logfile, "This is the log file containing a summary of "
            "decoding the input file '%s'\n", AC3_FILENAME);
    printf("\n\nDecoding input file '%s'\n", AC3_FILENAME);*/

    // also set up output WAVE file
    wav_out.wav_format = WAVE_FORMAT_PCM;
    wav_out.sample_rate = frame_properties.sample_rate * 1000;
    wav_out.num_channels = ADI_AC3D_MAX_NUM_OUTPUT_CHANS;
    wav_out.sample_num_bits = 16;

#ifdef DO_CYCLE_COUNTS
    fprintf(cycle_cnt_file, "Cycle log file for input file '%s'.\n",
            AC3_FILENAME);
    fprintf(cycle_cnt_file, "Cycle counts are provided for each frame.\n");
    fprintf(cycle_cnt_file, "The sampling rate extracted from the frame "
            "header from each frame is also printed.\n");
    fprintf(cycle_cnt_file, "A sampling rate of '0', implies unknown "
            "sampling rate (e.g. error in frame).\n");
    fprintf(cycle_cnt_file, "\n");
    fprintf(cycle_cnt_file, "Frame No  Cycle Count  Sampling Rate\n");
#endif

    if((bitstream_info.num_bytes_in_buffer <= frame_properties.frame_size * 2 + 16))
        return 1;
    return 0;
}

int AC3Decoder_decodeDMAblock()
{

    int errorr = 0;
    end_of_bitstream = 0;

    while((!end_of_bitstream) && (errorr == 0))
    {
        int noframinblock = 0;
        //noframinblock = sync_next_frame(&end_of_ac3block, &bitstream_info);

        //if(noframinblock)
        //return 0;

        if(bitstream_info.num_bytes_in_buffer <= frame_properties.frame_size * 2 + 16)
            return 0;


#ifdef PLAYBACK_CONTROL
        // playback control
        playback_status = playback_control( &current_time_us,
                                            &playback_cmd_id,
                                            instance_handle,
                                            &end_of_ac3block,
                                            &frame_num,
                                            &bitstream_info);

        if(playback_status == PLAYBACK_FAIL)
        {
            printf("playback control error! EXIT!\n");
            break;
        }
        else if(playback_status == PLAYBACK_STOP)
        {
            break;
        }
        printf("Time: %ld ms, ", current_time_us / 1000);
#endif
        // Loop for blocks within each frame
        for(block_num = 0; block_num < NUM_AUDIO_BLOCKS_PER_FRAME; block_num++)
        {

#ifdef DO_CYCLE_COUNTS
            START_CYCLE_COUNT(start_cnt);
#endif
            status = ADI_AC3D_SUCCESS;

            //decode the input bit stream
            status = adi_ac3d_decoder(
                         instance_handle,
                         bitstream_info.num_bytes_in_buffer,
                         bitstream_info.unconsumed_bitstream_ptr,
                         bitstream_info.input_byte_offset,
                         out_buffers,
                         &decoder_status);

#ifdef DO_CYCLE_COUNTS
            STOP_CYCLE_COUNT(elapsed_cnt, start_cnt);
            frame_cycle_cnt += elapsed_cnt;
#endif
            // write output and update args.
            /*words_written = adi_pcm_fwrite_float_to_fixed(wav_info.channel_ptrs,
                                                          ch_strides,
                                                          wav_out.num_channels,
                                                          (adi_pcm_format_t)wav_out.sample_num_bits,
                                                          decoder_status.num_output_samples_per_chan,
                                                          wav_info.wavfile);
                                                          */
            if(wav_out.num_channels > ADI_AC3D_MAX_NUM_OUTPUT_CHANS)
                wav_out.num_channels = ADI_AC3D_MAX_NUM_OUTPUT_CHANS;

#if 0//edit yuan 20161103
            pcm_sdramwriteblock_float_to_fixed(wav_info.channel_ptrs,
                                               ch_strides,
                                               wav_out.num_channels,
                                               (adi_pcm_format_t)wav_out.sample_num_bits,
                                               decoder_status.num_output_samples_per_chan);
#endif
                                       
                                               
            total_samples_per_chan += (CHAR_BIT * words_written) / (wav_out.sample_num_bits * wav_out.num_channels);

            //if(ADI_AC3D_SUCCESS == status)
            //successcount++;
            //statusset[successcount%300] = status;


            if(status == ADI_AC3D_SUCCESS ||
                    status == ADI_AC3D_TIMEOUT_REACHED ||
                    status == ADI_AC3D_NEED_MORE_INPUT/*||
                status == ADI_AC3D_CRC2_FAILED*/)
            {
                // Decoder has successfully run

                // Number bytes left in buffer
                bitstream_info.num_bytes_in_buffer -= decoder_status.num_input_bytes_consumed;

                // Update input buffer pointer
                bitstream_info.unconsumed_bitstream_ptr = decoder_status.unconsumed_input_data_ptr;

                // byte offset
                bitstream_info.input_byte_offset = decoder_status.unconsumed_input_byte_offset;

                // Update time
                current_time_us += (decoder_status.num_output_samples_per_chan * 1000 / config_params.frame_properties_ptr->sample_rate);

            }
            else
            {
                // if an error has been encountered, tell us about it,
                // but still write the output and keep running.
                /*print_error(frame_num,
                            block_num,
                            prev_status,
                            status,
                            logfile);
                            */
                prev_status = status;

                /*//Number bytes left in buffer
                bitstream_info.num_bytes_in_buffer -= decoder_status.num_input_bytes_consumed;

                // Update input buffer pointer
                bitstream_info.unconsumed_bitstream_ptr = decoder_status.unconsumed_input_data_ptr;

                // byte offset
                bitstream_info.input_byte_offset = decoder_status.unconsumed_input_byte_offset;

                // Update time
                current_time_us += (decoder_status.num_output_samples_per_chan * 1000 / config_params.frame_properties_ptr->sample_rate);
                */
                break;

            }
        }

        if(status != ADI_AC3D_SUCCESS /*&& status != ADI_AC3D_TIMEOUT_REACHED*/)
        {
            //if(sync_next_frame(&end_of_ac3block, &bitstream_info)){
            /*printf("\nError: Couldn't synchronise to the next frame, "
            		"after errors in the previous frame.");
            printf("\nTerminating decoding of input file '%s'",
            		AC3_FILENAME);
            break;
            }*/
            adi_ac3d_resynch(instance_handle);
        }
        else
        {

#ifdef LINEAR_BUFFER
            // Re-organize linear buffer. Move unconsumed bytes to the
            // begining of the buffer and ready to fill the rest.
            unconsumed_bitstream_ptr = bitstream_info.unconsumed_bitstream_ptr;
            bitstream_info.bitstream_ptr = bitstream_buffer;
            num_words_in_buffer = (bitstream_info.num_bytes_in_buffer + 3) / 4;
            for(i = 0; i < num_words_in_buffer; i++)
                *bitstream_info.bitstream_ptr++ = *unconsumed_bitstream_ptr++;
            bitstream_info.unconsumed_bitstream_ptr = bitstream_buffer;
#endif
        }

        if(end_of_ac3block && (bitstream_info.num_bytes_in_buffer <= frame_properties.frame_size * 2 + 16))
        {
            // Reached end of file or no more data left, "<=2" because
            // the num_bytes_in_buffer might contain two extra rubbish
            // bytes. fread returns the number_words_read but the size
            // of AC3 bit stream is not necessarily multiple of 32
            end_of_bitstream = 1;
        }
        else if(!end_of_ac3block)
        {
            // Fill the buffer
            num_words_to_read =
                (BITSTREAM_BUFFER_NUMWORDS * 4 -
                 bitstream_info.num_bytes_in_buffer) / 4;
            end_of_ac3block = fill_bitstream_buffer(num_words_to_read, &bitstream_info);
        }

        if(status != ADI_AC3D_NEED_MORE_INPUT)
        {
            frame_num++;
            frame_num = frame_num % 400;
        }
        //printf("Frame Num: %d\t", frame_num);

#ifdef DO_CYCLE_COUNTS
        printf("Cycle: %lu", frame_cycle_cnt);
        log_cycle_counts(frame_num, frame_cycle_cnt,
                         &frame_properties,
                         cycle_cnt_file);
        frame_cycle_cnt = 0;
#endif
        //printf("\n");
        //fprintf(logfile, "\nFrame number %02d processing complete.",
        //          frame_num);

        if((frame_num % 400) == 0)
        {
            /*
            // Set new pcm scale factor
            config_params.audio_render_params.pcm_scale_factor =
                1.5 - config_params.audio_render_params.pcm_scale_factor;

            // Reconfigure ac3_decoder
            status = adi_ac3d_reconfigure(instance_handle,
                                          ADI_AC3D_PCM_SCALE_FACTOR,
                                          &config_params);
            */
            //   printf("\nReconfigure pcm_scale_factor to %.2f\n",
            //      config_params.audio_render_params.pcm_scale_factor);
        }

        errorr = sync_next_frame(&end_of_ac3block, &bitstream_info);
        if ((bitstream_info.num_bytes_in_buffer <= frame_properties.frame_size * 2 + 16))
            end_of_bitstream = 1;

#ifdef LINEAR_BUFFER
        // Re-organize linear buffer. Move unconsumed bytes to the
        // begining of the buffer and ready to fill the rest.
        unconsumed_bitstream_ptr = bitstream_info.unconsumed_bitstream_ptr;
        bitstream_info.bitstream_ptr = bitstream_buffer;
        num_words_in_buffer = (bitstream_info.num_bytes_in_buffer + 3) / 4;
        for(i = 0; i < num_words_in_buffer; i++)
            *bitstream_info.bitstream_ptr++ = *unconsumed_bitstream_ptr++;

        bitstream_info.unconsumed_bitstream_ptr = bitstream_buffer;
#endif
        prev_status = ADI_AC3D_SUCCESS;
    }

    /* Go back to start of file and write RIFF header, now that the
     * length of the output data is known. */
    // wav_out.num_samples_per_channel = total_samples_per_chan;
    //  adi_wav_write_fileheader(&wav_out, MAX_WAV_FILE_HEADER * BYTES_PER_CHAR, output_wav_header);
    // fseek(wav_info.wavfile, 0L, SEEK_SET);
    // fwrite(output_wav_header,
    //         sizeof(char),
    //        wav_out.num_output_bytes_produced / BYTES_PER_CHAR,
    //        wav_info.wavfile);

    //  printf("\n\nOutput file: \"%s\"\n", WAV_FILENAME);
    //  print_wav_header_info(&wav_out);

    //    fclose(bitstream_info.ac3file);
    //  fclose(wav_info.wavfile);
    //  fclose(logfile);
    //   fclose(cycle_cnt_file);
    //   printf("\n\nProcessing file '%s' complete.\n", AC3_FILENAME);
    return 0;
}

void print_wav_header_info(adi_wav_t *wav)
{

    printf("WAVE Format:           ");
    if(wav->wav_format == WAVE_FORMAT_PCM)
        printf("WAVE_FORMAT_PCM\n");
    else
        printf("WAVE FORMAT UNSUPPORTED\n");

    printf("Number of channels:    %d\n", wav->num_channels);
    printf("Sampling frequency:    %d Hz\n", wav->sample_rate);
    printf("Sample width:          %d\n", wav->sample_num_bits);
    printf("Num of samples per ch: %d\n", wav->num_samples_per_channel);
}

int *ptr_tmpset[100];
int num_bytes_in_buffer_set[100];
int index_set = 0;
int fill_bitstream_buffer(unsigned int num_words, bitstream_info_t *bitstream_info)
{

    int num_words_b4_wraparound;
    int num_words_to_read;
    unsigned int num_words_read;
    unsigned int total_words_read;

    ptr_tmpset[index_set] = bitstream_info->bitstream_ptr;
    num_bytes_in_buffer_set[index_set] = bitstream_info->num_bytes_in_buffer;
    index_set = (index_set + 1) % 100;

    adi_ac3d_bitstream_data_t *bitstream_ptr = bitstream_info->bitstream_ptr;

    num_words_b4_wraparound = &(bitstream_info->buffer_base[BITSTREAM_BUFFER_NUMWORDS]) - bitstream_ptr;
    num_words_to_read = min(num_words, num_words_b4_wraparound);

    if(num_words_to_read < 0)
        printf("num_words_to_read error < 0 !");
    /*num_words_read = fread(bitstream_ptr,
                           sizeof(adi_ac3d_bitstream_data_t),
                           num_words_to_read,
                           bitstream_info->ac3file);*/
    num_words_read = readfromac3block_16(bitstream_ptr,
                                         sizeof(adi_ac3d_bitstream_data_t),
                                         num_words_to_read,
                                         &ac3block_m);

    total_words_read = num_words_read;
    bitstream_ptr = circptr(bitstream_ptr,
                            num_words_read,
                            bitstream_info->buffer_base,
                            bitstream_info->buffer_length);



    num_words_to_read = num_words - num_words_read;
    /*num_words_read = fread(bitstream_ptr,
                           sizeof(adi_ac3d_bitstream_data_t),
                           num_words_to_read,
                           bitstream_info->ac3file);
                           */

    num_words_read = readfromac3block_16(bitstream_ptr,
                                         sizeof(adi_ac3d_bitstream_data_t),
                                         num_words_to_read,
                                         &ac3block_m);


    bitstream_ptr = circptr(bitstream_ptr,
                            num_words_read,
                            bitstream_info->buffer_base,
                            bitstream_info->buffer_length);

    bitstream_info->num_bytes_in_buffer +=
        (total_words_read + num_words_read) * 4;
    bitstream_info->bitstream_ptr = bitstream_ptr;

    /*if(total_words_read + num_words_read>0x300)
    	printf("total_words_read + num_words_read>0x300)\n");
    */


    //if(bitstream_info->num_bytes_in_buffer>BITSTREAM_BUFFER_NUMWORDS*4)
    //	printf("bitstream_info->num_bytes_in_bufferBITSTREAM_BUFFER_NUMWORDS!\n");

    //if (feof(bitstream_info->ac3file)) {
    return (ifac3blockend(&ac3block_m)) ? 1 : 0;
}

int sync_cur_frame(unsigned int *end_of_ac3block, bitstream_info_t *bitstream_info)
{

    unsigned int sync_error = 0;
    unsigned int num_input_words;
    unsigned int num_output_words;
    unsigned int num_words_in_buffer;
    int i;

    if(*end_of_ac3block != 1)
        printf("*end_of_ac3block!=1\n");

    adi_ac3d_bitstream_data_t *unconsumed_bitstream_ptr = bitstream_info->unconsumed_bitstream_ptr;

    adi_ac3d_bitstream_data_t *buffer_base = bitstream_info->buffer_base;

    if (bitstream_info->num_bytes_in_buffer < 4)
        return 1;

    sync_error = 0;
    while(((*unconsumed_bitstream_ptr & 0xffff) != SYNC_WORD/*0x770b*/) &&
            ((*unconsumed_bitstream_ptr >> 16) != /*SYNC_WORD*/ SYNC_WORD) &&
            (!sync_error))
    {
        unconsumed_bitstream_ptr =
            circptr(unconsumed_bitstream_ptr,
                    sizeof(adi_ac3d_bitstream_data_t),
                    buffer_base,
                    BITSTREAM_BUFFER_NUMWORDS);
        bitstream_info->num_bytes_in_buffer = bitstream_info->num_bytes_in_buffer - 4;

        if(bitstream_info->num_bytes_in_buffer < 4)
        {
            //if(bitstream_info->num_bytes_in_buffer == 0){
            // check if buffer is empty
            if(!*end_of_ac3block)
                *end_of_ac3block = fill_bitstream_buffer(BITSTREAM_BUFFER_NUMWORDS, bitstream_info);
            else
                sync_error = 1;
        }
    }

    if ((*unconsumed_bitstream_ptr & 0xffff) == SYNC_WORD)
        bitstream_info->input_byte_offset = 0;
    else
        bitstream_info->input_byte_offset = 2;

    //if(bitstream_info->num_bytes_in_buffer > 4)
    //printf("bitstream_info->num_bytes_in_buffer>4\n");

#ifdef LINEAR_BUFFER
    // Re-organize linear buffer.  Move unconsumed bytes to the
    // begining of the buffer and ready to fill the rest
    bitstream_info->bitstream_ptr = bitstream_buffer;
    num_words_in_buffer = (bitstream_info->num_bytes_in_buffer + 3) / 4;
    for (i = 0; i < num_words_in_buffer; i++)
    {
        *bitstream_info->bitstream_ptr++ = *unconsumed_bitstream_ptr++;
    }

    unconsumed_bitstream_ptr = bitstream_buffer;

#endif

    bitstream_info->unconsumed_bitstream_ptr =
        unconsumed_bitstream_ptr;

    num_input_words = (BITSTREAM_BUFFER_NUMWORDS * 4 -
                       bitstream_info->num_bytes_in_buffer) / 4;

    if(*end_of_ac3block != 1)
        printf("*end_of_ac3block!=1\n");

    *end_of_ac3block = fill_bitstream_buffer(num_input_words, bitstream_info);
    //  if(bitstream_info->num_bytes_in_buffer>4)
    //   printf("bitstream_info->num_bytes_in_buffer>4\n");
    if(*end_of_ac3block != 1)
        printf("*end_of_ac3block!=1\n");

    return sync_error;
}

int num_bytes_in_buffer_pre_set[100];
int num_bytes_in_buffer_post_set[100];
int sync_next_frame(unsigned int *end_of_ac3block, bitstream_info_t *bitstream_info)
{

    unsigned int sync_error = 0;
    unsigned int num_input_words;
    unsigned int num_output_words;
    unsigned int num_words_in_buffer;
    int i;

    adi_ac3d_bitstream_data_t *unconsumed_bitstream_ptr = bitstream_info->unconsumed_bitstream_ptr;

    adi_ac3d_bitstream_data_t *buffer_base = bitstream_info->buffer_base;

    if (bitstream_info->num_bytes_in_buffer < 4)
        return 1;

    //测试
    num_bytes_in_buffer_pre_set[index_set] = ac3block_m.nextstartoffset;// bitstream_info->num_bytes_in_buffer;

    sync_error = 0;

    do
    {
        unconsumed_bitstream_ptr =
            circptr(unconsumed_bitstream_ptr,
                    sizeof(adi_ac3d_bitstream_data_t),
                    buffer_base,
                    BITSTREAM_BUFFER_NUMWORDS);
        bitstream_info->num_bytes_in_buffer = bitstream_info->num_bytes_in_buffer - 4;

        if(bitstream_info->num_bytes_in_buffer < 4)
        {
            //if (bitstream_info->num_bytes_in_buffer == 0) {
            // check if buffer is empty
            if(!*end_of_ac3block)
                *end_of_ac3block = fill_bitstream_buffer(BITSTREAM_BUFFER_NUMWORDS, bitstream_info);
            else
                sync_error = 1;
        }
    }
    while(((*unconsumed_bitstream_ptr & 0xffff) != SYNC_WORD/*0x770b*/) &&
            ((*unconsumed_bitstream_ptr >> 16) != /*SYNC_WORD*/ SYNC_WORD) &&
            (!sync_error));

    if((*unconsumed_bitstream_ptr & 0xffff) == SYNC_WORD)
        bitstream_info->input_byte_offset = 0;
    else
        bitstream_info->input_byte_offset = 2;

    //if(bitstream_info->num_bytes_in_buffer > 4)
    //printf("bitstream_info->num_bytes_in_buffer>4\n");

#ifdef LINEAR_BUFFER
    // Re-organize linear buffer.  Move unconsumed bytes to the
    // begining of the buffer and ready to fill the rest
    bitstream_info->bitstream_ptr = bitstream_buffer;
    num_words_in_buffer = (bitstream_info->num_bytes_in_buffer + 3) / 4;
    for (i = 0; i < num_words_in_buffer; i++)
        *bitstream_info->bitstream_ptr++ = *unconsumed_bitstream_ptr++;
    unconsumed_bitstream_ptr = bitstream_buffer;
#endif

    bitstream_info->unconsumed_bitstream_ptr = unconsumed_bitstream_ptr;

    num_input_words = (BITSTREAM_BUFFER_NUMWORDS * 4 - bitstream_info->num_bytes_in_buffer) / 4;

    if(*end_of_ac3block != 1)
        printf("*end_of_ac3block!=1\n");

    *end_of_ac3block = fill_bitstream_buffer(num_input_words, bitstream_info);
    //if(bitstream_info->num_bytes_in_buffer>4)
    //printf("bitstream_info->num_bytes_in_buffer>4\n");

    if(*end_of_ac3block != 1)
        printf("*end_of_ac3block!=1\n");
    //if(num_bytes_in_buffer_pre_set[index_set-1] !=ac3block_m.nextstartoffset)
    //printf("num_bytes_in_buffer_pre_set[index_set-1] !=ac3block_m.nextstartoffset\n");

    //测试
    num_bytes_in_buffer_post_set[index_set] = ac3block_m.nextstartoffset;// bitstream_info->num_bytes_in_buffer;
    //if(bitstream_info->num_bytes_in_buffer>2)
    //printf("bitstream_info->num_bytes_in_buffer>2!\n");
    return sync_error;
}

static void fill_config_params(bitstream_info_t *bitstream_info,
                               adi_ac3d_frame_properties_t *frame_properties,
                               adi_ac3d_config_t *config_params)
{
    config_params->audio_render_params.dyn_range_scale_lo  = 1.0;
    config_params->audio_render_params.dyn_range_scale_hi  = 1.0;
    config_params->audio_render_params.pcm_scale_factor    = 1.0;
    config_params->audio_render_params.compression_mode    = 2;
    config_params->audio_render_params.stereo_downmix_mode = 0;
    config_params->audio_render_params.dual_mono_mode      = 0;
    config_params->audio_render_params.karaoke_mode        = 3;
    config_params->audio_render_params.lfe_on              = 1;
    config_params->audio_render_params.output_chan_config  = ADI_AC3D_3_2_CHAN_CONFIG;

    config_params->num_prev_block_repeats = NUM_BLOCK_REPEATS;
    config_params->use_circular_bitstream_buffer = bitstream_info->use_circular_buffer;
    config_params->use_unpacked_input_buffer = bitstream_info->use_unpacked_buffer;
    config_params->circular_buffer_length = bitstream_info->buffer_length;
    config_params->circular_buffer_base = bitstream_info->buffer_base;
    config_params->frame_properties_ptr = frame_properties;
}

static void print_error(unsigned int frame_num,
                        unsigned int block_num,
                        adi_ac3d_return_code_t prev_status,
                        adi_ac3d_return_code_t status,
                        FILE *logfile)
{
    unsigned int error_code_idx;
    if(prev_status == ADI_AC3D_SUCCESS)
    {
        switch(status)
        {
        case ADI_AC3D_CRC1_FAILED:
            error_code_idx = 0;
            break;
        case ADI_AC3D_CRC2_FAILED:
            error_code_idx = 1;
            break;
        case ADI_AC3D_INVALID_BITSTREAM_SIDE_INFO:
            error_code_idx = 2;
            break;
        case ADI_AC3D_DECODING_AUDIO_BLOCK_FAILED:
            error_code_idx = 3;
            break;
        case ADI_AC3D_FRAME_BOUNDARY_OVERRUN:
            error_code_idx = 4;
            break;
        case ADI_AC3D_NEED_MORE_INPUT:
            error_code_idx = 5;
            break;
        case ADI_AC3D_INVALID_SYNC:
            error_code_idx = 6;
            break;
        case ADI_AC3D_INVALID_HEADER:
            error_code_idx = 7;
            break;
        case ADI_AC3D_INVALID_BITSTREAM:
            error_code_idx = 8;
            break;
        default:
            break;
        }
        fprintf(logfile, "\nFrame num: %d: Block num: %d: produced the "
                "error %s.",
                frame_num,
                block_num,
                error_codes[error_code_idx]);
    }

    switch(status)
    {
    case ADI_AC3D_CRC1_FAILED:
    case ADI_AC3D_CRC2_FAILED:
    case ADI_AC3D_INVALID_BITSTREAM_SIDE_INFO:
    case ADI_AC3D_DECODING_AUDIO_BLOCK_FAILED:
    case ADI_AC3D_FRAME_BOUNDARY_OVERRUN:
        if (prev_status == ADI_AC3D_SUCCESS)
            fprintf(logfile, "\nError concealment activated: "
                    "MODE: BLOCK REPEAT.");
        else
            fprintf(logfile, "\nFrame num: %d: Block num: %d: "
                    "error concealment active: MODE: BLOCK REPEAT",
                    frame_num,
                    block_num);
        break;
    case ADI_AC3D_NEED_MORE_INPUT:
    case ADI_AC3D_INVALID_SYNC:
    case ADI_AC3D_INVALID_HEADER:
    case ADI_AC3D_INVALID_BITSTREAM:
        if (prev_status == ADI_AC3D_SUCCESS)
            fprintf(logfile, "\nError concealment activated: "
                    "MODE: MUTE AUDIO");
        else
            fprintf(logfile, "\nFrame num: %d: Block num: %d: "
                    "error concealment active: MODE: MUTE AUDIO",
                    frame_num,
                    block_num);
        break;
    }
}

static void log_cycle_counts(unsigned int frame_num,
                             cycle_t frame_cycle_cnt,
                             adi_ac3d_frame_properties_t *frame_properties,
                             FILE *cycle_cnt_file)
{
    int sampling_rate;
    int bs_config;

    bs_config = frame_properties->bs_config;
    switch((bs_config & 0x0000c000) >> 14)
    {
    case 0:
        sampling_rate = 48000;
        break;
    case 1:
        sampling_rate = 44100;
        break;
    case 2:
        sampling_rate = 32000;
        break;
    default:
        sampling_rate = 0;
    }
    fprintf(cycle_cnt_file, "%02d        %lu   %d\n",
            frame_num,
            frame_cycle_cnt,
            sampling_rate);
}




///////////////////////////////////////////////////////////////////////
int setac3block(ac3block_t *pac3block, void *ptr, int blocklength)
{

    pac3block->nextstartoffset = 0;
    pac3block->pblockbuf = ptr;
    pac3block->blocklength = blocklength;

    if((blocklength % 4) != 0)
        printf("error : mod(blocklength,4) !=0 \n ");
    if(blocklength <= 0)
        printf("length error!");
    return 0;
}

int readfromac3block_16(adi_ac3d_bitstream_data_t *bitstream_ptr,
                        int sizeofword,
                        int num_words_to_read,
                        ac3block_t *pac3block)
{
    int num_words_readm4;
    int i;
    //static int *datasave = 0x300000;
    static int datasaveindex = 0;
    int tmplen;
    adi_ac3d_bitstream_data_t *ptr = bitstream_ptr;
    num_words_readm4 = min(num_words_to_read / 2, (pac3block->blocklength - pac3block->nextstartoffset) / 4);

    if((end_of_ac3block == 1) && (num_words_readm4 > 0))
        printf("((end_of_ac3block==1)&&(num_words_readm4>0))\n");
    if(num_words_readm4 <= 0)
        return 0;
    if(num_words_readm4 != 0x200)
        tmplen = num_words_readm4;

    //memcpy(datasave+datasaveindex*pac3block->blocklength,&(pac3block->pblockbuf[0]),pac3block->blocklength*sizeof(int));
    //datasaveindex = (datasaveindex+1)%200;

    for(i = 0; i < num_words_readm4; i++)
    {
        *ptr = ((pac3block->pblockbuf[pac3block->nextstartoffset + 1] & 0xFFFF0000) >> 0) |
               (((pac3block->pblockbuf[pac3block->nextstartoffset + 0] & 0xFFFF0000) >> 16) & 0xFFFF);
        //*ptr = (((*ptr&0x00FF00FF)<<8)|(((*ptr&0xFF00FF00)>>8)&0xFFFFFF));

        *(ptr + 1) = ((pac3block->pblockbuf[pac3block->nextstartoffset + 3] & 0xFFFF0000) << 0) |
                     (((pac3block->pblockbuf[pac3block->nextstartoffset + 2] & 0xFFFF0000) >> 16) & 0xFFFF);
        //*(ptr+1) = (((*(ptr+1)&0x00FF00FF)<<8)|(((*(ptr+1)&0xFF00FF00)>>8)&0xFFFFFF));

        pac3block->pblockbuf[pac3block->nextstartoffset + 0] = 0;
        pac3block->pblockbuf[pac3block->nextstartoffset + 1] = 0;
        pac3block->pblockbuf[pac3block->nextstartoffset + 2] = 0;
        pac3block->pblockbuf[pac3block->nextstartoffset + 3] = 0;
        ptr += 2;
        pac3block->nextstartoffset += 4;
    }

    if(pac3block->blocklength < pac3block->nextstartoffset)
        printf("error:pac3block->blocklength < pac3block->nextstartoffset\n");

    return num_words_readm4 * 2;
}

int ifac3blockend(ac3block_t *pac3block)
{

    if(pac3block->blocklength - pac3block->nextstartoffset == 0)
        return 1;
    else if(pac3block->blocklength - pac3block->nextstartoffset >= 4)
        return 0;
    else
    {
        //	printf("error : mod(blocklength,4) !=0 \n ");
        return 1;
    }
}


/*
**
** EOF: $HeadURL: svn://dingofiles.spd.analog.com/audio/trunk/common/templates/template.c $
**
*/
