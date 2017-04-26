/*****************************************************************************
* wavartsdsp23.c
 SPI ָ��Э��Ĵ���
*****************************************************************************/

#include <def21489.h>
#include <cdef21489.h>
#include <sru.h>
#include <math.h>
#include <complex.h>
#include <sysreg.h>
#include <stdio.h>
#include "./include/dsp_signal.h"
#include "./include/ADDS_21489_EzKit.h"
#include "./include/volume.h"
#include "./include/eq.h"

int LastInputXtype = 0;
#define USE_DEBUG_SPI_CODE 0

#if USE_DEBUG_SPI_CODE
	#define DEBUG_SPI_CODE(fmt,...)	printf(fmt,##__VA_ARGS__)
#else
	#define DEBUG_SPI_CODE(fmt,...)	
#endif

section("seg_ext_dmda") float DelayBuf[MAXROUTENUM][DelayBufSIZE];
//float *pDelayBuf[MAXROUTENUM];

int Delaytime_samplenums[MAXROUTENUM]={0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0};//��󲻳���DelayBufSIZE-FFTSIZE
int DelayBuf_writeptr=0; //delaybuff��дָ��λ��
								
extern int outsignalchannel_book24m3[MAXCAHNNELNUM][3];
float knobvolumegainvalue = 0.1;

//��ȡ��ʵ�ʵ�SPI����,��Ȼ�Ǿ�������ɸѡ֮�������2016-11-22
extern int g_spi_data;

//ָ�����
unsigned int getinstructiontype(unsigned int instructionparaword)
{ 
    return (instructionparaword & 0xC0000000) >> 30;
} 

unsigned int getinstructiongroup(unsigned int instructionparaword)
{
    return (instructionparaword & 0x3C000000) >> 26;
}

unsigned int getinstructionindexingroup(unsigned int instructionparaword)
{
    return (instructionparaword & 0x3E00000) >> 21;
}

unsigned int getpara1_unsignedint(unsigned int instructionparaword)
{
    return (instructionparaword & 0x1F0000) >> 16;
}

unsigned int getpara2_unsignedint(unsigned int instructionparaword)
{
    return (instructionparaword & 0xFF00) >> 8;
}

int getpara2_signedint(unsigned int instructionparaword)
{
    int word = (instructionparaword & 0xFF00) << 16; 
    return word >> 24;
}

unsigned int getpara3_unsignedint(unsigned int instructionparaword)
{
    return instructionparaword & 0xFF;
}

int getpara3_signedint(unsigned int instructionparaword)
{
    int word = (instructionparaword & 0xFF) << 24;
    return word >> 24;
}

unsigned int getpara23_unsignedint(unsigned int instructionparaword)
{
	return 256 * getpara2_unsignedint(instructionparaword) + getpara3_unsignedint(instructionparaword);
}

//ָ���
/*
31	30  |29	28	27	26|	25	24	23	22	21|	20	19	18	17	16|	15	14	13	12	11	10	09	08|	07	06	05	04	03	02	01	00
��������|	�������  |	    �������      |	     ��һ����     |	          �ڶ�����            |	          ��������
*/

//ָ�����
unsigned int instruction_encoder(unsigned int instructiontype, unsigned int instructiongroup, unsigned int instructionindexingroup, unsigned int para1, unsigned int para2, unsigned int para3)
{
    return (unsigned int)((instructiontype & 0x3) << 30)
           | ((instructiongroup & 0xF) << 26)
           | ((instructionindexingroup & 0x1F) << 21)
           | ((para1 & 0x1F) << 16)
           | ((para2 & 0xFF) << 8)
           | ((para3 & 0xFF));
}

///////////////////////// ͨ�ò���//////////////////////////////////////
/*
1.1 ��������
*/
int instructiondecoder_SoundFieldX_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara23_unsignedint(g_spi_data) > 10000)
        return 1;
        
    SoundFieldX = getpara23_unsignedint(g_spi_data);
    
    DEBUG_SPI_CODE("SoundFieldX:0x%08x\n",SoundFieldX);
    
    return 0;
}


/*
1.2 �������
*/
int instructiondecoder_SoundFieldY_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara23_unsignedint(g_spi_data) > 5000)
        return 1;
        
    SoundFieldY = getpara23_unsignedint(g_spi_data);
    
    DEBUG_SPI_CODE("SoundFieldY:0x%08x\n",SoundFieldY);
    
    return 0;
}


