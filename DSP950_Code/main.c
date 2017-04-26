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
*				2016-12-28 将写DA指针改为AD指针，这样用
*					spdif的时候，能保证算法能完成
*					也就是如果出问题，可以再去修改
*					指针,保证问题不会长时间一直有。
*****************************************************************/


//指标测试:
/****************************************************************
*1.幅频响应(单独DA出波形测试)
*2.动态范围
*3.信噪比SNR
*4.
*5.
*6.
*7.			
****************************************************************/
//信号输入顺序:
/****************************************************************
*1.AD
*2.AES
*3.RCA  
*4.MIC
*5.SPDIF			
****************************************************************/

/****************************************************************
*		SPORT占用情况
*-----------------------------------------------
*	DA占用4个SPORT口,完成8个通道的数据
*-----------------------------------------------
*	PCGB:
*	SPORT0:CS4385.----2016-11-25改为PCM4104
*	SPORT1:PCM4104
*   SPORT2:PCM4104
*	SPORT3:PCM4104 
*------------------------------------------------
*
*------------------------------------------------
*   PCGA:
*	SPORT4:SPDIF COAX
*	
*	SPORT5:MIC CS5368 等AD部分
*	SPORT6:RCA 
*
*	SPORT7:
*****************************************************************/

/*****************************************************************
*
*		整个过程的逻辑:   2016-11-24
*			         声场处理      matrix      分频等  Out[0]
*		InputXType[]----------X[m]--------Y[O]---------Out[1] 
*                                                      ...
*                                                      Out[n] 
*******************************************************************/

/*
*			要分为两种模式，一种是正常模式，一种是校准模式			
*			
*			校准模式，是需要自己产生信号的，如粉燥，三角波，白噪声等。
*/

//////////////////////////////////细节处理部分//////////////////////////////////////////
/*
*		1.静音和按键模式切换的处理: 2016-12-01
*
*		静音:
*			只需要将信号逐渐减弱，一直到0，而不静音，则从0一直到最大即可
*
*		
*		不静音与按键切换的处理:
*			切换按键的时候，需要将上一个按键的状态不变一段时间，然后再去逐渐减小
*			音量，一直减小到0，然后mute掉信号，之后将sru等配置完成之后，再去unmute
*			信号，接着就可以逐渐将信号增强。
*
*/
/***
*		2.校准程序移植 
***/


//输入类型定义
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

#if 0 //此处单独测试全部的DA
	PCM_TEST_DATA();
	//初始化PCM4104芯片
	PCM4104_Init();
	//printf("---------end--------------\n");
	while(1);
#endif

#if 1//此处测试AD到DA
	//PCM_TEST_DATA();
	int EQflag = 0;

	
	//SPI_Init( );
	TIMER0();
	outpcmDAdatabuf_link();
	{
		int i = 0;
		//初始化中间输出变量
	    for(i = 0; i < 8; i++)//有16个声道
	    {
	        p_multichanneloutputbuf[i] = &RxBlock_I2S_CS5368[i/2][i%2];//&outputmultichannelpcmdata[i * FFTSIZE];	
	        //取出左右声道的数据方法为:
	        /*
	        *	p_multichanneloutputbuf[i][2*j];  j大小为FFTSIZE/2	
	        */       
	    }	
	}	
	
	//初始化PCM4104芯片
	PCM4104_Init();
	//初始化AD部分	
	AD_AES_Init(0);
	
//	printf("----整个-----ad--------------\n");
	while(1)
	{
		//handle_spi_code();
		handle_ad_aes_data();     
	}
#endif


#if 0 //此处测试AES到DA
	//CS4385A_Init();
	//SPI_Init( );	
	//TIMER0();
	//Volume_Init();
	outpcmDAdatabuf_link();
	{
		int i = 0;
		//初始化中间输出变量
	    for(i = 0; i < 8; i++)//有16个声道
	    {
	        p_multichanneloutputbuf[i] = &RxBlock_I2S_CS5368[i/2][i%2];//&outputmultichannelpcmdata[i * FFTSIZE];	
	        //取出左右声道的数据方法为:       
	    }	
	}
	//PCM_TEST_DATA();
	//outpcmDAdatabuf_link();
	//初始化PCM4104芯片
	PCM4104_Init();
	//初始化AD部分	
	AD_AES_Init(1);
	
	//printf("---------aes--------------\n");
	while(1)
	{
		//handle_spi_code();
		handle_ad_aes_data();	
	}
#endif

#if 0 //此处测试RCA到DA
	//PCM_TEST_DATA();
	//初始化PCM4104芯片
	PCM4104_Init();
	//初始化AD部分	
	RCA_Init();
	printf("---------rca--------------\n");
	while(1)
	{
		handle_ad_aes_data();	
	}
