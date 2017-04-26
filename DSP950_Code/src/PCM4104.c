#include "./include/pcm4104.h"
#include "./include/ADDS_21489_EzKit.h"
#include <sysreg.h>
#include "math.h"
#include "./include/spi.h"
#include "sru.h"
#include "./include/cs4385a.h"
#include "./include/input_types.h"

#define NOP asm volatile("nop;")

 

/*
*		PCM4104  DA芯片
*		
*
*		数据控制部分
*		------------------------------------------------------
*		|    引脚        |      数据线      |     连接电阻   |
*       ------------------------------------------------------
*		|  DAI_P20       |     I2S_DA_BCLK  |      R9        |
*       ------------------------------------------------------
*		|  DAI_P10       |     I2S_DA_LRCLK |      R52       |
*       ------------------------------------------------------
*		|  DAI_P13       |     I2S_DA_DATA01|      R2        |
*       ------------------------------------------------------
*       |  DAI_P07       |     I2S_DA_DATA02|      R3        |
*       ------------------------------------------------------
*       |  DAI_P01       |     I2S_DA_DATA03|      R5        |
*       ------------------------------------------------------
*       |  DAI_P19       |     I2S_DA_DATA04|      R4        |
*       ------------------------------------------------------
*       |  DAI_P06       |     I2S_DA_DATA05|      R10       |
*       ------------------------------------------------------
*       |  DAI_P02       |     I2S_DA_DATA06|      R51       |
*       ------------------------------------------------------
*		|  DAI_P09       |     I2S_DA_DATA07|      R21       |
*       ------------------------------------------------------
*       |  DAI_P05       |     I2S_DA_DATA08|      R50       |
*       ------------------------------------------------------
*
*		
*		工作模式控制部分
*    	------------------------------------------------------
*		|    引脚        |      数据线      |     连接电阻   |
*       ------------------------------------------------------
*		|  DPI_P06       |     DA_RESET     |      R23       |
*       ------------------------------------------------------
*		|  DAI_P08       |     DA_MUTE      |      R73       |
*       ------------------------------------------------------
*
*
*
*		相应硬件控制管脚:
*
*    	-----------------------------------------------------------
*		|    FS0(28)     |     FS1(29)      |     SAMPLING MODE   |
*       -----------------------------------------------------------
*		|     0          |     0            |      single rate    |
*       -----------------------------------------------------------
*		|     0          |     1            |      dual rate      |
*       -----------------------------------------------------------
*		|     1          |     0            |      quad rate      |
*       -----------------------------------------------------------
*		|     1          |     1            |      ---not used    |
*       -----------------------------------------------------------
*
*   	----------------------------------------------------------------------
*		|  FMT2(27) |   FMT1(26)   |   FMT0(25)  |    音频格式               |
*       ----------------------------------------------------------------------
*		|     0     |      0       |      0      |    24-bit left justified  |
*       ----------------------------------------------------------------------
*		|     0     |      0       |      1      |    24-bit i2s             |
*       ----------------------------------------------------------------------
*		|     0     |      1       |      0      |    tdm with zero bck delay|
*       ----------------------------------------------------------------------
*		|     0     |      1       |      1      |    tdm with one bck delay |
*       ----------------------------------------------------------------------
*		|     1     |      0       |      0      |    24-bit right justified |
*       ----------------------------------------------------------------------
*		|     1     |      0       |      1      |    20-bit right justified |
*       ----------------------------------------------------------------------
*		|     1     |      1       |      0      |    18-bit right justified |
*       ----------------------------------------------------------------------
*		|     1     |      1       |      1      |    16-bit right justified |
*       ----------------------------------------------------------------------
*
*/



//section("seg_ext_dmda") 

section("seg_ext_dmda") int TxBlock_PCM4104[USE_PCM4104_DATALINE_MAX][CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS];
int	TCB_TxBlock_PCM4104[USE_PCM4104_DATALINE_MAX][CIRCLEDAMDATABUFFNUM][4];

static unsigned int PCI = 0x00080000;
static unsigned int OFFSET = 0x00080000;

float da_channel_buffer[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2];
float da_channel_buffer_multi[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2] = {0.0};


extern int g_input_type ;