/*
1.3 �����߶�
*/
int instructiondecoder_SoundFieldZ_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara23_unsignedint(g_spi_data) > 3000)
        return 1;
        
    SoundFieldZ = getpara23_unsignedint(g_spi_data);
 
	DEBUG_SPI_CODE("SoundFieldZ:0x%08x\n",SoundFieldZ);
    
    return 0;
}


/*
1.4 ����������
*/
int instructiondecoder_SpeakerNumber_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara2_unsignedint(g_spi_data) > 24)
        return 1;
        
    SpeakerNumber = getpara2_unsignedint(g_spi_data);
 
	DEBUG_SPI_CODE("SpeakerNumber:0x%08x\n",SpeakerNumber);
	   
    return 0;
}


/*
1.5 ������λ��X
*/
int instructiondecoder_SpeakerX_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 24)
    	return 1;
    	
    if(getpara23_unsignedint(g_spi_data) > 10000)
        return 1;
        
    SpeakerX[getpara1_unsignedint(g_spi_data) - 1] = getpara23_unsignedint(g_spi_data);
    
    DEBUG_SPI_CODE("SpeakerX[getpara1_unsignedint(g_spi_data) - 1]:0x%08x\n",SpeakerX[getpara1_unsignedint(g_spi_data) - 1]);
    
    return 0;
}


/*
1.6 ������λ��Y
*/
int instructiondecoder_SpeakerY_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 24)
    	return 1;
    	
    if(getpara23_unsignedint(g_spi_data) > 10000)
        return 1;
        
    SpeakerY[getpara1_unsignedint(g_spi_data) - 1] = getpara23_unsignedint(g_spi_data);
 	DEBUG_SPI_CODE("SpeakerY[getpara1_unsignedint(g_spi_data) - 1]:0x%08x\n",SpeakerY[getpara1_unsignedint(g_spi_data) - 1]);
       
    return 0;
}


/*
1.7 ������λ��Z
*/
int instructiondecoder_SpeakerZ_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 24)
    	return 1;
    	
    if(getpara23_unsignedint(g_spi_data) > 10000)
        return 1;
        
    SpeakerZ[getpara1_unsignedint(g_spi_data) - 1] = getpara23_unsignedint(g_spi_data);
 	DEBUG_SPI_CODE("SpeakerZ[getpara1_unsignedint(g_spi_data) - 1]:0x%08x\n",SpeakerZ[getpara1_unsignedint(g_spi_data) - 1]);
     
    return 0;
}


/*
1.8 ��ť����
	KnobVolume	0
*/
int instructiondecoder_KnobVolume_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara2_unsignedint(g_spi_data) > 96)
        return 1;
    
    //if(g_DSP23 == 2 && InitDone == 1)
    //	return 1;
    
    g_volume_count_value = getpara2_unsignedint(g_spi_data);	    
    knobvolume = 1.0 * getpara2_unsignedint(g_spi_data);
    knobvolumegainvalue = powf(10.0, (knobvolume - 90) / 20);

	DEBUG_SPI_CODE("knobvolume:%f\n",knobvolume);
 	DEBUG_SPI_CODE("knobvolumegainvalue:%f\n",knobvolumegainvalue);

	    
    return 0;
}


/*
1.9 ��������
InputXtype	0	0
*/
int instructiondecoder_InputXtype_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 7 || getpara1_unsignedint(g_spi_data) == 0)
    	return 1;

    	InputXtype = getpara1_unsignedint(g_spi_data);
    	LastInputXtype = InputXtype;

	//printf("InputXtype = 0x%0x\n",InputXtype);
	DEBUG_SPI_CODE("InputXtype:%f\n",InputXtype);
 
    return 0;
}






/*
1.10 ��ǿʱ��
FadeInTime	0	0
*/
int instructiondecoder_FadeInTime_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 10 || getpara1_unsignedint(g_spi_data) < 2)
    	return 1;

    FadeInTime = 0.1 * getpara1_unsignedint(g_spi_data);
	FadeInTimePerFrame = 4.0e-4 * NUM_SAMPLES / inputparam[InputXtype].SamplingRate / FadeInTime;

	DEBUG_SPI_CODE("FadeInTime:%f\n",FadeInTime);
 	DEBUG_SPI_CODE("FadeInTimePerFrame:%f\n",FadeInTimePerFrame);

	
    return 0;
}


