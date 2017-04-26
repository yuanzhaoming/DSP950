#include "./include/cs4385a.h"
#include <def21489.h> 
#include <cdef21489.h>
#include <math.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>

#if 0
 
#define SHIFT_NUMBER 31

#define NOP asm volatile("nop;")
//
/*
*
*		多通道(TDM)需要配置的寄存器
*
*		DIVx	
*	    SPCTLx	
*		SPCTLNx
*		SPMCTLx
*		MTxCSy,MRxCSy
*		MTxCCSy,MRxCCSy
*		SPERRCTLx
*		SPERRSTAT
*
*		see reference at : ADSP-214xx SHARC Processor Hardware Reference,Rev. 0.3  . P515.
*	
*	
*		P1148 as the TDM MODE detail.
*
*		使用SPCTLx和SPMCTLx配置SPORT运行在多通道模式。
*/

//section("seg_ext_dmda") 
int TxBlock_TDM_CS4385[2][CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS * 8/2];
int	TCB_TxBlock_TDM_CS4385[2][CIRCLEDAMDATABUFFNUM][4];

static unsigned int PCI = 0x00080000;
static unsigned int OFFSET = 0x00080000;


void clearDAIpins()
{
    //------------------------------------------------------------------------
    //  Tie the pin buffer inputs LOW for all DAI pins.  Even though
    //    these pins are inputs to the SHARC, tying unused pin buffer inputs
    //    LOW is "good coding style" to eliminate the possibility of
    //    termination artifacts internal to the IC.  Note that signal
    //    integrity is degraded only with a few specific SRU combinations.
    //    In practice, this occurs VERY rarely, and these connections are
    //    typically unnecessary.  This is GROUP D
    SRU(LOW, DAI_PB01_I);
    SRU(LOW, DAI_PB02_I);
    SRU(LOW, DAI_PB03_I);
    SRU(LOW, DAI_PB04_I);
    SRU(LOW, DAI_PB05_I);
    SRU(LOW, DAI_PB06_I);
    SRU(LOW, DAI_PB07_I);
    SRU(LOW, DAI_PB08_I);
    SRU(LOW, DAI_PB09_I);
    SRU(LOW, DAI_PB10_I);
    SRU(LOW, DAI_PB11_I);
    SRU(LOW, DAI_PB12_I);
    SRU(LOW, DAI_PB13_I);
    SRU(LOW, DAI_PB14_I);
    SRU(LOW, DAI_PB15_I);
    SRU(LOW, DAI_PB16_I);
    SRU(LOW, DAI_PB17_I);
    SRU(LOW, DAI_PB18_I);
    SRU(LOW, DAI_PB19_I);
    SRU(LOW, DAI_PB20_I);

    //------------------------------------------------------------------------
    //  Tie the pin buffer enable inputs LOW for all DAI pins so
    //  that they are always input pins.  This is GROUP F.
    SRU(LOW, PBEN01_I);
    SRU(LOW, PBEN02_I);
    SRU(LOW, PBEN03_I);
    SRU(LOW, PBEN04_I);
    SRU(LOW, PBEN05_I);
    SRU(LOW, PBEN06_I);
    SRU(LOW, PBEN07_I);
    SRU(LOW, PBEN08_I);
    SRU(LOW, PBEN09_I);
    SRU(LOW, PBEN10_I);
    SRU(LOW, PBEN11_I);
    SRU(LOW, PBEN12_I);
    SRU(LOW, PBEN13_I);
    SRU(LOW, PBEN14_I);
    SRU(LOW, PBEN15_I);
    SRU(LOW, PBEN16_I);
    SRU(LOW, PBEN17_I);
    SRU(LOW, PBEN18_I);
    SRU(LOW, PBEN19_I);
    SRU(LOW, PBEN20_I);
}