void PCM4104_Pin_Init(void)
{
	if(g_input_type == INPUT_TYPE_AES)	
	{
		SRU(HIGH,PBEN20_I);			
		SRU(DAI_PB17_O,	DAI_PB20_I);
		
		SRU(HIGH,PBEN10_I);	
		SRU(DAI_PB16_O,	DAI_PB10_I);


		SRU(DAI_PB17_O, SPORT0_CLK_I);
		SRU(DAI_PB16_O, SPORT0_FS_I); 	


		SRU(DAI_PB17_O, SPORT1_CLK_I);
		SRU(DAI_PB16_O, SPORT1_FS_I);

		SRU(DAI_PB17_O, SPORT2_CLK_I);
		SRU(DAI_PB16_O, SPORT2_FS_I);

		SRU(DAI_PB17_O, SPORT3_CLK_I);
		SRU(DAI_PB16_O, SPORT3_FS_I);

		//DA-DATA01-----DAI.13	 	
		SRU(HIGH,PBEN13_I);
		SRU(SPORT0_DA_O,DAI_PB13_I);
		
		//DA-DATA02-----DAI.07	 
		SRU(HIGH,PBEN07_I);			
		SRU(SPORT0_DB_O,DAI_PB07_I);

		//DA-DATA03-----DAI.01		
		SRU(HIGH,PBEN01_I);
		SRU(SPORT1_DA_O,DAI_PB01_I);		
		
		//DA-DATA04-----DAI.19	
		SRU(HIGH,PBEN19_I);
		SRU(SPORT1_DB_O,DAI_PB19_I);
		
		//DA-DATA05-----DAI.06	
		SRU(HIGH,PBEN06_I);
		SRU(SPORT2_DA_O,DAI_PB06_I);
		
		//DA-DATA06-----DAI.02	
		SRU(HIGH,PBEN02_I);
		SRU(SPORT2_DB_O,DAI_PB02_I);
		
		//DA-DATA07-----DAI.09	
		SRU(HIGH,PBEN09_I);
		SRU(SPORT3_DA_O,DAI_PB09_I);
		
		//DA-DATA08-----DAI.05
		SRU(HIGH,PBEN05_I);
		SRU(SPORT3_DB_O,DAI_PB05_I);
	}
    else
  	{
		//DA-BCLK ----DAI.20
		SRU(HIGH,PBEN20_I);			
		SRU(PCG_CLKB_O,	DAI_PB20_I);
		SRU(PCG_CLKB_O, SPORT0_CLK_I); 

		//DA-LRCLK----DAI.10	 								
		SRU(HIGH,PBEN10_I);			
		SRU(PCG_FSB_O, SPORT0_FS_I);
		SRU(PCG_FSB_O, DAI_PB10_I);	
	
		//SPORT1
		SRU(PCG_CLKB_O, SPORT1_CLK_I);
		SRU(PCG_FSB_O, SPORT1_FS_I);
	
		//SPORT2
		SRU(PCG_CLKB_O, SPORT2_CLK_I);
		SRU(PCG_FSB_O, SPORT2_FS_I);
	
		//SPORT3
		SRU(PCG_CLKB_O, SPORT3_CLK_I);
		SRU(PCG_FSB_O, SPORT3_FS_I);	

		//DA-DATA01-----DAI.13	 	
		SRU(HIGH,PBEN13_I);
		SRU(SPORT0_DA_O,DAI_PB13_I);
	
		//DA-DATA02-----DAI.07	 
		SRU(HIGH,PBEN07_I);			
		SRU(SPORT0_DB_O,DAI_PB07_I);

		//DA-DATA03-----DAI.01		
		SRU(HIGH,PBEN01_I);
		SRU(SPORT1_DA_O,DAI_PB01_I);		
	
		//DA-DATA04-----DAI.19	
		SRU(HIGH,PBEN19_I);
		SRU(SPORT1_DB_O,DAI_PB19_I);
	
		//DA-DATA05-----DAI.06	
		SRU(HIGH,PBEN06_I);
		SRU(SPORT2_DA_O,DAI_PB06_I);
	
		//DA-DATA06-----DAI.02	
		SRU(HIGH,PBEN02_I);
		SRU(SPORT2_DB_O,DAI_PB02_I);
	
		//DA-DATA07-----DAI.09	
		SRU(HIGH,PBEN09_I);
		SRU(SPORT3_DA_O,DAI_PB09_I);
	
		//DA-DATA08-----DAI.05
		SRU(HIGH,PBEN05_I);
		SRU(SPORT3_DB_O,DAI_PB05_I);
  	}
}
 
 