/*
1.11 ����ʱ��
FadeOutTime	0	0
*/
int instructiondecoder_FadeOutTime_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 10 || getpara1_unsignedint(g_spi_data) < 2)
    	return 1;

    FadeOutTime = 0.1 * getpara1_unsignedint(g_spi_data);
	FadeOutTimePerFrame = 4.0e-4 * NUM_SAMPLES / inputparam[InputXtype].SamplingRate / FadeOutTime;

	DEBUG_SPI_CODE("FadeOutTime:%f\n",FadeOutTime);
 	DEBUG_SPI_CODE("FadeOutTimePerFrame:%f\n",FadeOutTimePerFrame);
	
	
    return 0;
}


/*
1.12 ��������ʽ
SurroundType	0	0
*/
int instructiondecoder_SurroundType_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 5)
    	return 1;

    SurroundType = getpara1_unsignedint(g_spi_data);

   	DEBUG_SPI_CODE("SurroundType:0x%08x\n",SurroundType);
 
    
    return 0;
}


/*
1.13 ����ӿڹܽŷ���
channelp	channely	0
*/
int instructiondecoder_OutPinAssign_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
       	return 1;

	if(getpara1_unsignedint(g_spi_data) > 24 || getpara1_unsignedint(g_spi_data) == 0)
    		return 1;

	if(getpara2_unsignedint(g_spi_data) > 24 || getpara2_unsignedint(g_spi_data) == 0)
    		return 1;

    	//0~23
    	Matrix_ChtoOutch[getpara1_unsignedint(g_spi_data)-1][getpara2_unsignedint(g_spi_data)-1] = getpara3_unsignedint(g_spi_data);

	DEBUG_SPI_CODE("Matrix_ChtoOutch[%d][%d]:0x%08x\n",
   		getpara1_unsignedint(g_spi_data)-1,
   		getpara2_unsignedint(g_spi_data)-1,
   		Matrix_ChtoOutch[getpara1_unsignedint(g_spi_data)-1][getpara2_unsignedint(g_spi_data)-1]);

	return 0;
}


/*
1.14 �������״̬
MonitorOutState	0	0
*/
int instructiondecoder_MonitorOutState_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 1)
    	return 1;

    MonitorOutState = getpara1_unsignedint(g_spi_data);

   	DEBUG_SPI_CODE("MonitorOutState:0x%08x\n",MonitorOutState);
 
    
    return 0;
}


/*
1.15 ����
Mute	0	0

E1 ����
E0 ����

*/
int instructiondecoder_Mute_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 1)
    	return 1;

    Mute = getpara1_unsignedint(g_spi_data);

   // 	printf("FadeInCoeff = %d,FadeInFlag = %d,outpcmDAdatabuf[0][0]=%d\n",FadeInCoeff,FadeInFlag,outpcmDAdatabuf[0][0]);
   //	printf("FadeOutCoeff = %d,FadeOutFlag = %d,outpcmDAdatabuf[0][0]=%d\n",FadeOutCoeff,FadeOutFlag,outpcmDAdatabuf[0][0]);

    if (Mute == 1)
	{
		FadeInFlag = 1;
		FadeInCoeff = 0;
	}
	else
	{
		FadeOutFlag = 1;
		FadeOutCoeff = 1;
		
	}

   	DEBUG_SPI_CODE("Mute:0x%08x\n",Mute);
	
//	printf("---------mute:%d--------\n",Mute);//added by wit_yuan 2016-09-20  mute 1
//	printf("FadeOutTimePerFrame = %f\n",FadeOutTimePerFrame);
    return 0;
}

 
#if 0
/*
1.12 ϵͳ����
System_Reset_Set
*/
int instructiondecoder_System_Reset_Set()
{
    SystemControlState = 3;
    return 0;
}

/*
1.13 ����
System_Start_Set
*/
int instructiondecoder_System_Start_Set()
{
    SystemControlState = 0;
    return 0;
}

/*
1.14 ��ͣ
System_Pause_Set
*/
int instructiondecoder_System_Pause_Set()
{
    SystemControlState = 2;
    return 0;
}

/*
1.15 ֹͣ
System_Stop_Set
*/
int instructiondecoder_System_Stop_Set()
{
    SystemControlState = 1;
    return 0;
}
#endif

///////////////////////// ����ӿ�//////////////////////////////////////
/*
2.1-8.1 ��Ƶ������
SamplingRate	0	0
*/
int instructiondecoder_Input_SamplingRate_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    int InputType = getinstructiongroup(g_spi_data) - 1;

	switch(getpara1_unsignedint(g_spi_data))
    {
    case 1:
        inputparam[InputType].SamplingRate = 44100;
        break;
    case 2:
        inputparam[InputType].SamplingRate = 48000;
        break;
    case 3:
        inputparam[InputType].SamplingRate = 88200;
        break;
    case 4:
        inputparam[InputType].SamplingRate = 96000;
        break;
    default:
        inputparam[InputType].SamplingRate = 48000;
        break;
    }
	DEBUG_SPI_CODE("inputparam[%d].SamplingRate:%d\n",
			InputType,
			
			inputparam[InputType].SamplingRate);

    return 0;
}


