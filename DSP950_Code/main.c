/*****************************************************************************
 * main.c
 *****************************************************************************/
#include <stdio.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sysreg.h>
#include "math.h"
#include "./include/spi.h"
#include "./include/cs4385a.h"
#include "./include/input_types.h"
#include "time.h"
#include "./include/cs5368.h"
#include "./include/spi_code.h"
#include "./include/pcm4104.h"
#include "sru.h"
#include "./include/timer.h"
#include "./include/channel_to_da_port.h"
#include "./include/volume.h" 
#include "./include/eq.h"



 
/****************************************************************
*				2016-12-28 ��дDAָ���ΪADָ�룬������
*					spdif��ʱ���ܱ�֤�㷨�����
*					Ҳ������������⣬������ȥ�޸�
*					ָ��,��֤���ⲻ�᳤ʱ��һֱ�С�
*****************************************************************/


//ָ�����:
/****************************************************************
*1.��Ƶ��Ӧ(����DA�����β���)
*2.��̬��Χ
*3.�����SNR
*4.
*5.
*6.
*7.			
****************************************************************/
//�ź�����˳��:
/****************************************************************
*1.AD
*2.AES
*3.RCA  
*4.MIC
*5.SPDIF			
****************************************************************/

/****************************************************************
*		SPORTռ�����
*-----------------------------------------------
*	DAռ��4��SPORT��,���8��ͨ��������
*-----------------------------------------------
*	PCGB:
*	SPORT0:CS4385.----2016-11-25��ΪPCM4104
*	SPORT1:PCM4104
*   SPORT2:PCM4104
*	SPORT3:PCM4104 
*------------------------------------------------
*
*------------------------------------------------
*   PCGA:
*	SPORT4:SPDIF COAX
*	
*	SPORT5:MIC CS5368 ��AD����
*	SPORT6:RCA 
*
*	SPORT7:
*****************************************************************/

/*****************************************************************
*
*		�������̵��߼�:   2016-11-24
*			         ��������      matrix      ��Ƶ��  Out[0]
*		InputXType[]----------X[m]--------Y[O]---------Out[1] 
*                                                      ...
*                                                      Out[n] 
*******************************************************************/

/*
*			Ҫ��Ϊ����ģʽ��һ��������ģʽ��һ����У׼ģʽ			
*			
*			У׼ģʽ������Ҫ�Լ������źŵģ��������ǲ����������ȡ�
*/

//////////////////////////////////ϸ�ڴ�����//////////////////////////////////////////
/*
*		1.�����Ͱ���ģʽ�л��Ĵ���: 2016-12-01
*
*		����:
*			ֻ��Ҫ���ź��𽥼�����һֱ��0���������������0һֱ����󼴿�
*
*		
*		�������밴���л��Ĵ���:
*			�л�������ʱ����Ҫ����һ��������״̬����һ��ʱ�䣬Ȼ����ȥ�𽥼�С
*			������һֱ��С��0��Ȼ��mute���źţ�֮��sru���������֮����ȥunmute
*			�źţ����žͿ����𽥽��ź���ǿ��
*
*/
/***
*		2.У׼������ֲ 
***/


//�������Ͷ���
int g_spi_data = 0;
extern int g_SPI_Interrupt_Flag;

section("seg_ext_dmda")  float outputmultichannelpcmdata[FFTSIZE * 24 ];

int g_input_type =INPUT_TYPE_AD;

time_t start_time,stop_time;
double time_used=0;
float xx[1024];    
float yy[1024]; 
#define SDRAM_START  0x0200000	// start address of SDRAM
#define SDRAM_SIZE	 0x00800000	// size of SDRAM in 32-bit words.  (i.e. 4M x 32)
#define SDMODIFY  (BIT_17)
  
