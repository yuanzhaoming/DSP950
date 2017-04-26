#include "./include/Input_Types.h"
#include <stdio.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sysreg.h>
#include "math.h"
#include "./include/spi.h"
#include "./include/cs5368.h"
#include "./include/mic.h"
#include "./include/PCM4104.h"
#include "./include/channel_To_DA_Port.h"
#include "./include/spi_code.h"
#include "./include/cs5368.h"
#include "./include/spdif_coax.h"
#include "./include/rca.h"
#include "./include/eq.h"
#include "./include/dsp_signal.h"
#include <time.h>

#define USE_FADE_IN_OUT 1

float V0[16] = { 0 };

int	g_RxBlock_I2S_CS5368_Temp[MAININPUT_SPORTNUM][NUM_SAMPLES*NUM_TX_SLOTS];	
//extern int	TxBlock_TDM_CS4385[2][CIRCLEDAMDATABUFFNUM*NUM_SAMPLES*NUM_TX_SLOTS * 8/2];
extern int g_interruptFlag;
extern int g_spi_data;
section("seg_dmdaq")  float inputmultichannelfftdata[(FFTSIZE << 1)+(FFTSIZE << 1)+(FFTSIZE << 1)];
//音频与上位机进行映射的相应数组
int *p_multichanneloutputbuf[16]; 			//与上位机对s应的数组
int *p_multichannel_input_spdif_buf[16]; 	//与上位机对应的数组
float *p_divisionoutbuf;

extern int g_input_type;

#define USE_INPUT_TYPE_PRINT 0

#if USE_INPUT_TYPE_PRINT
	#define INPUT_PRINT(fmt,...)  printf("[%s][%d]"fmt,__FILE__,__LINE__,##__VA_ARGS__) 
#else
	#define INPUT_PRINT(fmt,...)   
#endif


typedef struct{
	//该参数主要用来处理变化之后，每一个样本点需要累加的数
	float g_audio_coef_for_change;
	float g_audio_coef_step;	
	//表示需要执行淡入淡出操作
	int need_audio_chang_flag;
}t_audio_in_out_effect;


t_audio_in_out_effect T_t_audio_in_out_effect[16];

int g_i_change_down_flag =  0;
int g_i_change_up_flag =  0;
//中断次数，通过该变量来获取多长时间去执行切换操作	
int g_interrupt_times = 0;
/*
*		2016-11-29 外设AD接口与内部命名不是相同的
*
*		我从左往右数:
*			信号顺序为:1khz 2khz 3khz 4khz 5khz 6khz 7khz 8khz
*
*		实际顺序为:
*			3khz---->AIN1
*			4khz---->AIN2
*			6khz---->AIN3
*			5khz---->AIN4
*			2khz---->AIN5	
*			1khz---->AIN6
*			7khz---->AIN7
*			8khz---->AIN8
*	
*/
//将输入的接口顺序与AD芯片内部顺序对接
//接口为顺时针方向
int g_i_ad_in[8] = {7,6,2,3,1,0,4,5};
int g_i_da_out[16] = {3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12};//最后两路是给监听的
/*
*
*	概述:
*		为总的处理各种输入方式的处理函数，注意要有互斥性，选择一个输入方式后，其他输入的
*		中断，芯片使能等都需要相应的禁止掉。
*
*/
int i_now_input = 0;