/*
*
*	2016-10-08 by yuan at qj
*
*	DA芯片的初始化等
*
*	DAI_P07	TDM_DA_BCLK1
*	DAI_P19	TDM_DA_LRCLK1
*	DAI_P13	TDM_DA_DATA01
*	DAI_P01	TDM_DA_DATA02
*
*	使用TDM的配置方式
*/
void CS4385A_Pin_Init(void)
{
	SRU(HIGH,PBEN07_I);			
	SRU(PCG_CLKB_O,	DAI_PB07_I);
	SRU(PCG_CLKB_O, SPORT0_CLK_I); 

	//DA-LRCLK----DAI.19	 								
	SRU(HIGH,PBEN19_I);			
	SRU(PCG_FSB_O, SPORT0_FS_I);
	SRU(PCG_FSB_O, DAI_PB19_I);		

	//DA-DATA01-----DAI.01	 	
	SRU(HIGH,PBEN01_I);
	SRU(SPORT0_DB_O,DAI_PB01_I);
	
	//DA-DATA02-----DAI.13	 
	SRU(HIGH,PBEN13_I);			
	SRU(SPORT0_DA_O,DAI_PB13_I);			
}

void CS4385A_Reset(void)
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
*	DA_Mute管脚	: DPI_P08
*
*
*/
void CS4385A_Mute(int i_choice)
{
    int i;

    //****************************reset DA**************************
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




static void Init_PCG(void )
{
	int i = 0;
	static int Init_PCG_FLAG = 0;	
	
	if(Init_PCG_FLAG == 1)
		return ;

	Init_PCG_FLAG = 1;

    //PCG的时钟输入
  	SRU(LOW,PBEN03_I); 											//PCG DAI_P03  （PCK clk input）
 	SRU(DAI_PB03_O,PCG_EXTB_I);									//PCG clock in
 	*pPCG_CTLB1 = (2) | CLKBSOURCE | FSBSOURCE | ((2/2)<<20);	//Division for clock
	*pPCG_CTLB0 = 512 | ENCLKB | ENFSB; 						// Frame syn Division
	*pPCG_PW = (2 << 16);
}


void CS4385A_InitData(void)
{
	int i = 0;
	int j = 0;	
	//Setting Up and Starting Chained DMA 
	//1.Clear the chain pointer register.
	*pCPSP0A = 0;
	//默认处理
   	//as the reference say
   	//1.Clear all control registers
	*pSPCTL0  = 0;
    *pSPMCTL0 = 0;
    *pSP0CS0  = 0;
    *pSP1CS0  = 0;
    //2.configure the channel section register
    *pSP0CS0 = 0xff;//TX//8通道
    *pSP1CS0 = 0;   //RX
    
  	*pMT0CCS0 = 0;
  	//*pMT0CS0 = 0xff;
	//*pDIV0 = 0x008F0002;//作为内部时钟分频使用
    
    //3.configure the dma parameter registers.index,modify,count etc..
    
    //printf("---use tdm configuration----\n");
    
    for(i = 0 ; i < 2 ; i ++)
    {
		for(j = 0; j < CIRCLEDMADATABUFFNUM; j++){
			TCB_TxBlock_TDM_CS4385[i][j][0] = (unsigned int)(TCB_TxBlock_TDM_CS4385[i][( j + 1 ) % CIRCLEDMADATABUFFNUM]) + 3 - OFFSET + PCI ;
			TCB_TxBlock_TDM_CS4385[i][j][1] = NUM_SAMPLES * NUM_TX_SLOTS * 8/2;														 
			TCB_TxBlock_TDM_CS4385[i][j][2] = 1;																			  
			TCB_TxBlock_TDM_CS4385[i][j][3] = (int)(&TxBlock_TDM_CS4385[i][j * NUM_SAMPLES * NUM_TX_SLOTS * 8 /2 ]);	 								  
		}
    }
	
	//3.Write the address containing the index register value of the first TCB to the chain pointer register, which starts the chain.
	*pCPSP0A = (unsigned int)(TCB_TxBlock_TDM_CS4385[0][0]) - OFFSET + 3 + PCI;
	*pCPSP0B = (unsigned int)(TCB_TxBlock_TDM_CS4385[1][0]) - OFFSET + 3 + PCI;
	//4.configure the sport control register ,SPCTLx and enable the dma chaining.
	//printf("-----use tdm configuration-----------\n");
	
	//reference P482.Multichannel Mode
	*pSPCTL0 = ( 
				SPTRAN				//SPTRAN=1 transfer
				//| OPMODE			//I2S mode 
	            | SLEN32 			//word length = 32
	           // | SPEN_A 			//serial port A channel enable
	            | SCHEN_A 			//Enable serial port channal A DMA channing
	            | SDEN_A 			//Enable serial port channal A DMA
	            | L_FIRST 		//Left channal first
	            //| CKRE			//上升沿采样
	            //| DIFS
				//| BHD
				| SCHEN_B 			//Enable serial port channal B DMA channing
	            | SDEN_B 			//Enable serial port channal B DMA
	            );
	//5.configure the receiver SPORT pair SPCTLy and enable the dma chaining.	
	//6.in multiple mode, operation starts as soon as the MCEx bit is enabled.          				
	//7.configure and enable multichannel in the multichannel control registers.
	*pSPCTLN0 |= FSED;
	
    while(1)
    {
        if((*pSPCTL0 & (DXS0_A | DXS0_B)))
            break;
    }      
	*pSPMCTL0 = (NCH7);
	*pSPMCTL0 |= MCEA;					 	
	
	for(i = 0; i < 10000; i++)
    	NOP;
}

void Clear_CS4385_Buffer(void)
{
	memset(TxBlock_TDM_CS4385[0],0,CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS * 8/2 * sizeof(int));	
	memset(TxBlock_TDM_CS4385[1],0,CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS * 8/2 * sizeof(int));
}
	
void Stop_CS4385_DMA(){

	*pCPSP0A = 0;
	*pCPSP0B = 0;
}

void dai_isr(int sig){

	int temp;
	temp = *pDAI_IRPTL_H;
}

section("seg_ext_dmda") int g_temp[NUM_SAMPLES * NUM_TX_SLOTS * CIRCLEDMADATABUFFNUM];

void ReadFile( void )
{
	FILE* file_p;
	int  i;	
	
//	return;	
//	printf("write file\n");
//	return;


	//printf("\n");
	file_p = fopen("../a.wav","rb");
		
	if(file_p == NULL)
	{
		printf("file open bad\n");
	}
	else
	{
		printf("file open ok\n");		
	}
	
	printf("----------read file start------");

	//写文件
	fread(g_temp,1,NUM_SAMPLES * NUM_TX_SLOTS * CIRCLEDMADATABUFFNUM ,file_p);	
	//fwrite(DIR9001_RxBlock_TDM_Temp,1,NUM_SAMPLES *NUM_RX_SLOTS,file_p);

	for(i = 0 ; i < NUM_SAMPLES * NUM_TX_SLOTS * CIRCLEDMADATABUFFNUM ;i ++)
	{
		TxBlock_TDM_CS4385[0][8*i+0] = g_temp[i] << 15 ;	
		TxBlock_TDM_CS4385[0][8*i+1] = g_temp[i] << 15 ;
		TxBlock_TDM_CS4385[0][8*i+2] = g_temp[i] << 15 ;	
		TxBlock_TDM_CS4385[0][8*i+3] = g_temp[i] << 15 ;	
		TxBlock_TDM_CS4385[0][8*i+4] = g_temp[i] << 15 ;
		TxBlock_TDM_CS4385[0][8*i+5] = g_temp[i] << 15 ;	
		TxBlock_TDM_CS4385[0][8*i+6] = g_temp[i] << 15 ;
		TxBlock_TDM_CS4385[0][8*i+7] = g_temp[i] << 15 ;		
	}	
		
	fclose(file_p);	
 	
	printf("----------read file end-------\n");		
}


#define DA_DIVIDER_VALUE (1)

void WaveFun2()
{
	int i = 0;
	
	int g_sinBaseValue = 100;
//	ReadFile();	
}


int Get_Current_CS4385_Dma_Txblock(){

	int i;
	for (i = 0; i < 4; i++){
		if (*pCPSP0A == (unsigned int)(TCB_TxBlock_TDM_CS4385[0][i]) - OFFSET + 3 + PCI)
			break;
	}
	return (i < 4) ? i : 0;
}



void Register_DA_Interrupt( void )
{
	interrupt(SIG_SP0, SP0ADCSend_ISR);	
}


void Re_Init_CS4385()
{
	Stop_CS4385_DMA();
	
	CS4385A_InitData();	
	//开启
	//CS4385A_Mute(0);
	//注册DA中断
	Register_DA_Interrupt();		
}


/*
*
*
*	CS4385A初始化
*
*	为对外接口
*
*/
void CS4385A_Init(void)
{
	//PCG初始化
	Init_PCG();
	//引脚初始化
	CS4385A_Pin_Init(  );
	//CS4385A初始化
	CS4385A_Reset();
	
	CS4385A_InitData();	
	//开启
	//CS4385A_Mute(0);
	//注册DA中断
	Register_DA_Interrupt();		
}

#endif