/*
*
*	DA复位操作，让DA运行稳定
*
*
*/
void PCM4104_Reset(void)
{
    int i;
 
    //****************************reset DA**************************
    SRU(HIGH, DPI_PBEN06_I);
    SRU(LOW, DPI_PB06_I);  //DA_RESET
    for(i = 0; i < 65536 ; i++)
        NOP;

    SRU(HIGH, DPI_PB06_I);  //DA_RESET
    //Wait for recommended number of cycles
    for(i = 0; i < 4096; i++)
        NOP;      
}

/*
*
*		将DA静音
*
*
*/
void PCM4104_Mute(int i_choice)
{
    int i;

    SRU(HIGH, DPI_PBEN08_I);
  
    switch(i_choice)
    {
    case 0://unmute
  		SRU(LOW, DPI_PB08_I); 
    break;
    case 1://mute
    	SRU(HIGH, DPI_PB08_I); 	
    break;	
    }
   
    for(i = 0; i < 4096; i++)
    NOP;	
}


void PCM4104_InitData(void)
{
	int i = 0;
	int j = 0;	
	//Setting Up and Starting Chained DMA 
	// 1  .Clear the chain pointer register.
	*pCPSP0A = 0;
	*pCPSP0B = 0;
	*pCPSP1A = 0;
	*pCPSP1B = 0;
	*pCPSP2A = 0;
	*pCPSP2B = 0;
	*pCPSP3A = 0;
	*pCPSP3B = 0;
	//默认处理
   	//as the reference say
   	//   1. Clear all control registers
	*pSPCTL0  = 0;
	*pSPCTL1  = 0;
	*pSPCTL2  = 0;
	*pSPCTL3  = 0;
    
	// 3.   configure the dma parameter registers.index,modify,count etc..    
	for(i = 0 ; i < USE_PCM4104_DATALINE_MAX ; i ++)
	{
		for(j = 0; j < CIRCLEDMADATABUFFNUM; j++){
			TCB_TxBlock_PCM4104[i][j][0] = (unsigned int)(TCB_TxBlock_PCM4104[i][( j + 1 ) % CIRCLEDMADATABUFFNUM]) + 3 - OFFSET + PCI ;
			TCB_TxBlock_PCM4104[i][j][1] = NUM_SAMPLES * NUM_TX_SLOTS;														 
			TCB_TxBlock_PCM4104[i][j][2] = 1;																			  
			TCB_TxBlock_PCM4104[i][j][3] = (int)(&TxBlock_PCM4104[i][j * NUM_SAMPLES * NUM_TX_SLOTS]);	 								  
		}
	}
	
	// 3 .Write the address containing the index register value of the first TCB to the chain pointer register, which starts the chain.
	*pCPSP0A = (unsigned int)(TCB_TxBlock_PCM4104[0][0]) - OFFSET + 3 + PCI;
	*pCPSP0B = (unsigned int)(TCB_TxBlock_PCM4104[1][0]) - OFFSET + 3 + PCI;
	*pCPSP1A = (unsigned int)(TCB_TxBlock_PCM4104[2][0]) - OFFSET + 3 + PCI;
	*pCPSP1B = (unsigned int)(TCB_TxBlock_PCM4104[3][0]) - OFFSET + 3 + PCI;
	*pCPSP2A = (unsigned int)(TCB_TxBlock_PCM4104[4][0]) - OFFSET + 3 + PCI;
	*pCPSP2B = (unsigned int)(TCB_TxBlock_PCM4104[5][0]) - OFFSET + 3 + PCI;
	*pCPSP3A = (unsigned int)(TCB_TxBlock_PCM4104[6][0]) - OFFSET + 3 + PCI;
	*pCPSP3B = (unsigned int)(TCB_TxBlock_PCM4104[7][0]) - OFFSET + 3 + PCI;	
	
	*pSPCTL0 = ( 
				SPTRAN				//SPTRAN=1 transfer
				| OPMODE			//I2S mode 
	            | SLEN24 			//word length = 24
	            | SPEN_A 			//serial port A channel enable
	            | SCHEN_A 			//Enable serial port channal A DMA channing
	            | SDEN_A 			//Enable serial port channal A DMA
	            | L_FIRST 			//Left channal first
	            //| CKRE				//上升沿采样
	            
				| SPEN_B 			//serial port B channel enable
				| SCHEN_B 			//Enable serial port channal B DMA channing
	            | SDEN_B 					//Enable serial port channal B DMA
	            );
	            
	*pSPCTL1 = *pSPCTL0;
	*pSPCTL2 = *pSPCTL0;
	*pSPCTL3 = *pSPCTL0;

	for(i = 0; i < 10000; i++)
		NOP;
}