void Init_all_input_types( )
{
	static int g_Init_Input_Types = 0;

	static int i_old_input = 0;
	static int i_old_system_type = -1;
	static int i_system_run_count = 0;
	
	static int i_numbers_count = 0;
	static int i_need_handle_change = 0;
	static int i_last_code;
	int i = 0;

	 
	if(SystemState == SYSTEMSTATE_NORMAL)
	{
		if((g_spi_data >= INPUT_TYPE_INIT) && (g_spi_data < INPUT_MAX))
		{
			i_old_input 	= g_spi_data;	
		}

		if(i_old_input == INPUT_TEST)
		{
			i_old_input = i_last_code;
		}
	}

	if(SystemState == SYSTEMSTATE_ALIGNMENT)
	{
		if((i_old_input >= INPUT_TYPE_INIT) && (i_old_input < INPUT_TEST))
		{
			i_last_code = 	i_old_input;	
		}
		i_old_input = INPUT_TEST;
	}
		
	//主要是用来将切换标志清除,切换一次，更新一次标志
	if((i_now_input != i_old_input) )
	{
		i_now_input = i_old_input;	
		
		g_Init_Input_Types = 0;	
		//printf("----input type changed ---\n");
		#if USE_FADE_IN_OUT
		g_i_change_down_flag = 1;
		g_i_change_up_flag = 0;
		g_interrupt_times = 0;

		for(i = 0 ; i < 16 ; i ++)
		{
			//淡出操作
			if((T_t_audio_in_out_effect[i].g_audio_coef_for_change > 0) && 
				(T_t_audio_in_out_effect[i].g_audio_coef_for_change < 1.0))
			{
				//T_t_audio_in_out_effect[i].g_audio_coef_for_change = 1.0;
				T_t_audio_in_out_effect[i].g_audio_coef_step = 0.0001;
			}
			else
			{
				T_t_audio_in_out_effect[i].g_audio_coef_for_change = 1.0;
				T_t_audio_in_out_effect[i].g_audio_coef_step = 0.0001;
			}
		}
		#endif
	}

	#if USE_FADE_IN_OUT
	//这里需要进行缓冲，也就是要对音频信号进行相应的消除噪音处理。
	if(g_i_change_down_flag == 1)
	{
		//主要是计数来做成的延时
		if(g_interrupt_times >= 100)
		{
			g_i_change_down_flag = 0;
			g_i_change_up_flag = 1;
			for(i = 0 ; i < 16 ; i ++)
			{
				T_t_audio_in_out_effect[i].g_audio_coef_for_change = 0.0;
				T_t_audio_in_out_effect[i].g_audio_coef_step = 0.00001;
			}
			//需要将DA静音掉
			PCM4104_Mute(1);					
		}
	}
	//INPUT_PRINT("----input type:0x%08x,%d----\n",g_input_type,g_interrupt_times);
	
	//如果没有变完，后面的操作是不应该执行的
	if(g_i_change_down_flag == 1)
		return;
	#endif	
	//if(T_t_audio_in_out_effect.g_interrupt_times >= 100)
		//PCM4104_Mute(1);	
	g_input_type = i_now_input;
	
	
	
	switch(g_input_type)
	{
	case INPUT_TYPE_AD:	//输入方式为AD输入
		if(g_Init_Input_Types == 1)
			return ;	
		g_Init_Input_Types = 1;
		if(g_input_type == INPUT_TYPE_AD)
		{
			g_i_ad_in[0] = 7;
			g_i_ad_in[1] = 6;
			g_i_ad_in[2] = 2;
			g_i_ad_in[3] = 3;
			g_i_ad_in[4] = 1;
			g_i_ad_in[5] = 0;
			g_i_ad_in[6] = 4;
			g_i_ad_in[7] = 5;
		}
		AD_Disable_Others();		
		AD_AES_Init(0);
		INPUT_PRINT("----select ad----\n");         
	break;
	case INPUT_TYPE_AES://输入方式为AES输入
		if(g_Init_Input_Types == 1)
			return ;	
		g_Init_Input_Types = 1;
		if(g_input_type == INPUT_TYPE_AES)
		{
			for(i = 0 ; i < 8 ; i ++)
			{
				g_i_ad_in[i]  = i;
			}
		}			
		AES_Disable_Others();
		AD_AES_Init(1);
 
		INPUT_PRINT("----select aes----\n");
	break;
	case INPUT_TYPE_DVD://输入方式为RCA输入
		if(g_Init_Input_Types == 1)
			return ;	
		g_Init_Input_Types = 1;
		if(g_input_type == INPUT_TYPE_DVD)
		{
			g_i_ad_in[0] = 2;
			g_i_ad_in[1] = 6;
			g_i_ad_in[2] = -1;
			g_i_ad_in[3] = -1;
			g_i_ad_in[4] = -1;
			g_i_ad_in[5] = -1;
			g_i_ad_in[6] = -1;
			g_i_ad_in[7] = -1;
		}
		
		AD_Disable_Others();	
		RCA_Init();

		INPUT_PRINT("----select rca----\n");
	break;
	case INPUT_TYPE_MIC://输入方式为MIC输入
		if(g_Init_Input_Types == 1)
			return ;	
		g_Init_Input_Types = 1;

		if(g_input_type == INPUT_TYPE_MIC)
		{
			g_i_ad_in[0] = 7;
			g_i_ad_in[1] = -1;
			g_i_ad_in[2] = -1;
			g_i_ad_in[3] = -1;
			g_i_ad_in[4] = -1;
			g_i_ad_in[5] = -1;
			g_i_ad_in[6] = -1;
			g_i_ad_in[7] = -1;
		}
		
		AD_Disable_Others();		
		MIC_Init();
		
		INPUT_PRINT("----select mic----\n");
	break;
	case INPUT_TYPE_SPDIF://输入方式为SPDIF输入
		if(g_Init_Input_Types == 1)
			return ;	
		g_Init_Input_Types = 1;

		if(g_input_type == INPUT_TYPE_SPDIF)
		{
			g_i_ad_in[0] = 0;
			g_i_ad_in[1] = 1;
			g_i_ad_in[2] = -1;
			g_i_ad_in[3] = -1;
			g_i_ad_in[4] = -1;
			g_i_ad_in[5] = -1;
			g_i_ad_in[6] = -1;
			g_i_ad_in[7] = -1;
		}
		
		//将其他中断给去掉
		Init_SPDIF_COAX(0);	
	
		INPUT_PRINT("----select spdif----\n");
	break;	
	case INPUT_TYPE_COAX://输入方式为COAX输入
		if(g_Init_Input_Types == 1)
			return ;	
		g_Init_Input_Types = 1;

		if(g_input_type == INPUT_TYPE_COAX)
		{
			g_i_ad_in[0] = 0;
			g_i_ad_in[1] = 1;
			g_i_ad_in[2] = -1;
			g_i_ad_in[3] = -1;
			g_i_ad_in[4] = -1;
			g_i_ad_in[5] = -1;
			g_i_ad_in[6] = -1;
			g_i_ad_in[7] = -1;
		}
		Init_SPDIF_COAX(1);
		INPUT_PRINT("----select coax----\n");
	break;
	case INPUT_TEST:
		if(g_Init_Input_Types == 1)
			return;	
		g_Init_Input_Types = 1;
		//注意AES下的不同，将开关切换一下，否则DA时钟会被冲突
		AD_AES_Select(1);
		AD_MIC_Select(0);
		reinit_da();
		
		
		SPDIF_Disable_Others();
		AD_Disable_Others();
		//禁止AD，AES,SPDIF,COAX等中断
		INPUT_PRINT("----test mode----\n");
	break;
	}
	//将DA开启
	PCM4104_Mute(0);
}