/*
2.2-8.2 ��Ƶѹ����ʽ
Decoder	0	0
*/
int instructiondecoder_Input_Decoder_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;

    int InputType = getinstructiongroup(g_spi_data) - 1;

    inputparam[InputType].Decoder = getpara1_unsignedint(g_spi_data);

	DEBUG_SPI_CODE("inputparam[%d].Decoder:0x%08x\n",
			InputType,
			inputparam[InputType].Decoder);

    return 0;
}


/*
2.3-8.3 ������
ChannelNumber	0	0
*/
int instructiondecoder_Input_ChannelNumber_Set()
{
	if(getinstructiontype(g_spi_data) != 0)
        return 1;

    int InputType = getinstructiongroup(g_spi_data) - 1;

    inputparam[InputType].ChannelNumber = getpara1_unsignedint(g_spi_data);
 
	DEBUG_SPI_CODE("inputparam[%d].ChannelNumber:%d\n",
			InputType,
			inputparam[InputType].ChannelNumber);
  
    return 0;
}


/*
2.4-8.4 ��������
Channelp	Channelx	0
*/
int instructiondecoder_Input_PinAssign_Set()
{
    return 0;
}


/*
2.5-8.5 ������ʱ
0	Delay	0
*/
int instructiondecoder_Input_TimeDelay_Set()
{
    return 0;
}


/*
2.6-8.6 �ط�����
PlaybackSF	0	0
*/
int instructiondecoder_Input_PlaybackSF_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 24)
    	return 1;

    int InputType = getinstructiongroup(g_spi_data) - 1;

    inputparam[1].PlaybackSF = getpara1_unsignedint(g_spi_data);
 
	DEBUG_SPI_CODE("inputparam[1].PlaybackSF:0x%08x\n",inputparam[1].PlaybackSF);
  
       	
    return 0;
}


///////////////////////// ����ӿ�����//////////////////////////////////////
/*
11.1 ��Ƶ��
FreqIndex	FreqDivVal	0
*/
int instructiondecoder_Output_FreqDivision_Set()
{
	DEBUG_SPI_CODE("FreqDivision_Set null\n");
 
    return 0;
}

/*
11.2 ��ƽ����
Channel	Y_ChannelGain	0
*/
int instructiondecoder_Output_ChannelGain_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    if(getpara1_unsignedint(g_spi_data) > 30 || getpara1_unsignedint(g_spi_data) == 0)
		return 1;

	//if (getpara2_signedint(GetRXSPIB) > 120 || getpara2_signedint(GetRXSPIB) < -120)
	//	return 1;
		
	unsigned int channely = getpara1_unsignedint(g_spi_data);
	
	outputparam[OutputYtype].ChannelGain[channely - 1] = 1.0 * getpara2_signedint(g_spi_data) / 10;
	outputparam[OutputYtype].ChannelGain_Val[channely - 1] = powf(10, outputparam[OutputYtype].ChannelGain[channely - 1] / 20.0);

	DEBUG_SPI_CODE("outputparam[%d].ChannelGain[%d]:%f\n",
				OutputYtype,channely - 1,
				outputparam[OutputYtype].ChannelGain[channely - 1]);
	
	
	DEBUG_SPI_CODE("outputparam[%d].ChannelGain_Val[%d]:%f\n",
				OutputYtype,
				channely - 1,	
				outputparam[OutputYtype].ChannelGain_Val[channely - 1]);
  	  	
	
    return 0;
}

/*
11.3 ��ʱ
Channel	Y_Delay	0
*/
int instructiondecoder_Output_TimeDelay_Set()
{
    if (getinstructiontype(g_spi_data) != 0)
        return 1;

    if (getpara1_unsignedint(g_spi_data) > 30)
		return 1;

	if (getpara2_unsignedint(g_spi_data) > 200)
		return 1;
		
	if (getpara3_unsignedint(g_spi_data) > 99)
		return 1;
		
	unsigned int channely = getpara1_unsignedint(g_spi_data);
	
    outputparam[OutputYtype].Delay[channely - 1] = getpara2_unsignedint(g_spi_data) + 0.01 * getpara3_unsignedint(g_spi_data);
    
    unsigned int val = inputparam[InputXtype].SamplingRate * outputparam[OutputYtype].Delay[channely - 1] / 1000;

    if(val > DelayBufSIZE  - FFTSIZE/2)
    	return 1;
    
    Delaytime_samplenums[channely - 1] = val;
 
	DEBUG_SPI_CODE(" Delaytime_samplenums[%d]:0x%08x,outputparam[%d].Delay[%d]:0x%08x,inputparam[%d].SamplingRate:%d\n", 
		channely - 1,
		Delaytime_samplenums[channely - 1],
		OutputYtype,
		channely - 1,
		outputparam[OutputYtype].Delay[channely - 1],
		InputXtype,
		inputparam[InputXtype].SamplingRate
		);
          
    return 0;
}


