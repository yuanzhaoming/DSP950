#include "./include/spdif_coax.h"
#include <def21489.h> 
#include <cdef21489.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>
#include "string.h"
#include "./include/cs5368.h"
#include "./include/pcm4104.h"
 
  
#define NOP asm volatile("nop;")

#define USE_SRC 1

/*
*		SPDIF  COAX
*		
*
*		数据控制部分
*		------------------------------------------------------
*		|    引脚        |      数据线      |     连接电阻   |
*       ------------------------------------------------------
*		|  DAI_P11       |  COAX_OPTICAL_IN |      R122      |
*       ------------------------------------------------------
*
*		
*		工作模式控制部分
*    	------------------------------------------------------
*		|    引脚        |      数据线       |     连接电阻   |
*       ------------------------------------------------------
*		|  DPI_P12       |  COAX_OPTICAL_SEL |      R60       |
*       ------------------------------------------------------
*/
//SPDIF RX 
section("seg_ext_dmda")   int RxBlock_SPDIF[MAXCAHNNELNUM][CIRCLEDAMDATABUFFNUM * NUM_SAMPLES * NUM_TX_SLOTS];
//section("seg_ext_dmda") int	TCB_RxBlock_SPDIF[ CIRCLEDAMDATABUFFNUM ][4];
int	TCB_RxBlock_SPDIF[ CIRCLEDAMDATABUFFNUM ][4];
static unsigned int PCI = 0x00080000;
static unsigned int OFFSET = 0x00080000;


/*
*		2016-12-21 yuan
*		使用src方法为:
*			spdif入，则有位时钟线和帧时钟线
*			
*			spdif_bck  :--->SRC1_CLK_IP_I
*			spdif_lrclk:--->SRC1_FS_IP_I
*						--->SRC1_DAT_IP_I
*
*
*			出:则用PCG产生的时钟，将其处理掉即可
*
*			out_bck	   :---->SRC1_CLK_OP_I
*			out_lrclk  :---->SRC1_FS_OP_I
*			out_data   :---->SRC1_DAT_OP_O
*
*
*		要组成这样的结构:
*		spdif进入信号	                  spdif输出信号
*		44.1KHZ        -------------->    48KHZ
*		2.82MHZ							  6.14MHZ
*
*/
void InitSRC(void)
{
	*pSRCCTL0 = SRC1_IN_I2S | SRC1_OUT_I2S | SRC1_OUT_24;
	*pSRCCTL0 |= SRC1_ENABLE;
}

static int g_i_init_spdif_once = 0;


void Init_SPDIF_Pin(void)
{ 
	int i = 0;


		// SPDIF Setup code goes here
		//*pDITCTL = (DIT_EN | DIT_IN_I2S | DIT_AUTO);
		//disable sport4.
		SRU(LOW, DIR_I);
		SRU(LOW,DAI_PB11_I);
		SRU(LOW,SPORT4_CLK_I);
		SRU(LOW,SPORT4_FS_I);
		SRU(LOW,SPORT4_DA_I);

		SRU(LOW,SRC1_CLK_OP_I);
		SRU(LOW,SRC1_FS_OP_I);

		SRU(LOW,SRC1_CLK_IP_I);
		SRU(LOW,SRC1_FS_IP_I);
		SRU(LOW,SRC1_DAT_IP_I);

		for(i = 0 ; i < 1000 ; i ++);

		*pDIRCTL = 0;//or ~DIR_PLLDIS, not sure yet
		
		
		#if USE_SRC
			//初始化SRC。
			InitSRC();
			Init_PCG();
			//使用SPDIF的DAI接口   
			SRU(DAI_PB11_O, DIR_I);
			SRU(DIR_CLK_O, SRC1_CLK_IP_I);
			SRU(DIR_FS_O, SRC1_FS_IP_I);
			SRU(DIR_DAT_O, SRC1_DAT_IP_I);
			
			//添加测试，查看DIR_TDMCLK_O的输出频率，SRC1_TDM_OP_I
			
			
			
			//信号输出状态
			SRU(PCG_CLKB_O, SPORT4_CLK_I); 
			SRU(PCG_FSB_O, SPORT4_FS_I);		
			SRU(PCG_CLKB_O, SRC1_CLK_OP_I);
			SRU(PCG_FSB_O, SRC1_FS_OP_I);
			
			SRU(SRC1_DAT_OP_O,SPORT4_DA_I);		
		#else
		    //选择的路径为SPDIF
			//使用SPDIF的DAI接口   
			SRU(DAI_PB11_O, DIR_I);
			//Clock in from SPDIF RX
			SRU(DIR_CLK_O, SPORT4_CLK_I);
			//Frame sync from SPDIF RX
			SRU(DIR_FS_O, SPORT4_FS_I);
			//Data in from SPDIF RX
			SRU(DIR_DAT_O, SPORT4_DA_I); 
			
			//添加测试，查看DIR_TDMCLK_O的输出频率，SRC1_TDM_OP_I
			
			
			 
		#endif
		Register_SPDIF_Interrupt();

	
/*	测试信号，可以使用这些内容来做。
*
*		|  DAI_P13       |     I2S_DA_DATA01|      R2        |
*       ------------------------------------------------------
*       |  DAI_P07       |     I2S_DA_DATA02|      R3        |  
*/
	//将数据输出到DAI_P13,DAI_P07.
//	SRU(HIGH, PBEN13_I); 
//	SRU(HIGH, PBEN07_I); 
//	SRU(DIR_CLK_O, DAI_PB07_I);
//	SRU(DIR_FS_O, DAI_PB13_I);	 
}