/*
*	概述:
*		该中断为光纤产生的中断。
*
*/
void SP2ADCRcv_ISR(int sig_int){
	g_interruptFlag = 1;
    //printf("----spdif interrupt---\n");	                                                                    
} 
/*
*	概述:
*		该中断为同轴产生的中断。
*
*/
void SP3ADCRcv_ISR(int sig_int){
	g_interruptFlag = 1;                                                                    
}   


void SP4ADCRcv_ISR(int sig_int){
	g_interruptFlag = 1;                                                                    
}    

 
float da_channel_buffer_signal[NUM_SAMPLES*NUM_TX_SLOTS/2];

void test_audio_mode( void )
{
	int i,j,m;
    	int bandz;
	static int i_now_SignalModeVal =  0;
	
	static int i_fade_out_count = 0;
	static int i_fade_out_flag = 0;
	static int i_fade_in_count = 0;
	static int i_fade_in_flag = 0;
	static float i_fade_coef = 1.0;
	
	int tx_blockcount  ;


	if(g_i_da_interrupt_flag == 0)
		return;
	if(g_i_da_interrupt_flag == 1)
		g_i_da_interrupt_flag = 0;

	/*
	   {
	   	int current_dma_tx_blockpointer = get_current_dma_tx_block();
	    	if(current_dma_tx_blockpointer==g_i_da_write_addr)
	        {
	        	g_i_da_write_addr= (g_i_da_write_addr-2+CIRCLEDAMDATABUFFNUM)%CIRCLEDAMDATABUFFNUM;
	        }  	 
	          
	   }
	   */
	tx_blockcount = g_i_da_write_addr;

    	for(bandz = 1; bandz <= 31; bandz++){
	    if(AlignmentChannel <= 24){
	    	float gain = outputparam[OutputYtype].EQBandGain[AlignmentChannel - 1][bandz - 1] + outputparam[OutputYtype].EQBandGain[30 - 1][bandz - 1];//20160908 单通路较准时加入整体曲线的影响;
	    	sim_31band_EQ_setgaintofilters(bandz, AlignmentChannel - 1, gain);
	    }
	    else if(AlignmentChannel == 25){
	    	for(i = 0; i <= 1; i++){
	    		float gain = outputparam[OutputYtype].EQBandGain[i][bandz - 1] + outputparam[OutputYtype].EQBandGain[AlignmentChannel - 1][bandz - 1];
	    		sim_31band_EQ_setgaintofilters(bandz, i, gain);
	    	}
	    }
		else if(AlignmentChannel == 26){
	    	for(i = 0; i <= 5; i++){
	    		float gain = outputparam[OutputYtype].EQBandGain[i][bandz - 1] + outputparam[OutputYtype].EQBandGain[AlignmentChannel - 1][bandz - 1];
	    		sim_31band_EQ_setgaintofilters(bandz, i, gain); 
	    	}
		}
		else if(AlignmentChannel == 27){
	    	for(i = 0; i <= 7; i++){
	    		float gain = outputparam[OutputYtype].EQBandGain[i][bandz - 1] + outputparam[OutputYtype].EQBandGain[AlignmentChannel - 1][bandz - 1];
	    		sim_31band_EQ_setgaintofilters(bandz, i, gain); 
	    	}
		}
		else if(AlignmentChannel == 28){
	    	for(i = 0; i <= 10; i++){
	    		float gain = outputparam[OutputYtype].EQBandGain[i][bandz - 1] + outputparam[OutputYtype].EQBandGain[AlignmentChannel - 1][bandz - 1];
	    		sim_31band_EQ_setgaintofilters(bandz, i, gain); 
	    	}
		}
	   	else if(AlignmentChannel == 29){
	    	for(i = 0; i <= 14; i++){
	    		float gain = outputparam[OutputYtype].EQBandGain[i][bandz - 1] + outputparam[OutputYtype].EQBandGain[AlignmentChannel - 1][bandz - 1];
	    		sim_31band_EQ_setgaintofilters(bandz, i, gain); 
	    	} 
	   	}
	    else if(AlignmentChannel == 30){
	    	for(i = 0; i <= 23; i++){
	    		float gain = outputparam[OutputYtype].EQBandGain[i][bandz - 1] + outputparam[OutputYtype].EQBandGain[AlignmentChannel - 1][bandz - 1];
	    		sim_31band_EQ_setgaintofilters(bandz, i, gain); 
	    	}
	    }
	}
	 
	//产生新的pcm数据, 
	testsignal_generator_oneframe(SignalModeVal, FFTSIZE / 2, da_channel_buffer[0], 1);

	/***********1.为方便测试，下面是简单输出的程序**********/
	#if 0
    	for(j = 0; j < 8; j++)
    	{
    		//if(channeloutputDAflag[j][0]+channeloutputDAflag[j][1]+channeloutputDAflag[j][2]==0)
    		//	continue;
        
        	for(i = 0; i < FFTSIZE / 2; i++)
        	{
            		da_channel_buffer[j][i] = da_channel_buffer[0][i];
			outpcmDAdatabuf[g_i_da_out[j]][i * 2 + tx_blockcount * NUM_SAMPLES * NUM_TX_SLOTS] = 
					((int )(da_channel_buffer[j][i]))>>8;
        	}
    	}
	return;
	#endif
	
    	for(j = 0; j < 16; j++)
    	{
        	for(i = 0; i < FFTSIZE / 2; i++)
            		da_channel_buffer[j][i] = da_channel_buffer[0][i];
    	}
	
    	for(j = 0; j < 16; j++)
    	{
        	//for(i = 0; i < FFTSIZE / 2; i++)
			//da_channel_buffer[j][i] = da_channel_buffer[0][i];
	
		filter_process_GEQ_block(da_channel_buffer[j], da_channel_buffer[j], FFTSIZE / 2, j);
		filter_process_HPF_block_dl(da_channel_buffer[j], da_channel_buffer[j], FFTSIZE / 2, j);
		filter_process_LPF_block(da_channel_buffer[j], da_channel_buffer[j], FFTSIZE / 2, j);
		#if 1
        	//声道开关
        	if(AlignmentChannel <= 24)
        		V0[j] = (j == AlignmentChannel - 1)? 
        				outputparam[OutputYtype].ChannelGain_Val[AlignmentChannel - 1]: 0;
       	else if (AlignmentChannel == 25)
       		V0[j] = (j <= 1)? 
       				outputparam[OutputYtype].ChannelGain_Val[j]: 0;
       	else if (AlignmentChannel == 26)
       		V0[j] = (j <= 5)? outputparam[OutputYtype].ChannelGain_Val[j]: 0;
       	else if (AlignmentChannel == 27)
       		V0[j] = (j <= 7)? outputparam[OutputYtype].ChannelGain_Val[j]: 0;
       	else if (AlignmentChannel == 28 && (OutputYtype == 2 || OutputYtype == 3))
       		V0[j] = (j <= 9)? outputparam[OutputYtype].ChannelGain_Val[j]: 0;
       	else if (AlignmentChannel == 29 && (OutputYtype == 2 || OutputYtype == 3))
       		V0[j] = (j <= 11)? outputparam[OutputYtype].ChannelGain_Val[j]: 0;
       	else if (AlignmentChannel == 30 && OutputYtype == 3)
       		V0[j] = outputparam[OutputYtype].ChannelGain_Val[j];
	
       	 for(i = 0; i < FFTSIZE / 2; i++)
            		//da_channel_buffer[j][i] *= ((int)(V0[j] * knobvolumegainvalue ))>>8;
            		da_channel_buffer[j][i] *= (V0[j] * knobvolumegainvalue );

		#endif
    	}

    	for(j = 0; j < 14; j++)
    	{
    		int bandx;
       	int pcmval1, pcmval2, pcmval3, pcmval4;
        	for(bandx = 0; bandx < 3; bandx ++)
        	{		
        		int channel_index =j;
            		int route_index =outsignalchannel_book24m3[channel_index][bandx];
            		if(route_index<0)
            			continue;
			//分频
			p_divisionoutbuf=&inputmultichannelfftdata[0];	
				
			frequencydivision_channel(channel_index,bandx, da_channel_buffer[channel_index],
											p_divisionoutbuf,FFTSIZE/2);
				
			for(i = 0; i < FFTSIZE / 2; i++)
			{
				//分为淡入和淡出两种状态
				if(g_i_change_down_flag == 1)
				{
					if(T_t_audio_in_out_effect[route_index].g_audio_coef_for_change > 0)
					{
						T_t_audio_in_out_effect[route_index].g_audio_coef_for_change -= 
										T_t_audio_in_out_effect[route_index].g_audio_coef_step;
						//printf("T_t_audio_in_out_effect.g_audio_coef_for_change:%f\n",T_t_audio_in_out_effect.g_audio_coef_for_change);
					}

					if(T_t_audio_in_out_effect[route_index].g_audio_coef_for_change <= 0)
					{
						T_t_audio_in_out_effect[route_index].g_audio_coef_for_change = 0;
					}
					//outpcmDAdatabuf[j][i] *=  (T_t_audio_in_out_effect[m].g_audio_coef_for_change);  
				}
				if(g_i_change_up_flag == 1)
				{
					if(T_t_audio_in_out_effect[route_index].g_audio_coef_for_change < 1)
					{
						T_t_audio_in_out_effect[route_index].g_audio_coef_for_change += T_t_audio_in_out_effect[route_index].g_audio_coef_step;
					}
					//outpcmDAdatabuf[j][i] *=  (T_t_audio_in_out_effect[m].g_audio_coef_for_change);
				}

				outpcmDAdatabuf[g_i_da_out[route_index]][i * 2 + tx_blockcount * NUM_SAMPLES * NUM_TX_SLOTS] =   
					((p_divisionoutbuf[i]* (T_t_audio_in_out_effect[route_index].g_audio_coef_for_change)))*(1<<23 - 1); 

			}	 
		} 
    	}
}