/*
11.4 ����Ƶ��
Channel	Y_HighCutFreq	0
*/
int instructiondecoder_Output_HighCutFreq_Set()
{
	if (getinstructiontype(g_spi_data) != 0)
        return 1;

    if (getpara1_unsignedint(g_spi_data) > 30)
		return 1;
	unsigned int channely = getpara1_unsignedint(g_spi_data);

	unsigned int val = (getpara2_unsignedint(g_spi_data)<<8) + getpara3_unsignedint(g_spi_data);
	
	if (val > 20000 || val < 20)
		return 1;
				
	if (outputparam[OutputYtype].LowCutFreq[channely - 1] > val)
		return 1;
	 
	outputparam[OutputYtype].HighCutFreq[channely - 1] = val;
	
	SPI_set_EQ_LPfilter(channely - 1);

	DEBUG_SPI_CODE(" outputparam[%d].HighCutFreq[%d]:0x%08x\n", 
		OutputYtype,
		channely - 1,
		outputparam[OutputYtype].HighCutFreq[channely - 1]);
      
	
				
    return 0;
}


/*
11.5 ����Ƶ��
Channel	Y_LowCutFreq	0
*/
int instructiondecoder_Output_LowCutFreq_Set()
{
	if (getinstructiontype(g_spi_data) != 0)
        return 1;

    if (getpara1_unsignedint(g_spi_data) > 30)
		return 1;
	unsigned int channely = getpara1_unsignedint(g_spi_data);

	unsigned int val = (getpara2_unsignedint(g_spi_data)<<8) + getpara3_unsignedint(g_spi_data);
	
	if (val > 20000 || val < 20)
		return 1;
			
	if (outputparam[OutputYtype].HighCutFreq[channely - 1] < val)
		return 1;
		
	outputparam[OutputYtype].LowCutFreq[channely - 1] = val;
 
	SPI_set_EQ_HPfilter(channely - 1);
	DEBUG_SPI_CODE(" outputparam[%d].LowCutFreq[%d]:0x%08x\n", 
			OutputYtype,
			channely - 1,
			outputparam[OutputYtype].LowCutFreq[channely - 1]);
      	
    return 0;
}


/*
11.6 ����б��
Channel	Y_HighCutSlope	0
*/
int instructiondecoder_Output_HighCutSlope_Set()
{
	if (getinstructiontype(g_spi_data) != 0)
        return 1;

    if (getpara1_unsignedint(g_spi_data) > 30)
		return 1;

	if (getpara2_unsignedint(g_spi_data) > 8)
		return 1;
	
	unsigned int channely = getpara1_unsignedint(g_spi_data);
	
	outputparam[OutputYtype].HighCutSlope[channely - 1] = getpara2_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" outputparam[%d].HighCutSlope[%d]:0x%08x\n", 
			OutputYtype,
			channely - 1,
			outputparam[OutputYtype].HighCutSlope[channely - 1]);
     	
    return 0;
}


/*
11.7 ����б��
Channel	Y_LowCutSlope	0
*/
int instructiondecoder_Output_LowCutSlope_Set()
{
	if (getinstructiontype(g_spi_data) != 0)
        return 1;

    if (getpara1_unsignedint(g_spi_data) > 30)
		return 1;

	if (getpara2_unsignedint(g_spi_data) > 8)
		return 1;
	
	unsigned int channely = getpara1_unsignedint(g_spi_data);
	
	outputparam[OutputYtype].LowCutSlope[channely - 1] = getpara2_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" outputparam[%d].LowCutSlope[%d]:0x%08x\n",
			OutputYtype,
			channely - 1,
			outputparam[OutputYtype].LowCutSlope[channely - 1]);
  	
    return 0;
}


