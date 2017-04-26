#ifndef _ezkit_21489_h_
#define _ezkit_21489_h_
///////////////////////////////////////////////////////////////////////////////////////
//
//NAME:     ADDS_21489_EzKit.h
//DATE:     02/06/10
//PURPOSE:  Header file with definitions use in the C-based talkthrough examples
//
////////////////////////////////////////////////////////////////////////////////////////
#include <signal.h>
#include <cdef21489.h>
#include <def21489.h>

#define USE_WRITE_FILE 0     
#define INTERRUPT_TIMES 100  
 
// Function prototypes for this talkthrough code
void initPLL_SDRAM(void);
void clearDAIpins(void);
void InitDAI(void);
void InitSPORT(int);
int PLL_Setting(int);

void PCG_Init_GLOBAL();

//void ProccessingTooLong(void);
void TalkThroughISR_SPORT0(int);
void TalkThroughISR_SPORT1(int);



void SP0ADCRcv_ISR(int sig_int);
void SP0SPDIFRcv_ISR(int sig_int);

int SP0ADCRcv_process(void);

int SP0AESEBURcv_process(void);
int SP0SPDIFRcv_process(void);
int SP0Alignment_process(void);

void SP0ADCSend_ISR(int sig_int);//SPORT6用来给AD输入数据的
void SP2ADCRcv_ISR(int sig_int);
void SP3ADCRcv_ISR(int sig_int);
void SP4ADCRcv_ISR(int sig_int);

void SP1ADCSend_ISR(int sig_int);
void SP6ADCRcv_ISR(int sig_int);

void SPIH_ISR(int sig_int);



#define CIRCLEDMADATABUFFNUM 	4
#define NUM_SAMPLES  256//((256+128) * 1) //此数为48的倍数
//(100 * 48)//2560
#define NUM_RX_SLOTS 1
#define NUM_TX_SLOTS NUM_RX_SLOTS
//24

#define PI 3.14159265358979323846

#define NUMDMAUSED 4									//DMA buffer block used for receive and tranmit data. receive block and transmit block num are same 
#define CIRCLEDAMDATABUFFNUM 4									
#define DECODEDPCMDATABUFFNUM (CIRCLEDAMDATABUFFNUM*4*1)

#define USE_VOLUME_STEP  0x60 //声音调度步长
//////////////////////////////////////////////////////////////////////////////////////////////

//信号发生器信号类型
#define WhiteNoise 1
#define PinkNoise 2
#define TriPulse 3


#define  DelayBufSIZE (FFTSIZE*20)
#define FFTSIZE (NUM_SAMPLES*NUM_TX_SLOTS)


//系统状态
#define SYSTEMSTATE_NORMAL 		1
#define SYSTEMSTATE_ALIGNMENT 	2
#define SYSTEMSTATE_PUB 			3

//程序系统运行标志
#define SYSTEM_CHANGING_INIT    0
#define SYSTEM_CHANGING             1
#define SYSTEM_CHANGING_DONE   2

extern unsigned int SystemControlState;
extern unsigned int GetRXSPIB;
extern unsigned int GetRXSPI;

/////////////通用参数//////////
extern unsigned int SoundFieldX;						//1.1
extern unsigned int SoundFieldY;						//1.2
extern unsigned int SoundFieldZ;						//1.3
extern unsigned int SpeakerNumber;						//1.4
extern unsigned int SpeakerX[24];						//1.5
extern unsigned int SpeakerY[24];						//1.6
extern unsigned int SpeakerZ[24];						//1.7
extern float knobvolume;								//1.8
extern float knobvolumegainvalue;
extern unsigned int InputXtype;							//1.9
extern float FadeInTime;								//1.10
extern float FadeOutTime;								//1.11
extern unsigned int SurroundType;						//1.12
extern unsigned int Matrix_ChtoOutch[24][24];			//1.13
extern unsigned int MonitorOutState;					//1.14
extern unsigned int Mute;								//1.15

/////////////////////////输入参数////////////////////////////
struct InputParams
{
    unsigned int SamplingRate;		//2.1-8.1
    unsigned int Decoder;			//2.2-8.2
    unsigned int ChannelNumber;		//2.3-8.3
    unsigned int PinAssignList[8];	//2.4-8.4
    unsigned int Delay;				//2.5-8.5
    unsigned int PlaybackSF;		//2.6-8.6
};