void set_interrupt_times( void  )
{
	#if USE_FADE_IN_OUT
	//计算中断多少次，用来处理淡入淡出的问题
	g_interrupt_times ++;
	#endif
}

section ("seg_ext_dmda") int i_now_da_addr_g_i_da_write_addr[1000][2];
int error2num = 0;
int error0num = 0;
section ("seg_ext_dmda") int i_l_current_index_g_i_da_write_addr[1000][3];
int i_l_current_index_g_i_da_write_addr_delta=-10;
int i_l_current_index_error=0;


void handle_audio_data(int i_l_current_index,int *p_input_buf[16])
{
	int i = 0,j = 0;
	//spdif的数据处理
	int	i_l_da_tx_addr;
	time_t  time_start;
	time_t  time_end;
	time_t  time_interval;

	static int g_biggest_time = 0;
	static int g_count_time_numbers = 0;
	
	int i_need_to_write_addr;
	i_need_to_write_addr = ((g_i_da_write_addr - 2 + NUMDMAUSED) % NUMDMAUSED);

	time_start = clock();
#if 0
	for(i = 0 ; i < 8 ; i ++)
	{	
		for(j = 0 ; j < NUM_SAMPLES * NUM_TX_SLOTS / 2 ; j ++)
		{	
			da_channel_buffer[i][j] = 
				p_input_buf[g_i_ad_in[i]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] >> 8 ;			

			outpcmDAdatabuf[g_i_da_out[i]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
				da_channel_buffer[i][j] ;
		}
	}
	return;
#endif
	
	

	/****************  1.输入数据初步处理*****************************/
	#if 1	
	//对各个通道做信号均衡，这里就全部都做
	for(i = 0 ; i < 8 ; i ++)
	{	
		for(j = 0 ; j < NUM_SAMPLES * NUM_TX_SLOTS / 2 ; j ++)
		{
			if(g_i_ad_in[i] == -1)
			{
				da_channel_buffer[i][j] = 0;
			}
			else
			{
				da_channel_buffer[i][j] =  
					(p_input_buf[g_i_ad_in[i]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] >> 8) * knobvolumegainvalue;//>> 8);
				//(T_t_audio_in_out_effect.g_audio_coef_for_change)
			}
		}
	}
	#endif

	#if 1
		#if 0
		for(i = 0 ; i < 8 ; i ++)
		{	
			for(j = 0 ; j < NUM_SAMPLES * NUM_TX_SLOTS / 2 ; j ++)
			{	
				if(g_i_ad_in[i] < 0)
				{
					da_channel_buffer[i][j] = 0;
				}
				else
				{
					da_channel_buffer[i][j] = 
						(p_input_buf[g_i_ad_in[i]][j * 2 + 
							i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] >> 8) *knobvolumegainvalue;			
				}
				//outpcmDAdatabuf[g_i_da_out[i]][j * 2 + g_i_da_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] = 
				//	da_channel_buffer[i][j];
				//清除其他8路的数据
				da_channel_buffer[i+8][j] = 0;
			}
		}
	 	#endif
		/*************2.矩阵映射与淡入淡出处理**********************************/
		matrixtooutputchannel(da_channel_buffer,da_channel_buffer_multi);
		//淡入淡出处理
		for(j = 0 ; j < NUM_SAMPLES * NUM_TX_SLOTS / 2 ; j ++)
		{
			for(i = 0 ; i < 16 ; i ++)
			{
				
				//分为淡入和淡出两种状态
				if(g_i_change_down_flag == 1)
				{
					if(T_t_audio_in_out_effect[i].g_audio_coef_for_change > 0)
					{
						T_t_audio_in_out_effect[i].g_audio_coef_for_change -= T_t_audio_in_out_effect[i].g_audio_coef_step;
						//printf("T_t_audio_in_out_effect.g_audio_coef_for_change:%f\n",T_t_audio_in_out_effect.g_audio_coef_for_change);
					}
					if(T_t_audio_in_out_effect[i].g_audio_coef_for_change <= 0)
					{
						T_t_audio_in_out_effect[i].g_audio_coef_for_change = 0;
					}
					da_channel_buffer_multi[i][j] *=  (T_t_audio_in_out_effect[i].g_audio_coef_for_change);  
				}
				if(g_i_change_up_flag == 1)
				{
					if(T_t_audio_in_out_effect[i].g_audio_coef_for_change < 1)
					{
						T_t_audio_in_out_effect[i].g_audio_coef_for_change += T_t_audio_in_out_effect[i].g_audio_coef_step;
					}
					da_channel_buffer_multi[i][j] *=  (T_t_audio_in_out_effect[i].g_audio_coef_for_change);
				}
				
			 } 	 
		}
	#else
	
	for(i = 0 ; i < 8 ; i ++)
	{	
		for(j = 0 ; j < NUM_SAMPLES * NUM_TX_SLOTS / 2 ; j ++)
		{	
			if(g_i_ad_in[i] < 0)
			{
				da_channel_buffer[i][j] = 0;
			}
			else
			{
				da_channel_buffer[i][j] = 
					(p_input_buf[g_i_ad_in[i]][j * 2 + 
						i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] >> 8) *knobvolumegainvalue;			
			}
			outpcmDAdatabuf[g_i_da_out[i]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
				da_channel_buffer[i][j];
		}
	}
	return;
	#endif

 
	/*************3.监听数据处理**********************************/
	//监听输出
	for(i = 0; i < 2; i++)
	{
		for(j = 0 ; j < NUM_SAMPLES * NUM_TX_SLOTS / 2 ; j ++)
		{
			int pcmval1 ;
			if(g_i_ad_in[i] == -1)
			{
				pcmval1 = 0;	
			}
			else
				pcmval1 = ((int)(p_input_buf[g_i_ad_in[i]]
					[j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] * knobvolumegainvalue))>> 8;
			//int pcmval1 = (p_input_buf[g_i_ad_in[i]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] >> 8)* knobvolumegainvalue;
			//outpcmDAdatabuf[g_i_da_out[i+14]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = pcmval1;
			outpcmDAdatabuf[g_i_da_out[i+14]][j * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] = pcmval1;
			//中置数据	
			if(g_i_ad_in[2] == -1)
				pcmval1 = 0;
			else
				pcmval1 = ((int)(p_input_buf[g_i_ad_in[2]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS]* knobvolumegainvalue))	>> 8;
			outpcmDAdatabuf[g_i_da_out[i+14]][j * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] += (pcmval1*0.707);

			//float pcmval1 = da_channel_buffer_multi[i][j];
			//outpcmDAdatabuf[g_i_da_out[i+14]][j * 2 + g_i_da_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] = pcmval1;	
		}
	}
	/***********2.1 将输出的数据全部清空********************/
	//置0
	#if 1
	for(i = 0;i < 14;i++)
	{
		 for(j = 0; j < NUM_SAMPLES * NUM_TX_SLOTS / 2; j += 1)
			outpcmDAdatabuf[g_i_da_out[i]][j * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS]=0;
	}
	#endif
	/***********2.2 测试的的时候将数据输出即可**************************/
	#if 0
	for(i = 0;i < 14;i++)
	{
		 for(j = 0; j < NUM_SAMPLES * NUM_TX_SLOTS / 2; j += 1)
			outpcmDAdatabuf[g_i_da_out[i]][j * 2 + g_i_da_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] 
														= da_channel_buffer_multi[i][j];
	}
	return;
	#endif
	/***********3.算法部分数据处理****************************/
	#if 1
	int channel_index;
	for(channel_index = 0 ; channel_index < 14 ; channel_index ++)
	{
		int bandnum,bandx;
		
		filter_process_GEQ_block(da_channel_buffer_multi[channel_index],da_channel_buffer_multi[channel_index], FFTSIZE / 2, channel_index);
		filter_process_HPF_block_dl(da_channel_buffer_multi[channel_index], da_channel_buffer_multi[channel_index], FFTSIZE / 2, channel_index);
		filter_process_LPF_block(da_channel_buffer_multi[channel_index], da_channel_buffer_multi[channel_index], FFTSIZE / 2, channel_index);
		
		//bandnum = 2;
		bandnum = outputparam[OutputYtype].FreqDivVal[channel_index][0];
		if(bandnum<1)
			bandnum = 1;
		if(bandnum>3)
			bandnum = 3;
		
		for(bandx=0; bandx<bandnum;bandx++)
		{
			int route_index =outsignalchannel_book24m3[channel_index][bandx];
			if(route_index<0)
				continue;

			int delayreadptr = (DelayBuf_writeptr - Delaytime_samplenums[route_index]
													+ DelayBufSIZE ) % DelayBufSIZE;
			
			p_divisionoutbuf = &inputmultichannelfftdata[0];
			frequencydivision_channel(channel_index,bandx, da_channel_buffer_multi[channel_index],
										p_divisionoutbuf,FFTSIZE/2);
			
			//将数据写入延迟缓冲区中
			for(i=0;i<FFTSIZE/2;i+=1) 
			{
				//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
				//	((int )(p_divisionoutbuf[i]));//>>8;	
				
				
				DelayBuf[route_index][DelayBuf_writeptr+i] = ((int )(p_divisionoutbuf[i]));
				//DelayBuf[route_index][DelayBuf_writeptr+i] = ((int )(da_channel_buffer_multi[channel_index][i]));
			}
			
			#if 1
			//printf("----math-----\n");
			//再将延迟缓冲区的数据写入对应的DA通道上
			if(delayreadptr + (FFTSIZE/2)<=DelayBufSIZE)
			{
				for(i=0;i<FFTSIZE/2;i+=1)
				{
					int pcmval1;
					pcmval1 = DelayBuf[route_index][delayreadptr+i];//i*1.0/10240.0*(0x1<<23);//DelayBuf[routeindex][delayreadptr+i]*(0x1<<23);

					#if 1
					if(pcmval1 > (int)(0x1 << 23) - 1)
                   				pcmval1 = (int)(0x1 << 23) - 1;
               			if(pcmval1 < -(int)(0x1 << 23))
                   				pcmval1 = -(int)(0x1 << 23);
					#endif

					if (Mute == 1)
					{
						if (FadeInFlag == 1)
						{
							FadeInCoeff += FadeInTimePerFrame;
							if (FadeInCoeff > 1)
							{
								FadeInCoeff = 1;
								FadeInFlag = 0;
							}
						}
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] =  
						//		(FadeInCoeff >= 1) ? pcmval1 : pcmval1 * FadeInCoeff;
																	
						outpcmDAdatabuf[g_i_da_out[route_index]][i * 2 + i_need_to_write_addr* NUM_SAMPLES * NUM_TX_SLOTS] =  
								(FadeInCoeff >= 1) ? pcmval1 : pcmval1 * FadeInCoeff;
						
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] += (FadeInCoeff >= 1) ? pcmval1 : pcmval1 * FadeInCoeff;
						//printf("FadeInCoeff = %d,FadeInFlag = %d,outpcmDAdatabuf[0][0]=%d\n",FadeInCoeff,FadeInFlag,outpcmDAdatabuf[0][0]);
					}
					else
					{
						if (FadeOutFlag == 1)
						{
							FadeOutCoeff -= FadeOutTimePerFrame;
							if (FadeOutCoeff < 0)
							{
								FadeOutCoeff = 0;
								FadeOutFlag = 0;
							}
						}
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] =  
						//		(FadeOutCoeff <= 0) ? 0 : pcmval1 * FadeOutCoeff;
						outpcmDAdatabuf[g_i_da_out[route_index]][i * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] =  
								(FadeOutCoeff <= 0) ? 0 : pcmval1 * FadeOutCoeff;									
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] += (FadeOutCoeff <= 0) ? 0 : pcmval1 * FadeOutCoeff;
						//printf("FadeOutCoeff = %d,FadeOutFlag = %d,outpcmDAdatabuf[0][0]=%d\n",FadeOutCoeff,FadeOutFlag,outpcmDAdatabuf[0][0]);
					}
				}
			}
			else
			{
				for(i=0;i<DelayBufSIZE-delayreadptr;i+=1)
				{
					int pcmval1;
					pcmval1 = DelayBuf[route_index][delayreadptr+i];//i*1.0/10240.0*(0x1<<23);//DelayBuf[routeindex][delayreadptr+i]*(0x1<<23);

					#if 1
					if(pcmval1 > (int)(0x1 << 23) - 1)
            					pcmval1 = (int)(0x1 << 23) - 1;
        				if(pcmval1 < -(int)(0x1 << 23))
            					pcmval1 = -(int)(0x1 << 23);
            				#endif
					
					//outpcmDAdatabuf[routeindex][i*NUM_TX_SLOTS + tx_blockoffset] = pcmval1;
					if (Mute == 1)
					{
						if (FadeInFlag == 1)
						{
							FadeInCoeff += FadeInTimePerFrame;
							if (FadeInCoeff > 1)
							{
								FadeInCoeff = 1;
								FadeInFlag = 0;
							}
						}
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
						//			(FadeInCoeff >= 1) ? pcmval1 : pcmval1 * FadeInCoeff;
						outpcmDAdatabuf[g_i_da_out[route_index]][i * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] = 
									(FadeInCoeff >= 1) ? pcmval1 : pcmval1 * FadeInCoeff;
						//printf("FadeInCoeff = %d,FadeInFlag = %d,outpcmDAdatabuf[0][0]=%d\n",FadeInCoeff,FadeInFlag,outpcmDAdatabuf[0][0]);
					}
					else
					{
						if (FadeOutFlag == 1)
						{
							FadeOutCoeff -= FadeOutTimePerFrame;
							if (FadeOutCoeff < 0)
							{
								FadeOutCoeff = 0;
								FadeOutFlag = 0;
							}
						}
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
						//				(FadeOutCoeff <= 0) ? 0 : pcmval1 * FadeOutCoeff;
						outpcmDAdatabuf[g_i_da_out[route_index]][i * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] = 
										(FadeOutCoeff <= 0) ? 0 : pcmval1 * FadeOutCoeff;
						
						//printf("FadeOutCoeff = %d,FadeOutFlag = %d,outpcmDAdatabuf[0][0]=%d\n",FadeOutCoeff,FadeOutFlag,outpcmDAdatabuf[0][0]);
					}	
				}
				for(;i<FFTSIZE/2;i+=1) //2*6*3/4 = 9
				{
					int pcmval1;
					pcmval1 = DelayBuf[route_index][delayreadptr+i-DelayBufSIZE];//i*1.0/10240.0*(0x1<<23);//DelayBuf[routeindex][delayreadptr+i-DelayBufSIZE]*(0x1<<23);

					#if 1
					if(pcmval1 > (int)(0x1 << 23) - 1)
						pcmval1 = (int)(0x1 << 23) - 1;
					if(pcmval1 < -(int)(0x1 << 23))
						pcmval1 = -(int)(0x1 << 23);
					#endif
					
					//outpcmDAdatabuf[routeindex][i*NUM_TX_SLOTS + tx_blockoffset] = pcmval1;
					if (Mute == 1)
					{
						if (FadeInFlag == 1)
						{
							FadeInCoeff += FadeInTimePerFrame;
							if (FadeInCoeff > 1)
							{
								FadeInCoeff = 1;
								FadeInFlag = 0;
							}
						}
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
						//		(FadeInCoeff >= 1) ? pcmval1 : pcmval1 * FadeInCoeff;
						outpcmDAdatabuf[g_i_da_out[route_index]][i * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] = 
								(FadeInCoeff >= 1) ? pcmval1 : pcmval1 * FadeInCoeff;
					}
					else
					{
						if (FadeOutFlag == 1)
						{
							FadeOutCoeff -= FadeOutTimePerFrame;
							if (FadeOutCoeff < 0)
							{
							FadeOutCoeff = 0;
								FadeOutFlag = 0;
							}
						}
						//outpcmDAdatabuf[g_i_da_out[routeindex]][i * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
						//			(FadeOutCoeff <= 0) ? 0 : pcmval1 * FadeOutCoeff;
						outpcmDAdatabuf[g_i_da_out[route_index]][i * 2 + i_need_to_write_addr * NUM_SAMPLES * NUM_TX_SLOTS] = 
									(FadeOutCoeff <= 0) ? 0 : pcmval1 * FadeOutCoeff;
					//	printf("FadeOutCoeff = %d,FadeOutFlag = %d,outpcmDAdatabuf[0][0]=%d\n",FadeOutCoeff,FadeOutFlag,outpcmDAdatabuf[0][0]);
					}
				}
			
			}
			#endif
		}
	}
	//延迟缓冲区指针更新
	DelayBuf_writeptr =((DelayBuf_writeptr +(FFTSIZE/2))%DelayBufSIZE);	
	#endif
	
	/***********4.数据输出*****************************************/
	//真正的数据输出部分
	#if 0
	for(i = 0 ; i < 8 ; i ++)
	{
		for(j = 0 ; j < NUM_SAMPLES * NUM_TX_SLOTS / 2 ; j ++)
		{
			#if USE_FADE_IN_OUT
				outpcmDAdatabuf[g_i_da_out[i]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
				da_channel_buffer[i][j];
			#else
				outpcmDAdatabuf[g_i_da_out[i]][j * 2 + i_l_current_index * NUM_SAMPLES * NUM_TX_SLOTS] = 
				da_channel_buffer[i][j];			
			#endif
		}
	}
	#endif
	
	time_end = clock();
	time_interval = time_end - time_start;

	g_count_time_numbers ++;
	if(g_count_time_numbers <1000)
	{
		if(g_biggest_time < time_interval)
			g_biggest_time = time_interval;
	}

	if(g_count_time_numbers == 1000)
	{
		g_count_time_numbers = 1000;
		
	}
			
}