/*
*
*		SPDIF:1
*		COAX: 0
*
*
*/
void SPDIF_COAX_SEL(int i_choice)
{
	SRU(HIGH, DPI_PBEN12_I);
	
	switch(i_choice)
	{
	case 0:
		SRU(LOW, DPI_PB12_I);  //DPI12:COAX	
	break;
	case 1:
		SRU(HIGH, DPI_PB12_I); //DPI12:SPDIF		
	break;	
	}	
}

/*
*
*
*/
void SPDIF_InitData(void)
{
	int i = 0;
	int j = 0;	
	
	//Setting Up and Starting Chained DMA 
	//1.Clear the chain pointer register.
	*pCPSP4A = 0;
	
	//2.For internal memory transfers, set up all TCBs in internal memory.
	*pDIV4 = 0x00000000;//0x00000000;  // Transmitter (SPORT0)
	
	for(j = 0; j < CIRCLEDMADATABUFFNUM; j++){
		TCB_RxBlock_SPDIF[j][0] = (unsigned int)(TCB_RxBlock_SPDIF[( j + 1 ) % CIRCLEDMADATABUFFNUM]) + 3 - OFFSET + PCI ;//
		TCB_RxBlock_SPDIF[j][1] = NUM_SAMPLES * NUM_TX_SLOTS;															  //
		TCB_RxBlock_SPDIF[j][2] = 1;																					  //
		TCB_RxBlock_SPDIF[j][3] = (int)(&RxBlock_SPDIF[0][ j * NUM_SAMPLES * NUM_TX_SLOTS ]); 								  //
	}
	
	//3.Write the address containing the index register value of the first TCB to the chain pointer register, which starts the chain.
	*pCPSP4A = (unsigned int)(TCB_RxBlock_SPDIF[0]) - OFFSET + 3 + PCI;
	
	//4.Write to the SPCTLx register by setting the DMA enable bit to one and the chaining enable bit to one. Setting these bits loads the
	//DMA parameter registers.
	            
    	*pSPCTL4 = ( OPMODE		//I2S mode
	     | SLEN32 		//word length = 32
	     | SPEN_A 		//serial port A channel enable
	     | SCHEN_A 		//Enable serial port channal A DMA channing
	     | SDEN_A 		//Enable serial port channal A DMA
	     | L_FIRST 		//Left channal first
	     | CKRE
     	//  | DIFS               
   	);  	                   	
	for(i = 0; i < 10000; i++)
    		NOP;		
}

/*
*
*
*
*/
void Clear_SPDIF_DMA_Buffer(void)
{
	memset(RxBlock_SPDIF[0],0,NUM_SAMPLES * NUM_TX_SLOTS*CIRCLEDMADATABUFFNUM*sizeof(int));		
}
/*
*
*
*
*/
void Disable_SPDIF_DMA(void)
{
	int i = 0;
	*pSPCTL4 = 0;	
	*pCPSP4A = 0;
	for(i = 0 ; i < 1000; i ++);
	
}


/*
*
*
*
*/
int Get_Current_Dma_Rxblock_SPDIF(void){

	int i;
	for (i = 0; i < CIRCLEDAMDATABUFFNUM; i++){
		if (*pCPSP4A == (unsigned int)(TCB_RxBlock_SPDIF[i]) - OFFSET + 3 + PCI)
			break;
	}
	return (i < CIRCLEDAMDATABUFFNUM) ? i : 0;
}


/*
*
*
*
*/
void SPDIF_Disable_Others(void)
{
	int i = 0;
	Disable_SPDIF_DMA();
	//Re_Init_CS4385();	
	Clear_SPDIF_DMA_Buffer();	
	//关闭AD
	AD_PowerOff();
	//关闭AES信号
	AES_PowerOff();
	//注册中断	
	//禁止AD中断，
	Disable_AD_DMA();
	for(i = 0; i < 10000; i++)
    		NOP;	
}




//对SPDIF输入的初始化
void Init_SPDIF_COAX(int i_choice)
{
	SPDIF_Disable_Others();

	switch(i_choice)
	{
	case 0://spdif
		SPDIF_COAX_SEL(1);
		//printf("---Init_SPDIF---\n");
		//clear_spdif_interrupt_index();
		Init_SPDIF_Pin();	
		
	break;
	case 1://coax
		SPDIF_COAX_SEL(0);
		//printf("---Init_SPDIF---\n");
		//clear_spdif_interrupt_index();
		Init_SPDIF_Pin();	
	break;	
	}

	AD_AES_Select(1);
	reinit_da();
	SPDIF_InitData();
}
/*
*
*
*
*/
void Register_SPDIF_Interrupt(void)
{
	interrupt(SIG_SP4, SP4ADCRcv_ISR);	
}

void Unregister_SPDIF_Interrupt(void)
{
	interrupt(SIG_SP4, SIG_IGN);	
}