//////////////////////////输出参数/////////////////// 2016-11-07 根据黄的说法，做一个修改////////////////////////
#if 0
struct OutputParams{
	unsigned int FreqDivVal[24][3];				//9.2-12.2
	float ChannelGain[30];					//9.3-12.3
	float ChannelGain_Val[30];
	float Delay[24];				//9.4-12.4
	float HighCutFreq[24];		//9.6-12.6
	float HighCutSlope[24];
	float LowCutFreq[24];		//9.7-12.7
	float LowCutSlope[24];
	float EQBandGain[30][33];	//9.8-12.8
	float EQBandGain_Val[30][33];
};

#else

struct OutputParams{
	unsigned int FreqDivVal[30][3];				//9.2-12.2
	float ChannelGain[30];					//9.3-12.3
	float ChannelGain_Val[30];
	float Delay[30];				//9.4-12.4
	float HighCutFreq[30];		//9.6-12.6
	float HighCutSlope[30];
	float LowCutFreq[30];		//9.7-12.7
	float LowCutSlope[30];
	float EQBandGain[30][33];	//9.8-12.8
	float EQBandGain_Val[30][33];
};
#endif


extern struct InputParams *inputparam;
extern struct OutputParams *outputparam;
	
/////////辅助参数///////
extern unsigned int SPLreferencevalue;		//13.1
extern unsigned int SPLalignmentvalue;		//13.2
extern unsigned int SPLoffset;
extern unsigned int Cutlevelchannely;		//13.3
extern unsigned int plAlignmentenablevalue;	//13.4
extern unsigned int centersignalenablevalue;//13.5
extern unsigned int invertenablevalue;		//13.6
extern unsigned int plfilterfrenquency;		//13.7
//13.8
extern unsigned int AlignmentChannel;		//13.9
extern unsigned int SignalModeVal;			//13.10
extern unsigned int SystemState;			//13.11
//13.12
extern float OutputVolume[24];				//13.13
extern unsigned int OutputYtype;

extern int DSP23_OutputYtype;
/////////声场测量参数///////
extern unsigned int DJsourceX;		//14.8
extern unsigned int DJsourceY;		//14.8
extern unsigned int DJsourceZ;		//14.9
extern unsigned int DJgain;			//14.10


//指令解析
unsigned int getinstructiontype(unsigned int);
unsigned int getinstructiongroup(unsigned int);
unsigned int getinstructionindexingroup(unsigned int);
unsigned int getpara1_unsignedint(unsigned int);
unsigned int getpara2_unsignedint(unsigned int); 
int getpara2_signedint(unsigned int);
unsigned int getpara3_unsignedint(unsigned int);
int getpara3_signedint(unsigned int);

//指令构造
/*
31	30  |29	28	27	26|	25	24	23	22	21|	20	19	18	17	16|	15	14	13	12	11	10	09	08|	07	06	05	04	03	02	01	00
命令类型|	命令组别  |	    组内序号      |	     第一参数     |	          第二参数            |	          第三参数
*/
//指令编码
unsigned int instruction_encoder(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

//设置类函数集合
int (*instructiondecoder[7][17])(void);



//旋钮音量调节  -90dB 到 6dB

float FadeInTimePerFrame, FadeOutTimePerFrame;
int FadeInFlag, FadeOutFlag;
float FadeInCoeff, FadeOutCoeff;


#define MAXROUTENUM 16
#define MAXCAHNNELNUM  16


#define MAX_FREQUNCYDIVISION_BANDNUM 3
#define MAXDIVISIONPOINT 2


#define OUTPUT_CHANNELS 14  //2016-11-24 袁兆铭  输出声道数量


extern int *p_outchanneloutputbuf[MAXROUTENUM];//矩阵映射后输出声道的数据的指针;
extern int *outpcmDAdatabuf[MAXROUTENUM];

#define USE_ALGORITHM 1


extern  int RxBlock_SPDIF[MAXCAHNNELNUM][CIRCLEDAMDATABUFFNUM * NUM_SAMPLES * NUM_TX_SLOTS];

extern void PCG_Init_GLOBAL(void);

#endif

