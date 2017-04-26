#ifndef _channel_to_da_port_h_
#define _channel_to_da_port_h_
#include "ADDS_21489_EzKit.h"
#include "pcm4104.h"

extern int outsignalchannel_book24m3[MAXCAHNNELNUM][3] ;

extern int matrixtooutputchannel(float p_multichannelbuf[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2]
	,float p_outchanneloutputbuf[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2] );

extern int outpcmDAdatabuf_link( void );


extern int inputADdatabuf_link( void );

extern int inputSPDIFdatabuf_link( void );

	
#endif