/*
*
*		描述:	该部分是对spdif进来的数据进行算法处理等。
*
*
*/
int g_i_spdif_current_index = 0;

void handle_spdif_data(void)
{
	int i = 0,j = 0;
	int i_l_spdif_current;
	//提取I2S的一半的数据，分作两用
	 
	if(g_interruptFlag == 1)
	{
		g_interruptFlag = 0; 

		//必须知道数据接收在哪一个位置
		//g_i_spdif_current_index ++;
		//g_i_spdif_current_index = g_i_spdif_current_index % NUMDMAUSED;
		g_i_spdif_current_index = Get_Current_Dma_Rxblock_SPDIF();
		//i_l_spdif_current = (g_i_spdif_current_index - 2 + NUMDMAUSED) % NUMDMAUSED;
		i_l_spdif_current = g_i_spdif_current_index;

		if(((g_i_da_write_addr) % NUMDMAUSED) == i_l_spdif_current)
		{
			i_l_spdif_current = (i_l_spdif_current + 1) % NUMDMAUSED;	 
		}
		else if(((g_i_da_write_addr + 1) % NUMDMAUSED) == i_l_spdif_current)
		{
			i_l_spdif_current = (i_l_spdif_current) % NUMDMAUSED; 
		}
 
		handle_audio_data(i_l_spdif_current,p_multichannel_input_spdif_buf);
 
	 
	}
}