/*
11.8 Ƶ��ϸ��
Channel	Band	Y_EQBandGain
*/
int instructiondecoder_Output_EQBandGain_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
        
    if(getpara1_unsignedint(g_spi_data) > 30)
		return 1;
		
    if(getpara2_unsignedint(g_spi_data) > 31)
		return 1;

    if(getpara3_signedint(g_spi_data) > 120 || getpara3_signedint(g_spi_data) < -120)
		return 1;
		
	unsigned int channely = getpara1_unsignedint(g_spi_data);
    unsigned int bandz = getpara2_unsignedint(g_spi_data);
    
	if(getpara2_unsignedint(g_spi_data) == 0 && getpara3_unsignedint(g_spi_data) == 0)
    {
    	int i;
    	for(i = 0; i < 31; i++)
    	{
    		outputparam[OutputYtype].EQBandGain[channely - 1][i] = 0;
    		outputparam[OutputYtype].EQBandGain_Val[channely - 1][i] = 1;
    	}
    	return 0;
    }
    
    outputparam[OutputYtype].EQBandGain[channely - 1][bandz - 1] = getpara3_signedint(g_spi_data) / 10;
    outputparam[OutputYtype].EQBandGain_Val[channely - 1][bandz - 1] = powf(10, getpara3_signedint(g_spi_data) / 20.0);

	DEBUG_SPI_CODE("outputparam[%d].EQBandGain[%d][%d]:0x%08x\n",
			OutputYtype,
			channely - 1,
			bandz - 1,	
			outputparam[OutputYtype].EQBandGain[channely - 1][bandz - 1] );
	DEBUG_SPI_CODE("outputparam[%d].EQBandGain_Val[%d][%d]:0x%08x\n",
			OutputYtype,
			channely - 1,
			bandz - 1,
			outputparam[OutputYtype].EQBandGain_Val[channely - 1][bandz - 1] );
        
    
    return 0;
}


///////////////////////// ��������������//////////////////////////////////////
/*
13.1 �ⲿ��ѹ����ֵ
SPLreferencevalue_Set 0(��һ����) SPLreferencevalue
*/
int instructiondecoder_SPLreferencevalue_Set()
{
    return 0;
}


/*
13.2 У׼�����ѹ����ֵ
SPLalignmentvalue_Set 0(��һ����) SPLalignmentvalue
*/
int instructiondecoder_SPLalignmentvalue_Set()
{
    return 0;
}


/*
13.3 ���е�ƽ
0	Cutlevelchannely	0
*/
int instructiondecoder_CutLevel_Set()
{
	return 0;
}


/*
13.4 �ص������������Լ��ʹ������
PL_Alignment_Enable_Set plAlignmentenablevalue
*/
int instructiondecoder_PL_Alignment_Enable_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    //DSP23����
    if(getpara1_unsignedint(g_spi_data) > 1)
		return 1;
		
    plAlignmentenablevalue = getpara1_unsignedint(g_spi_data);
    
    
 	DEBUG_SPI_CODE(" plAlignmentenablevalue:0x%08x\n", plAlignmentenablevalue);
    
    

    return 0;
}

/*
13.5 �����������ɲ����ź�ʹ������
CenterSignal_Enable_Set centersignalenablevalue
*/
int instructiondecoder_CenterSignal_Enable_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

	//DSP23����
    if(getpara1_unsignedint(g_spi_data) > 1)
    	return 1;
    
    centersignalenablevalue = getpara1_unsignedint(g_spi_data);

 	DEBUG_SPI_CODE(" centersignalenablevalue:0x%08x\n", centersignalenablevalue);
    
    
    return 0;
}

/*
13.6 ��������
Invert_Enable_Set invertenablevalue
*/
int instructiondecoder_Invert_Enable_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    //DSP23����
    if(getpara1_unsignedint(g_spi_data) > 1)
		return 1;
		
    invertenablevalue = getpara1_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" invertenablevalue:0x%08x\n", invertenablevalue);
   
    return 0;
}

/*
13.7 ���Լ��Ƶ������
PLfilter_Set plfilterfrenquency
*/
int instructiondecoder_PLfilter_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

	if(getinstructiontype(g_spi_data) != 0)
        return 1;

	//DSP23����
    if(getpara1_unsignedint(g_spi_data) > 1)
		return 1;
		
    plfilterfrenquency = (getpara1_unsignedint(g_spi_data) == 0) ? 50 : 100;
	DEBUG_SPI_CODE(" plfilterfrenquency:0x%08x\n", plfilterfrenquency);
 
    return 0;
}


/*
13.9 ��ǰУ׼����
AlignmentChannel	0	0
*/
int instructiondecoder_AlignmentChannel_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    //DSP23����
    if(getpara1_unsignedint(g_spi_data) > 30)
		return 1;
		
    AlignmentChannel = getpara1_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" AlignmentChannel:0x%08x\n", AlignmentChannel);
 	
    return 0;
}

