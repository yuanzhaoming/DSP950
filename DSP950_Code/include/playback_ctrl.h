/******************************************************************************
Copyright (c) 2006-2009 Analog Devices, Inc.  All Rights Reserved.
This software is proprietary and confidential to Analog Devices,
Inc. and its licensors.
*******************************************************************************

$Revision: 11420 $
$Date: 2009-11-17 17:13:40 +1100 (Tue, 17 Nov 2009) $

Project:    AC3 Decoder
Title:      File Based Demonstration
Author:     LM

Description:
            This file contains functions to implement fast-forward and
            rewind

******************************************************************************/

#include <stdio.h>
#include "adi_ac3_decoder.h"
#include "ac3decoder.h"

#define MAX_PLAYBACK_CMD_LENGTH 1

#define CMD_REWIND          'r'
#define CMD_PLAY            'p'
#define CMD_FAST_FORWARD    'f'
#define CMD_STOP            's'

#define FAIL 0
#define SUCCESS 1

typedef enum
{
    PLAYBACK_SUCCESS = 0,
    PLAYBACK_FAIL = 1,
    PLAYBACK_STOP = 2,
} playback_result_t;

typedef struct
{
    char command[MAX_PLAYBACK_CMD_LENGTH];
    int time_ms;
} playback_cmd_t;

playback_result_t playback_control(
    long *current_time_us,
    int *playback_cmd_id,
    adi_ac3d_instance_t *instance_handle,
    unsigned int *end_of_file,
    unsigned int *frame_number,
    bitstream_info_t *bitstream_info);






/*
**
** EOF: $HeadURL: http://adasvn.spd.analog.com/audio/trunk/ac3-decoder/sharc/example/Source/playback_ctrl.h $
**
*/