int g_i_test_cs5368_interrupt_times = 0;

void SP6ADCRcv_ISR(int sig_int){
	g_interruptFlag = 1;       


	g_i_test_cs5368_interrupt_times++;
}

/*
*
*		描述:	该部分是对AES或者AD进来的数据进行算法处理等。
*
*
*/
static int g_i_ad_current_index = 2;

void clear_ad_interrupt_index(void )
{	
	g_i_ad_current_index = NUMDMAUSED-1;	
}
 
static int first_time = 0;
static int first_value = 0;

static int g_i_pos_cof = 0;

void handle_ad_aes_data(void)
{
	int i = 0,j;
	int i_l_ad_current_index = 0;
	 
	if(g_interruptFlag == 1)
	{
		g_interruptFlag = 0;
		//printf("----ad input-----\n");
		//必须知道数据接收在哪一个位置
		g_i_ad_current_index = Get_Current_Dma_CS5368_Rxblock()  ;
		//g_i_ad_current_index ++;
		//g_i_ad_current_index = g_i_ad_current_index % NUMDMAUSED;
		//这是当前dma的位置，不能读取的位置
		//i_l_ad_current_index = (g_i_ad_current_index - 1 + NUMDMAUSED) % NUMDMAUSED;
		i_l_ad_current_index = g_i_ad_current_index;

		if(((g_i_da_write_addr) % NUMDMAUSED) == i_l_ad_current_index)
		{
			i_l_ad_current_index = (i_l_ad_current_index + 1) % NUMDMAUSED;	 
		}
		else if(((g_i_da_write_addr + 1) % NUMDMAUSED) == i_l_ad_current_index)
		{
			i_l_ad_current_index = (i_l_ad_current_index) % NUMDMAUSED; 
		}	
		
		handle_audio_data(i_l_ad_current_index,p_multichanneloutputbuf);
	}	
}