#endif


#if 0	
	//测试spdif到DA
	//初始化PCM4104芯片
	outpcmDAdatabuf_link();
	//此为SPDIF部分
    inputSPDIFdatabuf_link();
	
	PCM4104_Init();	
	//初始化光纤spdif	
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
        //计算耗时
        time_used_set[i]=((unsigned int)(stop_time-start_time))/(CLOCKS_PER_SEC/1000);
        time_used+=time_used_set[i];
        // printf ("%d:",bPassed);
	}
       printf ("time_used #: %f ms,	%d\n", time_used,CLOCKS_PER_SEC/1000);
	
	while(1){
			
	}
#endif
	

#if 0//AES透传
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
*	为了测试指标，不能使用直通方式，如果是仅仅测试硬件，则可以使用。
*/
extern int i_now_input;

int main( void )
{
   	//int i_spi_code;
   	int i ,j;
	int i_system_change = SYSTEMSTATE_NORMAL;
	int i_system_change_flag = 0;
	
	
	
   	int EQflag = 0;//EQ设置的标志
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
	
	//对EQ和分频滤波器的相应参数做初始化设置
	sim_31band_EQ();
	zero_filterstate();
	init_divisionfilter();

	//初始化PCM4104芯片
	PCM4104_Init();
	
	SpeakerNumber = 14;//DSP950是8+6格式的。
	knobvolume = 50.0;

	//输出接口矩阵,按照上位机的去做配置
	for(i = 0; i < 16; i++)
		for(j = 0; j < 16; j++)
			Matrix_ChtoOutch[i][j] = (i == j)? 1: 0;	
	
	//监听输出状态		
    MonitorOutState = 1;
    //不静音
    Mute = 1;
    FadeInFlag = 0;
    FadeInCoeff = 1;	//淡入系数为1
    FadeOutFlag = 0;
    FadeOutCoeff = 0;	//淡出系数为0
	//淡入淡出时间
	FadeInTime = 0.5;
	FadeOutTime = 0.5;
    
    //初始化中添加2016-09-29 
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
                outputparam[i].ChannelGain_Val[j] = 1;//0.5;//0.25; //LFE衰减
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
                if((j == 2) || (j == 14) || (j == 6) || (j == 7))//屏幕遮挡
                	outputparam[i].EQBandGain_Val[j][k] = 1;//pow(10.0, FC[k - 1] / 20.0);
                else//无遮挡
                	outputparam[i].EQBandGain_Val[j][k] = 1;//pow(10.0, FL[k - 1] / 20.0);
            }
        }
        //outputparam[i].OutputSamplingRate = 48000;
    }
    
    AlignmentChannel = 1;
    SignalModeVal = 0;
    SystemState = SYSTEMSTATE_NORMAL;//SYSTEMSTATE_ALIGNMENT;//
    OutputYtype = 3;//OUTPUTY3_24;//8*3分频，组成24声道,刚开始被设置成为了三个值，也就是8*3(3分频),12*2(2分频),和24(单声道)
    DSP23_OutputYtype = OutputYtype;
//  printf("fadeinoutgain = %f\n",fadeinoutgain);

	outpcmDAdatabuf_link();
	//初始化中间输出变量,此为AD部分
	inputADdatabuf_link();
	//此为SPDIF部分
    inputSPDIFdatabuf_link();
    
 	//for(i = 0; i < 16; i++)
  	//	pDelayBuf[i] = DelayBuf[i];//延时缓冲区
            	
	while(1)
	{
		if(g_init_done != 1)
		{
			handle_spi_code();
			continue;
		}
		
		//printf("--init end--0x%08x-\n",g_input_type);
	 
		//初始化EQ相关参数等
		if(EQflag == 0)
		{ 
			sim_31band_EQ_outputparamset();//sim_31band_EQ();
			sim_frequencydivisionfilter_outputparamset(); //设置分频信息
			EQflag = 1;
		}
		//接受SPI指令，并且处理相应内容
       	handle_spi_code();
       	
       	 
        
        #if 1
		//if(i_system_change == SYSTEMSTATE_NORMAL)
		{
		    //采样率变化等，目前暂不考虑
		    //之后，需要处理帧数据
		    //oneframeprocess_alignment( );	
			Init_all_input_types( );
	//		g_spi_data = INPUT_TYPE_SPDIF;//输入方式为光纤
	//		g_spi_data = INPUT_TYPE_COAX;//输入方式为同轴
	//		g_spi_data = INPUT_TYPE_AD;//输入方式为AD输入
	//		g_spi_data = INPUT_TYPE_AES;//输入方式为AES输入
	//		g_spi_data = INPUT_TYPE_MIC;//输入方式为MIC输入
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