int get_current_dma_tx_block(void)
{
    int i;

    for(i = 0; i < CIRCLEDAMDATABUFFNUM; i++)
    {
        if(*pCPSP0A == (unsigned int)(TCB_TxBlock_PCM4104[0][i]) - OFFSET + 3 + PCI)
            break;
    }

    return i;

}

/*
*
*		禁止DMA
*
*/
void Stop_PCM4104_DMA(){

	*pCPSP0A = 0;    
	*pCPSP0B = 0;
	*pCPSP1A = 0;
	*pCPSP1B = 0;	
	*pCPSP2A = 0;
	*pCPSP2B = 0;
	*pCPSP3A = 0;
	*pCPSP3B = 0;
}
/*
*
*
*	测试的时候，只需要使用该函数即可，往里面放入数据
*
*/
void PCM_TEST_DATA( void )
{
	int i,j;
    for(i = 0 ; i < USE_PCM4104_DATALINE_MAX ; i ++)
    {
		for(j = 0 ; j < CIRCLEDMADATABUFFNUM * NUM_SAMPLES * NUM_TX_SLOTS ; j ++)
		{																			  
			TxBlock_PCM4104[i][j] = sin( (1/48.0) * PI * j )  * (1 << 20);
			//TxBlock_PCM4104[i][j] = 0;
		}								  	
    }		
}


void clear_pcm4104_data(void)
{
	int i,j;
    for(i = 0 ; i < USE_PCM4104_DATALINE_MAX ; i ++)
    {
		for(j = 0 ; j < CIRCLEDMADATABUFFNUM * NUM_SAMPLES * NUM_TX_SLOTS ; j ++)
		{																			  
			TxBlock_PCM4104[i][j] =0;
		}								  	
    }	
	
}



/*
*
*		注册中断
*	
*
*/
void Register_DA_Interrupt( void )
{
	interrupt(SIG_SP0, SP0ADCSend_ISR);	
	//interrupt(SIG_SP1, SIG_IGN);	
	//interrupt(SIG_SP2, SIG_IGN);	
	//interrupt(SIG_SP3, SIG_IGN);	
}

/*
*
*		PCG初始化
*
*
*/
 void Init_PCG(void )
{
	int i = 0;
	static int Init_PCG_FLAG = 0;	
	
	if(Init_PCG_FLAG == 1)
		return ;

	Init_PCG_FLAG = 1;

    //PCG的时钟输入
  	SRU(LOW,PBEN03_I); 											//PCG DAI_P03  （PCK clk input）
 	SRU(DAI_PB03_O,PCG_EXTB_I);									//PCG clock in
 	*pPCG_CTLB1 = (4) | CLKBSOURCE | FSBSOURCE | ((4/2)<<20);	//Division for clock
	*pPCG_CTLB0 = 512 | ENCLKB | ENFSB; 						//Frame syn Division
}


 

/* 
*	初始化DA芯片
*
*/
void PCM4104_Init(void)
{
	//PCG初始化
	Init_PCG();
	 
	Stop_PCM4104_DMA();
	//引脚初始化
	PCM4104_Pin_Init(  );
	//PCM4104初始化
	//PCM4104复位

	PCM4104_Reset();
  
	//设置DMA等
	PCM4104_InitData();	
	//注册DA中断
	Register_DA_Interrupt();
  
	//unmute
	PCM4104_Mute(0);

}

void reinit_da(void )
{
	Stop_PCM4104_DMA();
	clear_pcm4104_data();
	PCM4104_Init();	
}