int TEST_SDRAM(void)
{
	volatile unsigned int *pDst;
	int nIndex = 0;
	int bError = 1; 	// returning 1 indicates a pass, anything else is a fail
	int n;
	// write incrementing values to each SDRAM location
	for(nIndex = 0, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++, nIndex++ )
	{
		*pDst = nIndex;
		//xx[nIndex%1000]=nIndex;
	}
	// verify incrementing values
/*	for(nIndex = 0, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++, nIndex++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}*/
#if 1
	for(nIndex = 0x12345678, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	for(nIndex = 0x12345678, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	for(nIndex = 0x87654321, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	for(nIndex = 0x87654321, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	for(nIndex = 0x01234567, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	for(nIndex = 0x01234567, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	for(nIndex = 0x76543210, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	for(nIndex = 0x76543210, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}
	
	// butterfly test.  Write incrementing values to beginging and end of SDRAM
	volatile unsigned int *pStart = (volatile unsigned int *)SDRAM_START;
	volatile unsigned int *pEnd = (volatile unsigned int *)(SDRAM_START + SDRAM_SIZE)-1;
	
	for(nIndex = 0; nIndex < (SDRAM_SIZE/2); nIndex++ )
	{
		*pStart = nIndex;
		*pEnd = (SDRAM_SIZE - nIndex);
		
		pStart++;
		pEnd--;
	}

	// verify butterfly test values
	pStart = (volatile unsigned int *)SDRAM_START;
	pEnd = (volatile unsigned int *)(SDRAM_START + SDRAM_SIZE)-1;

	for(nIndex = 0; nIndex < (SDRAM_SIZE/2); nIndex++ )
	{
		if( nIndex != *pStart )
		{
			bError = 0;
			break;
		}
		
		if( (SDRAM_SIZE - nIndex) != *pEnd )
		{
			bError = 0;
			break;
		}

		pStart++;
		pEnd--;
	}	
#endif
	return bError;
}




void TestFun()
{
	int randx;
	int i = 0;
	
#if 0	
	for(i = 0 ; i < 10 ; i ++)
	{
		printf("%d ",rand());
	}
	printf("\n");
	
	for(i = 0 ; i < 10000000 ; i ++);
	volatile clock_t i_start_time = clock();
	volatile clock_t i_end_time = clock();
	printf("time eclapse : %e\n",((double)(i_end_time- i_start_time))/CLOCKS_PER_SEC);
	printf("CLOCKS_PER_SEC = %d\n",CLOCKS_PER_SEC);
#endif

#if 0
	double y = log10(10.0);
	printf("log10(10) = %f\n",y);
#endif

#if 0 //�˴���������ȫ����DA
	PCM_TEST_DATA();
	//��ʼ��PCM4104оƬ
	PCM4104_Init();
	//printf("---------end--------------\n");
	while(1);
#endif

#if 1//�˴�����AD��DA
	//PCM_TEST_DATA();
	int EQflag = 0;

	
	//SPI_Init( );
	TIMER0();
	outpcmDAdatabuf_link();
	{
		int i = 0;
		//��ʼ���м��������
	    for(i = 0; i < 8; i++)//��16������
	    {
	        p_multichanneloutputbuf[i] = &RxBlock_I2S_CS5368[i/2][i%2];//&outputmultichannelpcmdata[i * FFTSIZE];	
	        //ȡ���������������ݷ���Ϊ:
	        /*
	        *	p_multichanneloutputbuf[i][2*j];  j��СΪFFTSIZE/2	
	        */       
	    }	
	}	
	
	//��ʼ��PCM4104оƬ
	PCM4104_Init();
	//��ʼ��AD����	
	AD_AES_Init(0);
	
//	printf("----����-----ad--------------\n");
	while(1)
	{
		//handle_spi_code();
		handle_ad_aes_data();     
	}
#endif


#if 0 //�˴�����AES��DA
	//CS4385A_Init();
	//SPI_Init( );	
	//TIMER0();
	//Volume_Init();
	outpcmDAdatabuf_link();
	{
		int i = 0;
		//��ʼ���м��������
	    for(i = 0; i < 8; i++)//��16������
	    {
	        p_multichanneloutputbuf[i] = &RxBlock_I2S_CS5368[i/2][i%2];//&outputmultichannelpcmdata[i * FFTSIZE];	
	        //ȡ���������������ݷ���Ϊ:       
	    }	
	}
	//PCM_TEST_DATA();
	//outpcmDAdatabuf_link();
	//��ʼ��PCM4104оƬ
	PCM4104_Init();
	//��ʼ��AD����	
	AD_AES_Init(1);
	
	//printf("---------aes--------------\n");
	while(1)
	{
		//handle_spi_code();
		handle_ad_aes_data();	
	}
#endif

#if 0 //�˴�����RCA��DA
	//PCM_TEST_DATA();
	//��ʼ��PCM4104оƬ
	PCM4104_Init();
	//��ʼ��AD����	
	RCA_Init();
	printf("---------rca--------------\n");
	while(1)
	{
		handle_ad_aes_data();	
	}
#endif


#if 0	
	//����spdif��DA
	//��ʼ��PCM4104оƬ
	outpcmDAdatabuf_link();
	//��ΪSPDIF����
    inputSPDIFdatabuf_link();
	
	PCM4104_Init();	
	//��ʼ������spdif	
	Init_SPDIF_COAX(0);
	
//	printf("---------spdif--------------\n");
	while(1)
	{
		//handle_spdif_test_data();
		
		
	}
#endif

#if 0
	int bPassed = 0;
	time_t start_time_set[20];
	time_t stop_time_set[20];
	float time_used_set[20];
	for(i=0;i<5;i++)
	{
		start_time =clock();
		start_time_set[i] =start_time;
		bPassed = TEST_SDRAM();
		stop_time=clock();
		stop_time_set[i] =stop_time;
        //�����ʱ
        time_used_set[i]=((unsigned int)(stop_time-start_time))/(CLOCKS_PER_SEC/1000);
        time_used+=time_used_set[i];
        // printf ("%d:",bPassed);
	}
       printf ("time_used #: %f ms,	%d\n", time_used,CLOCKS_PER_SEC/1000);
	
	while(1){
			
	}
#endif
	

#if 0//AES͸��
	TIMER0();
//	int i = 0;
	for(i = 0 ; i< 10000 ; i++);
	//printf("---aes talkthrough-----\n");
	
//	LED_FLASH_P13(0);

	AES_Reset();
	//printf("---aes talkthrough-----\n");
	AD_AES_Select(0);
	AD_MIC_Select(0);
	AD_AES_Pin_Init(1);	
	//printf("---aes talkthrough-----\n");

	SRU(HIGH,PBEN20_I);			
	SRU(DAI_PB17_O,	DAI_PB20_I);

	SRU(HIGH,PBEN10_I);	
	SRU(DAI_PB16_O,	DAI_PB10_I);

	//DA-DATA01-----DAI.13	 	
	SRU(HIGH,PBEN13_I);
	SRU(DAI_PB18_O,DAI_PB13_I);
	
	//DA-DATA02-----DAI.07	 
	SRU(HIGH,PBEN07_I);			
	SRU(DAI_PB18_O,DAI_PB07_I);

	//DA-DATA03-----DAI.01		
	SRU(HIGH,PBEN01_I);
	SRU(DAI_PB18_O,DAI_PB01_I);		
		
	//DA-DATA04-----DAI.19	
	SRU(HIGH,PBEN19_I);
	SRU(DAI_PB18_O,DAI_PB19_I);
	
	//DA-DATA05-----DAI.06	
	SRU(HIGH,PBEN06_I);
	SRU(DAI_PB18_O,DAI_PB06_I);
		
	//DA-DATA06-----DAI.02	
	SRU(HIGH,PBEN02_I);
	SRU(DAI_PB18_O,DAI_PB02_I);
	
	//DA-DATA07-----DAI.09	
	SRU(HIGH,PBEN09_I);
	SRU(DAI_PB18_O,DAI_PB09_I);
	
	//DA-DATA08-----DAI.05
	SRU(HIGH,PBEN05_I);
	SRU(DAI_PB18_O,DAI_PB05_I);

	PCM4104_Reset();
	//unmute
	PCM4104_Mute(0);	
	while(1)
	{
	//	printf("------while------\n");	
	}

#endif

}


/*
*			
*				yuan 2016-10-09
*				edit yuan 2016-12-07
*
*	Ϊ�˲���ָ�꣬����ʹ��ֱͨ��ʽ������ǽ�������Ӳ���������ʹ�á�
*/
extern int i_now_input;

int main( void )
{
   	//int i_spi_code;
   	int i ,j;
	int i_system_change = SYSTEMSTATE_NORMAL;
	int i_system_change_flag = 0;
	
	
	
   	int EQflag = 0;//EQ���õı�־
   	initPLL_SDRAM();// Initiali////////se the PLL and SDRAM controller
	clearDAIpins();	
	//printf("----app run----\n");
	
	//printf("CLOCKS_PER_SEC = %d\n",CLOCKS_PER_SEC);  
    
    #if 0
    TestFun();
    #endif
     
	SPI_Init( );	
	TIMER0();
	Volume_Init();	
	
	//AC3Decoder_init();
	
	//��EQ�ͷ�Ƶ�˲�������Ӧ��������ʼ������
	sim_31band_EQ();
	zero_filterstate();
	init_divisionfilter();

	//��ʼ��PCM4104оƬ
	PCM4104_Init();
	
	SpeakerNumber = 14;//DSP950��8+6��ʽ�ġ�
	knobvolume = 50.0;

	//����ӿھ���,������λ����ȥ������
	for(i = 0; i < 16; i++)
		for(j = 0; j < 16; j++)
			Matrix_ChtoOutch[i][j] = (i == j)? 1: 0;	
	
	//�������״̬		
    MonitorOutState = 1;
    //������
    Mute = 1;
    FadeInFlag = 0;
    FadeInCoeff = 1;	//����ϵ��Ϊ1
    FadeOutFlag = 0;
    FadeOutCoeff = 0;	//����ϵ��Ϊ0
	//���뵭��ʱ��
	FadeInTime = 0.5;
	FadeOutTime = 0.5;
    
    //��ʼ�������2016-09-29 
    FadeInTimePerFrame  = 0.001;
    FadeOutTimePerFrame = 0.001;  
      
    for(i = 0; i < 8; i++)
    {
        inputparam[i].SamplingRate = 48000;
        inputparam[i].PlaybackSF = 0;
    }

    for(i = 1; i < 5; i++)
    {
    	for(j = 0; j < 16; j++)
    	{
        	outputparam[i].FreqDivVal[j][0] = 1;
        	outputparam[i].FreqDivVal[j][1] = 20;
        	outputparam[i].FreqDivVal[j][2] = 20000;
    	}
        for(j = 0; j < 30; j++)
        {
            outputparam[i].ChannelGain[j] = 0;
            outputparam[i].ChannelGain_Val[j] = 1;
            if((j == 3) || (j == 9))
            {
                outputparam[i].ChannelGain_Val[j] = 1;//0.5;//0.25; //LFE˥��
            }
			if(j < 16)
			{
				outputparam[i].Delay[j] = 0;
				outputparam[i].HighCutFreq[j] = 20000;
				outputparam[i].HighCutSlope[j] = 0;
				outputparam[i].LowCutFreq[j] = 20;
				outputparam[i].LowCutSlope[j] = 1;
			}
            
            int k;
            for(k = 0; k < 31; k++)
            {
                outputparam[i].EQBandGain[j][k] = 0;
                if((j == 2) || (j == 14) || (j == 6) || (j == 7))//��Ļ�ڵ�
                	outputparam[i].EQBandGain_Val[j][k] = 1;//pow(10.0, FC[k - 1] / 20.0);
                else//���ڵ�
                	outputparam[i].EQBandGain_Val[j][k] = 1;//pow(10.0, FL[k - 1] / 20.0);
            }
        }
        //outputparam[i].OutputSamplingRate = 48000;
    }
    
    AlignmentChannel = 1;
    SignalModeVal = 0;
    SystemState = SYSTEMSTATE_NORMAL;//SYSTEMSTATE_ALIGNMENT;//
    OutputYtype = 3;//OUTPUTY3_24;//8*3��Ƶ�����24����,�տ�ʼ�����ó�Ϊ������ֵ��Ҳ����8*3(3��Ƶ),12*2(2��Ƶ),��24(������)
    DSP23_OutputYtype = OutputYtype;
//  printf("fadeinoutgain = %f\n",fadeinoutgain);

	outpcmDAdatabuf_link();
	//��ʼ���м��������,��ΪAD����
	inputADdatabuf_link();
	//��ΪSPDIF����
    inputSPDIFdatabuf_link();
    
 	//for(i = 0; i < 16; i++)
  	//	pDelayBuf[i] = DelayBuf[i];//��ʱ������
            	
	while(1)
	{
		if(g_init_done != 1)
		{
			handle_spi_code();
			continue;
		}
		
		//printf("--init end--0x%08x-\n",g_input_type);
	 
		//��ʼ��EQ��ز�����
		if(EQflag == 0)
		{ 
			sim_31band_EQ_outputparamset();//sim_31band_EQ();
			sim_frequencydivisionfilter_outputparamset(); //���÷�Ƶ��Ϣ
			EQflag = 1;
		}
		//����SPIָ����Ҵ�����Ӧ����
       	handle_spi_code();
       	
       	 
        
        #if 1
		//if(i_system_change == SYSTEMSTATE_NORMAL)
		{
		    //�����ʱ仯�ȣ�Ŀǰ�ݲ�����
		    //֮����Ҫ����֡����
		    //oneframeprocess_alignment( );	
			Init_all_input_types( );
	//		g_spi_data = INPUT_TYPE_SPDIF;//���뷽ʽΪ����
	//		g_spi_data = INPUT_TYPE_COAX;//���뷽ʽΪͬ��
	//		g_spi_data = INPUT_TYPE_AD;//���뷽ʽΪAD����
	//		g_spi_data = INPUT_TYPE_AES;//���뷽ʽΪAES����
	//		g_spi_data = INPUT_TYPE_MIC;//���뷽ʽΪMIC����
			handle_all_input_data(g_input_type);	
		}
		#else
			test_audio_mode(); 
		#endif
		/* 
		if(i_system_change != SystemState)
		{
			if(i_system_change_flag == 0)
			{
				if(i_system_change == SYSTEMSTATE_ALIGNMENT)
				{
 					 
 					i_now_input = -1;
					 
					i_system_change = SystemState;

					PCM4104_Mute(1);
					
					PCM4104_Init();
					i_system_change_flag = 1;
				}
			}

			if(i_system_change_flag == 0)
			{
				if(i_system_change == SYSTEMSTATE_NORMAL)
				{
					  
					 
					i_system_change = SystemState;
					i_system_change_flag = 1;
				}
				
			}
			i_system_change_flag = 0;
		}
		
		
		if(i_system_change == SYSTEMSTATE_ALIGNMENT)
		{
			 
			{
				static int i_pr = 0;
				if(i_pr == 0)
				{
					printf("---select test mode-----\n");
					 
					i_pr = 1;	
					AES_Disable_Others();
					SPDIF_Disable_Others();
				}
			}
			
			if(g_i_da_interrupt_flag == 1)  
			{                                            
				g_i_da_interrupt_flag = 0;
				test_audio_mode();          
			}
		}
		*/
	}	
}




