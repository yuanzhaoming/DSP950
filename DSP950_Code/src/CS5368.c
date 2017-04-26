#include "./include/cs5368.h"
#include <def21489.h>
#include <cdef21489.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>
#include "./include/spdif_coax.h"
#include "./include/input_types.h"
#include "./include/pcm4104.h"


 
#define NOP asm volatile("nop;")
//RxBlock_I2S 
//section("seg_ext_dmda") 
 int RxBlock_I2S_CS5368[MAININPUT_SPORTNUM][CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS];
int	TCB_RxBlock_I2S_CS5368[MAININPUT_SPORTNUM][CIRCLEDAMDATABUFFNUM][4];
 
static unsigned int PCI    = 0x00080000;
static unsigned int OFFSET = 0x00080000;
/*
*		AD����ʵ�ʵ�оƬ�ӿڶ�Ӧ��ϵ
*		
*		-------------------------------------
*		|    �ⲿ        |      �ڲ�        |       
*       -------------------------------------
*		|    AIN1(17,18) |      AIN8        |
*       -------------------------------------
*		|    AIN2(15,16) |      AIN7        | 
*       -------------------------------------
*		|    AIN3(13,14) |      AIN3        | 
*       -------------------------------------
*       |    AIN4(11,12) |      AIN4        |
*       -------------------------------------
*		|    AIN5( 1, 2) |      AIN2        | 
*       -------------------------------------
*       |    AIN6(47,48) |      AIN1        |
*       -------------------------------------
*		|    AIN7(45,46) |      AIN5        | 
*       -------------------------------------
*       |    AIN8(43,44) |      AIN6        |
*       -------------------------------------		
*/