/*
13.10 �źŷ�����״̬
SignalModeVal	0	0
*/
int instructiondecoder_SignalModeVal_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    //DSP23����
    if(getpara1_unsignedint(g_spi_data) > 3)
		return 1;
		
    SignalModeVal = getpara1_unsignedint(g_spi_data);
    DEBUG_SPI_CODE(" SignalModeVal:0x%08x\n", SignalModeVal);
 
    return 0;
}

/*
13.11 ����״̬
SystemState_Set SystemState
*/
int instructiondecoder_SystemState_Set()
{
    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    if(getpara1_unsignedint(g_spi_data) > 3 || getpara1_unsignedint(g_spi_data) == 0)
    	return 1;
    	
    SystemState = getpara1_unsignedint(g_spi_data);
    

    
    //if(InitDone == 1)
    {
    	if(SystemState == 2)
    	{
    		LastInputXtype = InputXtype;
    		InputXtype = 6;
    	}
    	else
    		InputXtype = LastInputXtype;
    }
	DEBUG_SPI_CODE(" SystemState:0x%08x\n", SystemState);
 
    return 0;
}


/*
13.13 ��Ƶ���������
OutputVolume_Set channel channelvolume
*/
int instructiondecoder_OutputVolume_Set()
{	
    if(getinstructiontype(g_spi_data) != 0)
        return 1;
    
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);
    unsigned int channelx;

    //DSP23����
    if(getpara1_unsignedint(g_spi_data) > 24 || getpara1_unsignedint(g_spi_data) == 0)
    	return 1;
    	
    channelx = getpara1_unsignedint(g_spi_data);

    if(getpara2_unsignedint(g_spi_data) > 140)
		return 1;
		
    OutputVolume[channelx - 1] = getpara2_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" OutputVolume[channelx - 1]:0x%08x\n", OutputVolume[channelx - 1]);
 
    return 0;
}

/*
13.15 �������ĸ�Ƶ������
MicinBandeng_Set bandx Bandeng
*/
int instructiondecoder_MicinBandeng_Set()
{
    return 0;
}

/*
13.16 ��Ƶ����ӿڵĸ�����������
InputVolume_Set channel InputVolume
*/
int instructiondecoder_InputVolume_Set()
{
    return 0;
}

/////////////////////////����������������//////////////////////////////////////
/*
14.8 DJ��Դλ��(XY)
DJsourceXY_Set 0(��һ����) DJsourceX DJsourceY
*/
int instructiondecoder_DJsourceXY_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    DJsourceX = getpara2_unsignedint(g_spi_data);
    DJsourceY = getpara3_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" DJsourceX:0x%08x,DJsourceY:0x%08x\n",DJsourceX,DJsourceY);
 
    return 0;
}

/*
14.9 DJ��Դλ��(Z)
DJsourceZ_Set DJsourceZ
*/
int instructiondecoder_DJsourceZ_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    DJsourceZ = getpara1_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" DJsourceZ:0x%08x\n",DJsourceZ);
 
    return 0;
}

/*
14.10 DJ������
DJgain_Set  0  DJgain
*/
int instructiondecoder_DJgain_Set()
{
    unsigned int instructiongroup = getinstructiongroup(g_spi_data);

    if(getinstructiontype(g_spi_data) != 0)
        return 1;

    DJgain = getpara1_unsignedint(g_spi_data);
	DEBUG_SPI_CODE(" DJgain:0x%08x\n",DJgain);
 
    return 0;
}


