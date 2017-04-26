#ifndef _spdif_coax_h_
#define _spdif_coax_h_
#include "ADDS_21489_EzKit.h"


extern void Register_SPDIF_Interrupt();
extern void Register_COAX_Interrupt();

extern void Clear_SPDIF_DMA_Buffer(void);

extern void Disable_SPDIF_DMA(void);

extern void Disable_COAX_DMA(void);

extern void Register_SPDIF_Interrupt(void);
 
extern void Register_COAX_Interrupt(void);

extern void Init_SPDIF_COAX(int i_choice);
extern void SPDIF_Disable_Others(void);


extern int  RxBlock_SPDIF[MAXCAHNNELNUM][CIRCLEDAMDATABUFFNUM * NUM_SAMPLES * NUM_TX_SLOTS];

extern int Get_Current_Dma_Rxblock_SPDIF(void);

#endif