/*
*
*	AES��AD�ǲ��ġ�ֻ��AES��������Դ��DIR9001оƬ
*
*	DPI_P11	AES_RESET
*	DPI_P08	AES_ERROR
*/
/*
*		CS5368  ADоƬ	
*
*		���ݿ��Ʋ���
*		------------------------------------------------------
*		|    ����        |      ������       |     ���ӵ���   |
*       ------------------------------------------------------
*		|  DAI_P17       |     I2S_IN_BCLK   |      R101      |
*       ------------------------------------------------------
*		|  DAI_P16       |     I2S_IN_LRCLK  |      R100      |
*       ------------------------------------------------------
*		|  DAI_P18       |     I2S_IN_DATA01 |      R94       |
*       ------------------------------------------------------
*       |  DAI_P04       |     I2S_IN_DATA02 |      R95       |
*       ------------------------------------------------------
*       |  DAI_P08       |     I2S_IN_DATA03 |      R96       |
*       ------------------------------------------------------
*       |  DAI_P14       |     I2S_IN_DATA04 |      R97       |
*       ------------------------------------------------------
*
*		
*		����ģʽ���Ʋ���
*    	------------------------------------------------------
*		|    ����        |      ������      |     ���ӵ���   |
*       ------------------------------------------------------
*		|  DPI_P11       |     AD_RESET     |      R117      |
*       ------------------------------------------------------
*		|  DAI_P07       |     AD_MIC_SEL   |      R102      |
*       ------------------------------------------------------
*		|  DAI_P13       |     AES_AD_SEL   |      R93       |
*       ------------------------------------------------------
*		|  DAI_P09       |     AES_RESET    |      R140      |
*       ------------------------------------------------------
*/
/*
*
*	����:   ѡ�����AD���뻹��AES���룬AD�����AES����ʱ�Ӳ����ķ�ʽ�ǲ�һ���ġ�
*
*	i_choice:
*		0:AD
*		1:AES
*/
void AD_AES_Pin_Init( int i_choice)
{	
    /**���ݲ���**/
	switch(i_choice)
	{
	case 0:
	    //ʹ��PCG��Ϊ��Ƶ���ݷ�Ƶ��
	    PCG_Init_GLOBAL(); 
		/**ʱ�Ӳ���BCLK**/
	    SRU(HIGH, PBEN17_I);
	    SRU(PCG_CLKA_O, DAI_PB17_I);  
	    SRU(PCG_CLKA_O, SPORT5_CLK_I); 
	    SRU(PCG_CLKA_O, SPORT6_CLK_I); 
	    /**֡ʱ�Ӳ���LRCLK**/		  
	    SRU(HIGH, PBEN16_I); 		  
	    SRU(PCG_FSA_O, DAI_PB16_I); 
	   	SRU(PCG_FSA_O, SPORT5_FS_I); 
	    SRU(PCG_FSA_O, SPORT6_FS_I); 
	    //������
	    //DATA01   DAI18 - 2016-11-25
    	SRU(LOW, PBEN18_I);			
   	 	SRU(DAI_PB18_O, SPORT5_DA_I);
   	 	//DATA02   DAI04 - 2016-11-25
    	SRU(LOW, PBEN04_I);			
   	 	SRU(DAI_PB04_O, SPORT5_DB_I);
   	 	//DATA03   DAI08 - 2016-11-25
     	SRU(LOW, PBEN08_I);			
   	 	SRU(DAI_PB08_O, SPORT6_DA_I);
	    //DATA04   DAI14 - 2016-11-25    
    	SRU(LOW, PBEN14_I);			
   	 	SRU(DAI_PB14_O, SPORT6_DB_I);
   	 	
	break; 
 	case 1://AESΪ���룬��ѡ��������Щ����
	 	SRU(LOW, PBEN17_I);/*BCK*/			
		SRU(LOW, PBEN16_I);/*LRCLK*/
		SRU(DAI_PB17_O, SPORT5_CLK_I);
	    SRU(DAI_PB16_O, SPORT5_FS_I); 	
	 	SRU(DAI_PB17_O, SPORT6_CLK_I);
	    SRU(DAI_PB16_O, SPORT6_FS_I); 
	    //������
	    //DATA01   DAI18 - 2016-11-25
    	SRU(LOW, PBEN18_I);			
   	 	SRU(DAI_PB18_O, SPORT5_DA_I);
   	 	//DATA02   DAI04 - 2016-11-25
    	SRU(LOW, PBEN04_I);			
   	 	SRU(DAI_PB04_O, SPORT5_DB_I);
   	 	//DATA03   DAI08 - 2016-11-25
     	SRU(LOW, PBEN08_I);			
   	 	SRU(DAI_PB08_O, SPORT6_DA_I);
	    //DATA04   DAI14 - 2016-11-25    
    	SRU(LOW, PBEN14_I);			
   	 	SRU(DAI_PB14_O, SPORT6_DB_I);		
 	break;
	}	
}


void AD_Reset(void)
{	
    int i;
    //****************************reset AD**************************
    SRU(HIGH, DPI_PBEN11_I);
    SRU(LOW, DPI_PB11_I);  //AD_RESET
    for(i = 0; i < 65536; i++)
        NOP;

    SRU(HIGH, DPI_PB11_I);  //AD_RESET
    //Wait for recommended number of cycles
    for(i = 0; i < 4096; i++)
        NOP; 			
}

void AD_PowerOff(void)
{
    int i;
    /***************************reset AD**************************/
    SRU(HIGH, DPI_PBEN11_I);
    SRU(LOW, DPI_PB11_I);  //AD_RESET
    for(i = 0; i < 65536 ; i++)
        NOP;
	
}



void AES_Reset(void)
{	
    int i;
    /****************************reset AES**************************/
    SRU(HIGH, DPI_PBEN09_I);
    SRU(LOW, DPI_PB09_I);  //AES_RESET
    for(i = 0; i < 65536 ; i++)
        NOP;

    SRU(HIGH, DPI_PB09_I);  //AES_RESET
    //Wait for recommended number of cycles
    for(i = 0; i < 4096; i++)
        NOP; 			
}

