#include "./include/dsp_signal.h"
#include "./include/ADDS_21489_EzKit.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*****************************************************************************
 * wavarts_DSP23.c
  信号发生器：testsignal_generator_oneframe()
  类型：白噪声，粉红噪声，100Hz三角脉冲
 *****************************************************************************/
unsigned int SystemControlState;

/////////////通用参数//////////
unsigned int SoundFieldX;			//1.1
unsigned int SoundFieldY;			//1.2
unsigned int SoundFieldZ;			//1.3
unsigned int SpeakerNumber;			//1.4
unsigned int SpeakerX[24];			//1.5
unsigned int SpeakerY[24];			//1.6
unsigned int SpeakerZ[24];			//1.7
float knobvolume;					//1.8
unsigned int InputXtype;			//1.9
float FadeInTime;					//1.10
float FadeOutTime;					//1.11
unsigned int SurroundType;			//1.12
unsigned int Matrix_ChtoOutch[24][24];	//1.13
unsigned int MonitorOutState;		//1.14
unsigned int Mute;					//1.15

/////////////输入输出//////////
section("seg_ext_dmda") struct InputParams InputParam[8];
section("seg_ext_dmda") struct OutputParams OutputParam[5];
struct InputParams *inputparam = InputParam;
struct OutputParams *outputparam = OutputParam;

	
/////////辅助参数///////
unsigned int SPLreferencevalue;			//13.1
unsigned int SPLalignmentvalue;			//13.2
unsigned int SPLoffset;
unsigned int Cutlevelchannely;			//13.3
unsigned int plAlignmentenablevalue;	//13.4
unsigned int centersignalenablevalue;	//13.5
unsigned int invertenablevalue;			//13.6
unsigned int plfilterfrenquency;		//13.7
//13.8
unsigned int AlignmentChannel;			//13.9
unsigned int SignalModeVal;				//13.10
unsigned int SystemState;				//13.11
//13.12
float OutputVolume[24];					//13.13
unsigned int OutputYtype;

/////////声场测量参数///////
unsigned int DJsourceX;					//14.8
unsigned int DJsourceY;					//14.8
unsigned int DJsourceZ;					//14.9
unsigned int DJgain;					//14.10

float Xprev1 = 0, Xprev2 = 0, Xprev3 = 0, Yprev1 = 0, Yprev2 = 0, Yprev3 = 0;
float Xprevv1 = 0, Xprevv2 = 0, Xprevv3 = 0, Yprevv1 = 0, Yprevv2 = 0, Yprevv3 = 0;



//DSP23算法处理参数
int DSP23_SystemState;
int DSP23_InputXtype;
int DSP23_OutputYtype;
int DSP23_samplingrate;
int DSP23_AlignmentChannel;

 

//信号发生器
int testsignal_generator_oneframe(int signaltype, int len, float *sigbuf, float gain)
{
    int i, j;
    //float a[4] = {0.4957526, -0.6305581, 0.148322};
    //float b[4] = {1, -1.801161, 0.802577};

	#if 0
    	for(i = 0; i < len; i++)
       	*(sigbuf + i) = sin( (1/48.0) * PI * i) ;// * (1 << 30);
	return;
	#endif


    	float K = tan(PI / 2.4);
    	float den = 1 / (1 + sqrtf(2) * K + K * K);

    	double a[4] = {1, -1.894043, 0.958564, -0.062132};//{0.49922035, -0.95993537, 0.50612699, -0.04408786};
    	double b[4] = {1, -2.479309, 1.985013, -0.5056};//{1, -2.494956002, 2.017265875, -0.522189400};

    	double z1 = 0.98443604, z2 = 0.83392334, z3 = 0.07568359;
    	double p1 = 0.99572754, p2 = 0.94790649, p3 = 0.53567505;

    	float lpfa[4], lpfb[4];
	lpfa[0] = K * den * K;
	lpfa[1] = 2 * lpfa[0];
	lpfa[2] = lpfa[0];
	lpfa[3] = 0;
	lpfb[0] = 1;
	lpfb[1] = lpfa[1] - 2 * den;
	lpfb[2] = 1 - sqrt(8) * K * den;
	lpfb[3] = 0;
	/*a[0] = 1;
	a[1] = -z1-z2-z3;
	a[2] = z1*z2+z2*z3+z1*z3;
	a[3] = -z1*z2*z3;
	b[0] = 1;
	b[1] = -p1-p2-p3;
	b[2] = p1*p2+p2*p3+p1*p3;
	b[3] = -p1*p2*p3;*/

	srand(rand());

	float tmp[len], tmp1[len];
    
    for(i = 0; i < len; i++)
        tmp[i] = (rand()*1.0/(1<<30)-1)/2;
	
    if(signaltype == WhiteNoise)   
    {
        for(i = 0; i < len; i++)
            *(sigbuf + i) = tmp[i];
    }
    else if(signaltype == PinkNoise) 
    {
	    //粉噪
	    for(i = 0; i < len; i++)
	    {
	        tmp1[i] = a[0] * tmp[i] + a[1] * Xprev1 + a[2] * Xprev2 + a[3] * Xprev3 - b[1] * Yprev1 - b[2] * Yprev2 - b[3] * Yprev3;
	        Xprev3 = Xprev2;
	        Yprev3 = Yprev2;
	        Xprev2 = Xprev1;
	        Yprev2 = Yprev1;
	        Xprev1 = tmp[i];
	        Yprev1 = tmp1[i];
	    }
	    for(i = 0; i < len; i++)
	        *(sigbuf + i) = tmp1[i];
    }
    else if(signaltype == TriPulse) 
    {       
        	/*100Hz 三角波*/
       	static int triangle_index = 0;
		int periodic_100Hz = 48000/100;
       	//int periodic100Hz = DSP23_samplingrate/100;
		for(i = 0; i < len; i++)
       	{ 	
       		*(sigbuf + i) = (triangle_index * 1.0)/ (periodic_100Hz);
            //*(sigbuf + i) = triangle_index / (periodic100Hz * 2.0);
            triangle_index = ( triangle_index + 1 ) % periodic_100Hz;
       	}
   }
   else
   {
   	for(i = 0; i < len; i++)
        	*(sigbuf + i) = 0;
    } 
    return 0;
}

 
 
 
 
 
 




