#ifndef _pcm4104_h_
#define _pcm4104_h_

#include "ADDS_21489_EzKit.h"

#define USE_PCM4104_DATALINE_MAX 8

extern int TxBlock_PCM4104[USE_PCM4104_DATALINE_MAX][CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS];

extern float  da_channel_buffer[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2];
extern float  da_channel_buffer_multi[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2];


extern int g_i_da_interrupt_flag;
extern int g_i_da_pos_counter ;
extern int g_i_da_write_addr;

extern void Init_PCG(void );

extern void Stop_PCM4104_DMA(void );
extern void PCM4104_Mute(int i_choice);

extern void reinit_da(void );


extern void PCM4104_Init(void);

extern int get_current_dma_tx_block(void);

#endif