void AES_PowerOff(void)
{	
    int i;
    //****************************reset AES**************************
    SRU(HIGH, DPI_PBEN09_I);
    SRU(LOW, DPI_PB09_I);  //AES_RESET
    for(i = 0; i < 65536 ; i++)
        NOP;			
}

/*
*
*	ѡ��AD--AES�ȡ�
*		i_choice:
*			0:	AES
*			1:  AD
*		
*/
void AD_AES_Select(int i_choice)
{
	SRU(HIGH, DPI_PBEN13_I);
	
	switch(i_choice)
	{
	case 0:
		SRU(LOW, DPI_PB13_I);  //DPI13:AES			
	break;
	case 1:
		SRU(HIGH, DPI_PB13_I); //DPI13:AD		
	break;	
	}		
} 


void AD_MIC_Select(int i_choice)
{
	SRU(HIGH, DPI_PBEN07_I);
	
	switch(i_choice)
	{
	case 0:
		SRU(LOW, DPI_PB07_I);  //DPI07:AD	
	break;
	case 1:
		SRU(HIGH, DPI_PB07_I); //DPI07:MIC		
	break;	
	}		
}



void CS5368_InitData(void)
{
	int i = 0;
	int j = 0;	

	//Setting Up and Starting Chained DMA 
	//1.Clear the chain pointer register.
	*pCPSP5A = 0;
	*pCPSP5B = 0;
	*pCPSP6A = 0;
	*pCPSP6B = 0;
	
	//2.For internal memory transfers, set up all TCBs in internal memory.
	*pDIV5 = 0x00000000;//0x00000000;  // Transmitter (SPORT5)
	*pDIV6 = 0x00000000;//0x00000000;  // Transmitter (SPORT6)
	
	for(i = 0 ; i < MAININPUT_SPORTNUM ; i ++)
	{
		for(j = 0; j < CIRCLEDMADATABUFFNUM; j++)
		{
			TCB_RxBlock_I2S_CS5368[i][j][0] = (unsigned int)(TCB_RxBlock_I2S_CS5368[i][( j + 1 ) % CIRCLEDMADATABUFFNUM]) + 3 - OFFSET + PCI ;//
			TCB_RxBlock_I2S_CS5368[i][j][1] = NUM_SAMPLES * NUM_TX_SLOTS;															  //
			TCB_RxBlock_I2S_CS5368[i][j][2] = 1;																					  //
			TCB_RxBlock_I2S_CS5368[i][j][3] = (int)(&RxBlock_I2S_CS5368[i][ j * NUM_SAMPLES * NUM_RX_SLOTS ]); 								  //
			//rx_block_pointer[i][j] = (&RxBlock_I2S_CS5368[i][j * NUM_SAMPLES * NUM_RX_SLOTS]);
		}
	}
	//3.Write the address containing the index register value of the first TCB to the chain pointer register, which starts the chain.
	*pCPSP5A = (unsigned int)(TCB_RxBlock_I2S_CS5368[0][0]) - OFFSET + 3 + PCI;
	*pCPSP5B = (unsigned int)(TCB_RxBlock_I2S_CS5368[1][0]) - OFFSET + 3 + PCI;
	*pCPSP6A = (unsigned int)(TCB_RxBlock_I2S_CS5368[2][0]) - OFFSET + 3 + PCI;
	*pCPSP6B = (unsigned int)(TCB_RxBlock_I2S_CS5368[3][0]) - OFFSET + 3 + PCI;
	
	//4.Write to the SPCTLx register by setting the DMA enable bit to one and the chaining enable bit to one. Setting these bits loads the
	//DMA parameter registers.
	            
    *pSPCTL5 = (OPMODE		//I2S mode
	     | SLEN32	  		//word length = 32
	     | SPEN_A 	  		//serial port A channel enable
	     | SCHEN_A 	  		//Enable serial port channal A DMA channing
	     | SDEN_A 	  		//Enable serial port channal A DMA
	     | L_FIRST 		//Left channal first
	     | CKRE  
	     
	     | SPEN_B 	  		//serial port B channel enable
	     | SCHEN_B 	  		//Enable serial port channal B DMA channing
	     | SDEN_B	  		//Enable serial port channal B DMA       
     	//| DIFS               
   	);  
   	*pSPCTL6 = *pSPCTL5;
   	            
	for(i = 0; i < 10000; i++)
    	NOP;
	
}