/*
*
*		描述:	 
*
*
*/
void handle_all_input_data(int i_input_types)
{
	static int g_spdifNowFlag = 0;
	static int g_spdifChangedFlag = 0;
			
	switch(i_input_types)
	{
	case INPUT_TYPE_AD://输入方式为AD输入
	case INPUT_TYPE_AES://输入方式为AES输入
		handle_ad_aes_data();	
	break;
	case INPUT_TYPE_COAX://输入方式为COAX输入
	case INPUT_TYPE_SPDIF://输入方式为SPDIF输入
		handle_spdif_data();
		#if 0
		{
		    if ((*pDIRSTAT & DIR_LOCK))
		    {
		    	g_spdifChangedFlag = 1;	
		    }
		    //SPDIFstream_state = (*pDIRSTAT & DIR_NOAUDIOLR) ? 2 : 1;
		    //else if(*pDIRSTAT & DIR_VALID)
		    else
		    {
		        //SPDIFstream_state = 0;
		    	g_spdifChangedFlag = 0;
		    }
   
		   if(g_spdifNowFlag != g_spdifChangedFlag)
		   {
		   		g_spdifNowFlag = g_spdifChangedFlag;
		   		//Re_Init_CS4385();
	   			if(g_input_type == INPUT_TYPE_SPDIF)
	   			{
		   			Init_SPDIF_COAX(0);
	   			}
	   			else
		   		{
		   			Init_SPDIF_COAX(1);
		   		}
		   		//printf("---re_init spdif----\n");
		   }
		}
		#endif	
	break;
	case INPUT_TYPE_DVD://输入方式为RCA输入
		handle_ad_aes_data();			
	break;
	case INPUT_TYPE_MIC://输入方式为MIC输入	
		handle_ad_aes_data();	
	break;
	case INPUT_TEST:
		test_audio_mode();
	break;
	default:
	break;
	}		
}








