#ifndef _cs5368_h_
#define _cs5368_h_

#include "ADDS_21489_EzKit.h"

extern void Disable_AD_DMA(void);

extern void AD_AES_Init(int i_choice );

extern void AES_Disable_Others(void);

extern void AD_Disable_Others(void);

extern int Get_Current_Dma_CS5368_Rxblock(void);

extern void AD_PowerOff(void);

extern void AES_Reset(void);

extern void AES_PowerOff(void);

extern void clear_ad_buffer(void);
extern void AD_MIC_Select(int i_choice);
extern void AD_AES_Select(int i_choice);




#define MAININPUT_SPORTNUM 		4//输入通道数量,这是按照数据线的数量定义的
extern int RxBlock_I2S_CS5368[MAININPUT_SPORTNUM][CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS];


#endif