int Get_Current_Dma_CS5368_Rxblock(){

	int i;
	for (i = 0; i < 4; i++){
		if (*pCPSP6A == (unsigned int)(TCB_RxBlock_I2S_CS5368[2][i]) - OFFSET + 3 + PCI)
			break;
	}
	return (i < 4) ? i : 0;
}


void Disable_AD_DMA()
{
	*pCPSP5A = 0;	
	*pCPSP5B = 0;
	*pCPSP6A = 0;
	*pCPSP6B = 0;	
}


void Unregister_AD_Interrupt( void )
{
	interrupt(SIG_SP5, SIG_IGN);	
	interrupt(SIG_SP6, SIG_IGN);
}

void clear_ad_buffer(void)
{
	int i=0,j = 0;
	for(i = 0 ; i < MAININPUT_SPORTNUM ; i ++)
	{
		for(j = 0 ; j < CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS ; j ++)
		{
			RxBlock_I2S_CS5368[i][j] = 0;	
		}
	}
}


void Register_AD_Interrupt( void )
{
	interrupt(SIG_SP6, SP6ADCRcv_ISR);
}


/*
*
*	ѡ����ʹ��AD������ʹ��AES��
*
*	ע���ڽӿ����������ֵ�ѡ��һ����������ǰ������ѡ��AD��MIC���롣���������ߵ�ѡ��
*		ѡ�����DIR9001������AES���ݻ���AD����
*
*
*	0:ѡ��AD
*	1:ѡ��AES
*/
void AD_AES_Init(int i_choice )
{
	Disable_AD_DMA();
	//�Ͽ��ж�����
	Unregister_AD_Interrupt();
	Register_AD_Interrupt();
	clear_ad_interrupt_index();
	clear_ad_buffer();
	
	switch(i_choice)
	{
		case 0://ѡ��AD����
			AD_AES_Select(1);
			AD_MIC_Select(0);
			AD_AES_Pin_Init(0);
			AD_Reset();
		break;
		case 1://ѡ��AES����
			AES_Reset();
			AD_AES_Select(0);
			AD_MIC_Select(0);
			AD_AES_Pin_Init(1);			
		break;
		default:
		break;	
	}
	//��ʼ��DA����
	//PCM4104_Pin_Init();
	reinit_da();
	//�����������������ˡ�
	CS5368_InitData();
}
/*
*	AD���뿪����ʱ����Ҫ��ֹDIR9001��SPDIF,COAX�ȵ�
*
*/
void AD_Disable_Others(void)
{
	//��ֹ������жϵ�
	//Disable_AD_DMA();
	//�Ͽ��ж�����
	Unregister_AD_Interrupt();
	//��ֹDIR9001
	AES_PowerOff();
	//��ֹSPDIF��������ֹDMA.�������������
	Disable_SPDIF_DMA();
	Clear_SPDIF_DMA_Buffer();
 
	//���DA����
	//Clear_CS4385_Buffer();
	//Clear_PCM4104_Buffer();
}


/*
*	AES���뿪����ʱ����Ҫ��ֹAD��SPDIF,COAX�ȵ�
*
*/

void AES_Disable_Others(void)
{
	static int i_ad_power_flag = 0;
	//��ֹ������жϵ�
	//Disable_AD_DMA();
	Unregister_AD_Interrupt();
	//��ֹAD
	AD_PowerOff();	
	//��ֹSPDIF��������ֹDMA.�������������
	Disable_SPDIF_DMA();
	Clear_SPDIF_DMA_Buffer();
 
	//���DA����
	//Clear_CS4385_Buffer();
	//Clear_PCM4104_Buffer();
}