//15
int instructiondecoder_FreqDiv_Set()
{
	int row = getinstructionindexingroup(g_spi_data);
	if(row > MAXCAHNNELNUM)
		return -1;
	int col = getpara1_unsignedint(g_spi_data);
	int val = (getpara2_unsignedint(g_spi_data)<<8) + getpara3_unsignedint(g_spi_data);
	
	if(col <= 3)
	{
		outputparam[OutputYtype].FreqDivVal[row-1][col-1] = val;
		DEBUG_SPI_CODE(" outputparam[%d].FreqDivVal[%d][%d]:%d \n",
				OutputYtype,row-1,col-1,outputparam[OutputYtype].FreqDivVal[row-1][col-1]);
	}
	else
	{
		outsignalchannel_book24m3[row-1][col-4] = val - 1;	
		DEBUG_SPI_CODE(" outsignalchannel_book24m3[%d][%d]:%d \n",row-1,col-4,outsignalchannel_book24m3[row-1][col-4]);
	}
	
	if(outputparam[OutputYtype].FreqDivVal[row-1][0] == 1 
	|| outputparam[OutputYtype].FreqDivVal[row-1][0] == 2
			&& outputparam[OutputYtype].FreqDivVal[row-1][1] >= 20 && outputparam[OutputYtype].FreqDivVal[row-1][1] <= 20000			
	|| outputparam[OutputYtype].FreqDivVal[row-1][0] == 3
			&& outputparam[OutputYtype].FreqDivVal[row-1][1] >= 20 && outputparam[OutputYtype].FreqDivVal[row-1][1] <= 20000	
			&& outputparam[OutputYtype].FreqDivVal[row-1][2] >= 20 && outputparam[OutputYtype].FreqDivVal[row-1][2] <= 20000			
			)
		sim_frequencydivision_setdivisionpointnum(row-1, outputparam[OutputYtype].FreqDivVal[row-1][0]);	
	return 0;
}


//�����ຯ������
int (*instructiondecoder[7][17])(void) =
{
    {NULL},
    {
        NULL,
        instructiondecoder_SoundFieldX_Set, 	//1.1
        instructiondecoder_SoundFieldY_Set, 	//1.2
		instructiondecoder_SoundFieldZ_Set,		//1.3
        instructiondecoder_SpeakerNumber_Set,	//1.4
        instructiondecoder_SpeakerX_Set,		//1.5
        instructiondecoder_SpeakerY_Set,		//1.6
        instructiondecoder_SpeakerZ_Set,		//1.7
        instructiondecoder_KnobVolume_Set,		//1.8
        instructiondecoder_InputXtype_Set,		//1.9
        instructiondecoder_FadeInTime_Set,		//1.10
        instructiondecoder_FadeOutTime_Set,		//1.11
        instructiondecoder_SurroundType_Set,	//1.12
        instructiondecoder_OutPinAssign_Set,	//1.13
        instructiondecoder_MonitorOutState_Set,	//1.14
		instructiondecoder_Mute_Set,			//1.15
		NULL,// 16	
    },
    {
        NULL,
        instructiondecoder_Input_SamplingRate_Set,	//2.1-8.1
        instructiondecoder_Input_Decoder_Set,		//2.2-8.2
        instructiondecoder_Input_ChannelNumber_Set,	//2.3-8.3
        instructiondecoder_Input_PinAssign_Set,		//2.4-8.4
        instructiondecoder_Input_TimeDelay_Set,		//2.5-8.5
        instructiondecoder_Input_PlaybackSF_Set,	//2.6-8.6
    },
    {
        NULL,
        instructiondecoder_Output_FreqDivision_Set,	//9.1-12.1
        instructiondecoder_Output_ChannelGain_Set,	//9.2-12.2
        instructiondecoder_Output_TimeDelay_Set,	//9.3-12.3
        instructiondecoder_Output_HighCutFreq_Set,	//9.4-12.4
        instructiondecoder_Output_LowCutFreq_Set,	//9.5-12.5
        instructiondecoder_Output_HighCutSlope_Set,	//9.6-12.6
        instructiondecoder_Output_LowCutSlope_Set,	//9.7-12.7
        instructiondecoder_Output_EQBandGain_Set,	//9.8-12.8
    },
    {
        NULL,
        instructiondecoder_SPLreferencevalue_Set,	//13.1
        instructiondecoder_SPLalignmentvalue_Set,	//13.2
        instructiondecoder_CutLevel_Set,			//13.3
        instructiondecoder_PL_Alignment_Enable_Set,	//13.4
        instructiondecoder_CenterSignal_Enable_Set,	//13.5
        instructiondecoder_Invert_Enable_Set,		//13.6
        instructiondecoder_PLfilter_Set,			//13.7
        NULL,//13.8
        instructiondecoder_AlignmentChannel_Set,	//13.9
        instructiondecoder_SignalModeVal_Set,		//13.10
        instructiondecoder_SystemState_Set,			//13.11
        NULL,//13.12
        instructiondecoder_OutputVolume_Set,		//13.13
        NULL,//13.14
        instructiondecoder_MicinBandeng_Set,		//13.15
        instructiondecoder_InputVolume_Set			//13.16
    },
	{
        instructiondecoder_DJsourceXY_Set,		//14.8
        instructiondecoder_DJsourceZ_Set,		//14.9
        instructiondecoder_DJgain_Set			//14.10
    },
    {
    	instructiondecoder_FreqDiv_Set			//15
    }
};

