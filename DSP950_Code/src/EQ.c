#include "./include/EQ.h"
#include "./include/ADDS_21489_EzKit.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <complex.h>
#include <filter.h>
// 均衡处理函数
// Reference:Digital Audio Signal Processing 2nd Edition Aug2008DDU
#include <math.h>
#include <stdio.h>

float fs = (48000);

#define CHANNEL_OUTPUT_NUMBER 14

#define PI 3.14159265358979323846

#define EQ6b_ID_Max 		1
#define HPF_ID_Max 			1
#define Band_count_GEQ 		(31)
#define Channelnum_Max 		24	
#define Band_count_male 	(6)
#define MAXDIVISIONPOINT 	2
#define CHANNELS 			4
//#define FFTSIZE 			(NUM_SAMPLES*2)


//DSP23算法处理参数
int DSP23_SystemState;
int DSP23_InputXtype;
int DSP23_OutputYtype;
int DSP23_samplingrate;
int DSP23_AlignmentChannel;



/*********************用户可配置***************************/
//分频段数量, 与参数集中保持一致
int divisionbandnum_set[MAXCAHNNELNUM] = { 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//int divisionbandnum_set[24] ={2,2,2,2,2,2,2,2,2,2,2,2};
//int divisionbandnum_set[24] ={3,3,3,3,3,3,3,3,2,2,2};
//各声道的分频点, 与参数集中保持一致
float fc_division[MAXCAHNNELNUM][MAXDIVISIONPOINT] =
{
	{300,2000},
	{300,2000},
	{300,2000}, 
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
	{300,2000},
 };
 

float pm filter_cf_frequencydivion[Channelnum_Max *MAX_FREQUNCYDIVISION_BANDNUM][5 * MAXDIVISIONPOINT];
float dm filter_state_frequencydivion[Channelnum_Max *MAX_FREQUNCYDIVISION_BANDNUM][2 * MAXDIVISIONPOINT+1];
//第一个频段，只需要做高通；最后一个频段，只需要做低通；中间频段需要做一次低通再做一次高通


extern int outsignalchannel_book24m3[MAXCAHNNELNUM][3];

// Center Frequency
//float fc_EQ_normal[EQ6b_ID_Max][Band_count_normal];


//	Gain
//float gain_EQ_normal[EQ6b_ID_Max][Band_count_normal];


// 	Q
//float q_EQ_normal[EQ6b_ID_Max][Band_count_normal];

//  Array of filter coefficients
//float pm filter_cf_6b[EQ6b_ID_Max *Band_count_normal][5];



// Filter State Array
//float filter_state_6b[EQ6b_ID_Max *Band_count_normal][2 * Band_count_normal + 1];

// Center Frequency
float fc_EQ_GEQ[EQ6b_ID_Max][Band_count_GEQ];

//	Gain
float gain_EQ_GEQ[EQ6b_ID_Max][Band_count_GEQ];

// 	Q
float q_EQ_GEQ[EQ6b_ID_Max][Band_count_GEQ];

//Array of filter coefficients
float pm filter_cf_31b[Channelnum_Max *EQ6b_ID_Max][5 * Band_count_GEQ];
// Filter State Array
float dm filter_state_31b[Channelnum_Max *EQ6b_ID_Max][5 * Band_count_GEQ];

//Array of filter coefficients
float dm filter_state_HPF[HPF_ID_Max *Channelnum_Max][2 * 2 + 1];
// Filter State Array
float pm filter_cf_HPF[HPF_ID_Max *Channelnum_Max][5];

//Array of filter coefficients
//float filter_state_LFE_HPF[HPF_ID_Max *Channelnum_Max][2 * 1 + 1];
// Filter State Array
//float pm filter_cf_LFE_HPF[HPF_ID_Max *Channelnum_Max][5];

//Array of filter coefficients
float dm filter_state_LPF[HPF_ID_Max *Channelnum_Max][2 * 2 + 1];
// Filter State Array
float pm filter_cf_LPF[HPF_ID_Max *Channelnum_Max][5];

float filter_state_HPF_dl[HPF_ID_Max *Channelnum_Max][2 * 2 + 1];
float filter_cf_HPF_dl[HPF_ID_Max *Channelnum_Max][5];

//void 	init_EQ(void);
void    set_EQ_HPF_dl (int EQ_ID, float *filter_cf, float *fc);
void	set_EQ_High_Shelf (int EQ_ID, int Band_ID, int Band_count, float *filter_cf, float *fc, float *gain, float *q);
void	set_EQ_Low_Shelf  (int EQ_ID, int Band_ID, int Band_count, float *filter_cf, float *fc, float *gain, float *q);
void	set_EQ_Peak  (int EQ_ID, int Band_ID, int Band_count, float *filter_cf, float *fc, float *gain, float *q);
void    set_EQ_HPF (int EQ_ID, float *filter_cf, float *fc);



void	filter_process_HPF(float *EQ_In, float *EQ_Out);

float test_fc[] 	= {60, 300, 700, 1500, 10000};
float test_gain[] 	= { -6, -3, 6, -4, -6};
float test_q[] 		= {2, 4.32, 8, 4, 2};

// Centre Frequency of High Pass Filters
float fc_HPF[HPF_ID_Max] =
{
    //	80,80,
    //	112,112,
    //	332,332,
    //	100,100
    120//100//100
};
float fc_LFE_HPF[HPF_ID_Max] =
{
    //	80,80,
    //	112,112,
    //	332,332,
    //	100,100
    20//100
};

float fc_LPF[HPF_ID_Max] =
{
	20000//100
};

// Specifications of filters for Mic Main-processing path
float MicMain_fc[Band_count_male] =
{
    204,
    321,
    2500,
    5600,
    7400,
    8200,
};
float MicMain_gain[Band_count_male] =
{
    7.1,
    -3.0,
    2.1,
    0,
    0,
    2.6,
};
float MicMain_q[Band_count_male] =
{
    0.7,
    1.17,
    1,
    1.2,
    0.54,
    1,
};


float EQ_FR[3] = {17, 7, 0.1};


// Specifications of filters for 31 band EQ-processing path
float GEQ31band_fc[Band_count_GEQ] =
{
    20,//1
    25,//2
    31.5,//3
    40,//4
    50,//5
    63,//6
    80,//7
    100,//8
    125,//9
    160,//10
    200,//11
    250,//12
    315,//13
    400,//14
    500,//15
    630,//16
    800,//17
    1000,//18
    1250,//19
    1600,//20
    2000,//21
    2500,//22
    3150,//23
    4000,//24
    5000,//25
    6300,//26
    8000,//27
    10000,//28
    12500,//29
    16000,//30
    20000//31
};
float GEQ31band_gain[Band_count_GEQ] =
{
    0,
    0,
    0,
    0,//4
    0,
    0,
    0,
    0,//8
    0,
    0,
    0,
    0,//12
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,//
    0,
    0,
    0,
    0,
    0,
    0
};
/*float GEQ31band_gain[Band_count_GEQ]={
					1,
					2,
					3,
					4,//4
					5,
					6,
					7,
					8,//8
					9,
					10,
					9,
					8,//12
					7,
					6,
					5,
					4,
					3,
					2,
					1,
					0,
					-1,
					-2,
					-3,
					-4,
					-5,//
					-6,
					-7,
					-8,
					-9,
					-10,
					-11
					};*/
float GEQ31band_q[Band_count_GEQ] =
{
    1.414, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473,
    4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473,
    4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473, 4.318473,
    1.414
};

void zero_filterstate(void)
{
    int i, j;
/*float dm filter_state_31b[Channelnum_Max *EQ6b_ID_Max][5 * Band_count_GEQ];

float dm filter_state_HPF[HPF_ID_Max *Channelnum_Max][2 * 2 + 1];
float pm filter_cf_HPF[HPF_ID_Max *Channelnum_Max][5];

float filter_state_LFE_HPF[HPF_ID_Max *Channelnum_Max][2 * 1 + 1];
float pm filter_cf_LFE_HPF[HPF_ID_Max *Channelnum_Max][5];

float filter_state_HPF_dl[HPF_ID_Max *Channelnum_Max][2 * 2 + 1];
float filter_cf_HPF_dl[HPF_ID_Max *Channelnum_Max][5];
*/
    for(i = 0; i < Channelnum_Max * EQ6b_ID_Max; i++)
        for(j = 0; j < Band_count_GEQ * 5/*2*Band_count_GEQ+1*/; j++)
            filter_state_31b[i][j] = 0;



    for(i = 0; i < HPF_ID_Max * Channelnum_Max; i++)
        for(j = 0; j < 2 * 2 + 1; j++)
        {
            filter_state_HPF[i][j] = 0;
			filter_state_LPF[i][j] = 0;
            filter_state_HPF_dl[i][j] = 0;
        }

    return;
}



void	filter_process_HPF(float *EQ_In, float *EQ_Out)
{
    int EQ_ID = 1;
    // Calling DSP Functions for Sharc Processor
    biquad (
        EQ_In,
        EQ_Out,
        filter_cf_HPF[(EQ_ID - 1)], //pm coeffs[]
        filter_state_HPF[(EQ_ID - 1)],	//float dm state[2*sections + 1]
        1, //int samples
        1 //int sections
    );
}



void	filter_process_HPF_block(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex)
{
    int EQ_ID = 1;
    // Calling DSP Functions for Sharc Processor

    if(samplenuminblock < 1)
    {
#if USEprintf
        printf("samplenuminblock<1!\n");
#endif
        return ;
    }
    biquad (
        p_EQ_In,
        p_EQ_Out,
        filter_cf_HPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], //pm coeffs[]
        filter_state_HPF[(EQ_ID - 1) + HPF_ID_Max * channelindex],	//float dm state[2*sections + 1]
        samplenuminblock, //int samples
        1 //int sections
    );


}


void	filter_process_LPF_block(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex)
{
	int EQ_ID = 1;
	// Calling DSP Functions for Sharc Processor

	if (samplenuminblock < 1)
	{
#if USEprintf
		printf("samplenuminblock<1!\n");
#endif
		return;
	}
	
	biquad(
		p_EQ_In,
		p_EQ_Out,
		filter_cf_LPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], //pm coeffs[]
		filter_state_LPF[(EQ_ID - 1) + HPF_ID_Max * channelindex],	//float dm state[2*sections + 1]
		samplenuminblock, //int samples
		1 //int sections
		);
}


/*
double filter_state_HPF_dl[HPF_ID_Max*Channelnum_Max][2*2+1];
double pm filter_cf_HPF_dl[HPF_ID_Max*Channelnum_Max][5];
*/
void	filter_process_HPF_block_dl(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex)
{
    int EQ_ID = 1;
    int i, j;
    // Calling DSP Functions for Sharc Processor

    if(samplenuminblock < 1)
    {
#if USEprintf
    	printf("samplenuminblock<1!\n");
#endif
        return ;
    }

    for(i = 0; i < samplenuminblock; i++)
    {
        float xin, yout;

        xin = p_EQ_In[i];
        yout = xin * filter_cf_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][4];
        for(j = 0; j < 4; j++)
            yout = yout + filter_state_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][j] * filter_cf_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][j];

        filter_state_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][2] = filter_state_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][3];
        filter_state_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][3] = xin;
        filter_state_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][0] = filter_state_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][1];
        filter_state_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex][1] = yout;

        p_EQ_Out[i] = yout;

    }
}



/*void	filter_process_LFE_HPF_block(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex)
{
    int EQ_ID = 1;
    // Calling DSP Functions for Sharc Processor

    if(samplenuminblock < 1)
    {
#if USEprintf
        printf("samplenuminblock<1!\n");
#endif
        return ;
    }
    biquad (
        p_EQ_In,
        p_EQ_Out,
        filter_cf_LFE_HPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], //pm coeffs[]
        filter_state_LFE_HPF[(EQ_ID - 1) + HPF_ID_Max * channelindex],	//float dm state[2*sections + 1]
        samplenuminblock, //int samples
        1 //int sections
    );


}
*/

void filter_process_GEQ_block(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex/*0,1,2...*/)
{
    // vectorized for multi filter stages, not for samples
    // test: EQ_ID ==1
    int EQ_ID = 1;
    int Band_ID;
    int i, j;
    Band_ID = 1;

    if(samplenuminblock < 1)
    {
#if USEprintf
        printf("samplenuminblock<1!\n");
#endif
        return ;
    }
#if 1

	
#if 1
	//不处理20,25Hz两个频段  20160909 wchg
    biquad (
        p_EQ_In,
        p_EQ_Out,
        &filter_cf_31b[(EQ6b_ID_Max) * channelindex][0+ 5 * 2] ,  //pm coeffs[]
        &filter_state_31b[(EQ6b_ID_Max) * channelindex][0+ 5 * 2] ,	//float dm state[2*sections + 1]
        samplenuminblock, //int samples
        Band_count_GEQ-2 //int sections
    );


	//printf("EQ in\n");
	
	
//	printf("out--\n");
#else

    //部分频段用biquad，部分频段用直接运算
    for(i = 0; i < samplenuminblock; i++)
    {
        double xin, yout;

        xin = p_EQ_In[i];
        yout = xin * filter_cf_31b[channelindex][4];
        for(j = 0; j < 4; j++)
            yout = yout + filter_state_31b[channelindex][j] * filter_cf_31b[channelindex][j];

        filter_state_31b[channelindex][2] = filter_state_31b[channelindex][3];
        filter_state_31b[channelindex][3] = xin;
        filter_state_31b[channelindex][0] = filter_state_31b[channelindex][1];
        filter_state_31b[channelindex][1] = yout;

        p_EQ_Out[i] = yout;


    }

    Band_ID = 2;
    while(Band_ID <=/*Band_count_GEQ*/4)
    {
        for(i = 0; i < samplenuminblock; i++)
        {
            double xin, yout;

            xin = p_EQ_Out[i];
            yout = xin * filter_cf_31b[channelindex][4 + (Band_ID - 1) * 5];
            for(j = 0; j < 4; j++)
                yout = yout + filter_state_31b[channelindex][j + (Band_ID - 1) * 5] * filter_cf_31b[channelindex][j + (Band_ID - 1) * 5];

            filter_state_31b[channelindex][2 + (Band_ID - 1) * 5] = filter_state_31b[channelindex][3 + (Band_ID - 1) * 5];
            filter_state_31b[channelindex][3 + (Band_ID - 1) * 5] = xin;
            filter_state_31b[channelindex][0 + (Band_ID - 1) * 5] = filter_state_31b[channelindex][1 + (Band_ID - 1) * 5];
            filter_state_31b[channelindex][1 + (Band_ID - 1) * 5] = yout;

            p_EQ_Out[i] = yout;


        }
        Band_ID++;
    }


    biquad (
        p_EQ_Out,
        p_EQ_Out,
        &filter_cf_31b[(EQ6b_ID_Max) * channelindex][0 + 5 * 4] , //pm coeffs[]
        &filter_state_31b[(EQ6b_ID_Max) * channelindex][0 + 5 * 4] ,	//float dm state[2*sections + 1]
        samplenuminblock, //int samples
        Band_count_GEQ - 4 //int sections
    );
#endif
	
#else

    ////不使用biquad函数而采用直接运算
    for(i = 0; i < samplenuminblock; i++)
    {
        double xin, yout;

        xin = p_EQ_In[i];
        yout = xin * filter_cf_31b[channelindex][4];
        for(j = 0; j < 4; j++)
            yout = yout + filter_state_31b[channelindex][j] * filter_cf_31b[channelindex][j];

        filter_state_31b[channelindex][2] = filter_state_31b[channelindex][3];
        filter_state_31b[channelindex][3] = xin;
        filter_state_31b[channelindex][0] = filter_state_31b[channelindex][1];
        filter_state_31b[channelindex][1] = yout;

        p_EQ_Out[i] = yout;


    }

    Band_ID = 2;
    while(Band_ID <= Band_count_GEQ)
    {
        for(i = 0; i < samplenuminblock; i++)
        {
            double xin, yout;

            xin = p_EQ_Out[i];
            yout = xin * filter_cf_31b[channelindex][4 + (Band_ID - 1) * 5];
            for(j = 0; j < 4; j++)
                yout = yout + filter_state_31b[channelindex][j + (Band_ID - 1) * 5] * filter_cf_31b[channelindex][j + (Band_ID - 1) * 5];

            filter_state_31b[channelindex][2 + (Band_ID - 1) * 5] = filter_state_31b[channelindex][3 + (Band_ID - 1) * 5];
            filter_state_31b[channelindex][3 + (Band_ID - 1) * 5] = xin;
            filter_state_31b[channelindex][0 + (Band_ID - 1) * 5] = filter_state_31b[channelindex][1 + (Band_ID - 1) * 5];
            filter_state_31b[channelindex][1 + (Band_ID - 1) * 5] = yout;

            p_EQ_Out[i] = yout;


        }
        Band_ID++;
    }

#endif
}




void sim_31band_EQ(void)
{
    int  EQ_ID, Band_ID, test_i;
    int channelindex = 0;
    ///////////////////////////////

    /////////
    //Mic Main Path
    //
    EQ_ID = 1;
    //for(EQ_ID=MicMainL_EQ_ID;EQ_ID<=MicMainR_EQ_ID;EQ_ID++)
    for(channelindex = 0; channelindex < 24; channelindex++)
    {

        Band_ID = 1;
        // High Pass Filter
        //set_EQ_HPF(EQ_ID, filter_cf_HPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], &fc_HPF[EQ_ID - 1]);
        set_EQ_HPF_dl(EQ_ID, filter_cf_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex], &fc_HPF[EQ_ID - 1]);
        
		// Low Pass Filter
		set_EQ_LPF(EQ_ID, filter_cf_LPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], &fc_LPF[EQ_ID - 1]);
		
		if (channelindex == 3)
        {
			set_EQ_LPF(EQ_ID, filter_cf_LPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], &fc_HPF[EQ_ID - 1]);
			//set_EQ_HPF(EQ_ID, filter_cf_HPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], &fc_LFE_HPF[EQ_ID - 1]);
			set_EQ_HPF_dl(EQ_ID, filter_cf_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex], &fc_LFE_HPF[EQ_ID - 1]);
		}
        // Low Shelving Filter
        fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
        gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
        q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

        set_EQ_Low_Shelf(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1, gain_EQ_GEQ[EQ_ID - 1] + Band_ID - 1, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);


        // Test  peak filters
        for(Band_ID = 2; Band_ID <= Band_count_GEQ - 1; Band_ID++)
        {
            fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
            gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
            q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

            set_EQ_Peak(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1, gain_EQ_GEQ[EQ_ID - 1] + Band_ID - 1, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);

            /*if(Band_ID<9){filter_cf_31b[(EQ6b_ID_Max) * channelindex+ (EQ_ID-1)][(Band_ID-1)*5+0]=0;
             filter_cf_31b[(EQ6b_ID_Max) * channelindex+ (EQ_ID-1)][(Band_ID-1)*5+1]=0;
             filter_cf_31b[(EQ6b_ID_Max) * channelindex+ (EQ_ID-1)][(Band_ID-1)*5+2]=0;
             filter_cf_31b[(EQ6b_ID_Max) * channelindex+ (EQ_ID-1)][(Band_ID-1)*5+3]=0;
             filter_cf_31b[(EQ6b_ID_Max) * channelindex+ (EQ_ID-1)][(Band_ID-1)*5+4]=1;}*/

        }

        // High Shelving Filter
        Band_ID = Band_count_GEQ;

        fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
        gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
        q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

        set_EQ_High_Shelf(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1, gain_EQ_GEQ[EQ_ID - 1] + Band_ID - 1, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);
    }




}

//根据参数集重新设置滤波器系数
void sim_31band_EQ_outputparamset(void)
{
    int  EQ_ID, Band_ID, test_i;
    int channelindex = 0;
	float EQGain;
    ///////////////////////////////
    EQ_ID = 1;

    for(channelindex = 0; channelindex < CHANNEL_OUTPUT_NUMBER; channelindex++)
    {
		Band_ID = 1;
		EQGain = outputparam[DSP23_OutputYtype].EQBandGain[channelindex][Band_ID - 1] + outputparam[DSP23_OutputYtype].EQBandGain[29][Band_ID - 1];
		// High Pass Filter
		//set_EQ_HPF(EQ_ID, filter_cf_HPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], &outputparam[OutputYtype].LowCutFreq[channelindex]);     
		set_EQ_HPF_dl(EQ_ID, filter_cf_HPF_dl[(EQ_ID - 1) + HPF_ID_Max * channelindex], &outputparam[OutputYtype].LowCutFreq[channelindex]);

		// Low Pass Filter
		set_EQ_LPF(EQ_ID, filter_cf_LPF[(EQ_ID - 1) + HPF_ID_Max * channelindex], &outputparam[OutputYtype].HighCutFreq[channelindex]);

		// Low Shelving Filter
		fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
		gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
		q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

		set_EQ_Low_Shelf(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1, 
			&EQGain, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);

        // Test  peak filters
        for(Band_ID = 2; Band_ID <= Band_count_GEQ - 1; Band_ID++)
        {
        	EQGain = outputparam[DSP23_OutputYtype].EQBandGain[channelindex][Band_ID - 1] + outputparam[DSP23_OutputYtype].EQBandGain[29][Band_ID - 1];
        	
			fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
			gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
			q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

        	set_EQ_Peak(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1,
                        &EQGain, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);
        }

        // High Shelving Filter
		Band_ID = Band_count_GEQ;
		EQGain = outputparam[DSP23_OutputYtype].EQBandGain[channelindex][Band_ID - 1] + outputparam[DSP23_OutputYtype].EQBandGain[29][Band_ID - 1];

		fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
		gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
		q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

        set_EQ_High_Shelf(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1,
                          &EQGain, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);

    }
}

//根据spi指令中的参数值来更新响应的滤波器参数
/*
int Band_ID, 1~31
int channelindex, 0~23
 float gain db值
*/
void sim_31band_EQ_setgaintofilters(int Band_ID, int channelindex, float gain)
{
    int  EQ_ID, test_i;

    float gain_value = gain;
    ///////////////////////////////
    EQ_ID = 1;
    {
        if(Band_ID == 1)
        {
            // Low Shelving Filter
            fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
            gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
            q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

            set_EQ_Low_Shelf(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1,
                             &gain_value, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);
        }
        // Test  peak filters
        //for(Band_ID=2;Band_ID<=Band_count_GEQ-1;Band_ID++)
        if((Band_ID > 1) && (Band_ID < Band_count_GEQ))
        {
            fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
            gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
            q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

            set_EQ_Peak(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1,
                        &gain_value, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);
        }

        // High Shelving Filter
        if(Band_ID == Band_count_GEQ)
        {

            fc_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_fc[Band_ID - 1];
            gain_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_gain[Band_ID - 1];
            q_EQ_GEQ[EQ_ID - 1][Band_ID - 1] = GEQ31band_q[Band_ID - 1];

            set_EQ_High_Shelf(EQ_ID, Band_ID, Band_count_GEQ, &filter_cf_31b[(EQ6b_ID_Max) * channelindex + (EQ_ID - 1)][(Band_ID - 1) * 5], fc_EQ_GEQ[EQ_ID - 1] + Band_ID - 1,
                              &gain_value, q_EQ_GEQ[EQ_ID - 1] + Band_ID - 1);
        }
    }
}


//根据spi指令中的参数值来更新响应的滤波器参数
//高通滤波器
void 	SPI_set_EQ_HPfilter(int channelindex)
{
	///////////////////////////////
	// High Pass Filter
	//    set_EQ_HPF(1, filter_cf_HPF[HPF_ID_Max * channelindex], &outputparam[OutputYtype].LowCutFreq[channelindex]); 
#if 1
	set_EQ_HPF_dl(1, filter_cf_HPF_dl[HPF_ID_Max * channelindex], &outputparam[OutputYtype].LowCutFreq[channelindex]);
#endif
}

//根据spi指令中的参数值来更新响应的滤波器参数
//低通滤波器
void SPI_set_EQ_LPfilter(int channelindex)
{

	// Low Pass Filter
#if 1
	set_EQ_LPF(1, filter_cf_LPF[HPF_ID_Max * channelindex], &outputparam[OutputYtype].HighCutFreq[channelindex]);
#endif

	return;
}


void    set_EQ_High_Shelf (int EQ_ID, int Band_ID, int Band_count, float *filter_cf, float *fc, float *gain, float *q)
{
    float V0, Wc, k , common_den;
    float inverse_q = 1 / (*q);

    Wc = 2 * PI * (*fc);
    k = tan(Wc / (fs * 2));

    if (*gain > 0)
    {

        V0 = powf(10, *gain / 20);

        /* From  ADSP Run-Time Library Manual:
        for vector versions of the biquad function, the 5 coefficients are stored in the order:
        -A2, -A1, B2, B1, B0

        order is descent , not like ascent in Matlab
        */

        /*
        Same coefficients naming as http://en.wikipedia.org/wiki/Digital_filter#Direct_Form_II

        */

        /*
        different naming in filter design from reference textbook
        -b2=>A2, -b1=>A1, a2 => B2, a1=> B1, a0 => B0
        	!! time domain Y coefficients has different sign from Z domain denominator  coefficients.
        	DSP.Biquad use time domain coefficients, while filter design use Z domain coefficients.

        */

        common_den = 1 + (inverse_q + k) * k;	// Commmon Denominator

        filter_cf[0] = -(1 + (-inverse_q + k) * k) / common_den; // -b2 =>A2
        filter_cf[1] = -2 * (k * k - 1) / common_den;				//  -b1=> A1

        filter_cf[2] = (V0 + k * (-sqrt(V0) * inverse_q + k)) / common_den;	 // a2 => B2
        filter_cf[3] = 2 * (k * k - V0) / common_den;				 // a1 => B1
        filter_cf[4] = (V0 + k * (sqrt(V0) * inverse_q + k)) / common_den; 	 // a0 => B0


    }
    else
    {
        V0 = powf(10, -*gain / 20);

        common_den = 1 + (1.0 / sqrt(V0) * inverse_q + k / V0) * k;
        filter_cf[0] = -(1 + (-1.0 / sqrt(V0) * inverse_q + k / V0) * k) / common_den;	// -b2=>A2
        filter_cf[1] = -2 * (k * k / V0 - 1) / common_den;   //  -b1=> A1

        common_den = V0 + (sqrt(V0) * inverse_q + k) * k;
        filter_cf[2] = (1 + (-inverse_q + k) * k) / common_den;	// a2 => B2
        filter_cf[3] = 2 * (k * k - 1) / common_den;     // a1 => B1
        filter_cf[4] = (1 + (+inverse_q + k) * k) / common_den; // a0 => B0


    }

}

void	set_EQ_Low_Shelf  (int EQ_ID, int Band_ID, int Band_count, float *filter_cf, float *fc, float *gain, float *q)
{

    float V0, Wc, k , common_den;
    float inverse_q = 1 / (*q);

    Wc = 2 * PI * (*fc);
    k = tan(Wc / (fs * 2));

    if (*gain > 0)
    {

        V0 = powf(10, *gain / 20);

        common_den = 1 + (inverse_q + k) * k;

        filter_cf[0] = -(1 + (-inverse_q + k ) * k) / common_den; // -b2 =>A2
        filter_cf[1] = -2 * (k * k - 1) / common_den;				//  -b1=> A1

        filter_cf[2] = (1 + k * (-sqrt(V0) * inverse_q + k * V0)) / common_den;	 // a2 => B2
        filter_cf[3] = 2 * (k * k * V0 - 1) / common_den;				 // a1 => B1
        filter_cf[4] = (1 + k * (sqrt(V0) * inverse_q + k * V0)) / common_den; 	 // a0 => B0

    }
    else
    {
        V0 = powf(10, -*gain / 20);

        common_den = 1 + (sqrt(V0) * inverse_q + V0 * k) * k;
        filter_cf[0] = -(1 + ( -sqrt(V0) * inverse_q + k * V0 ) * k) / common_den;	// -b2=>A2
        filter_cf[1] = -2 * (k * k * V0 - 1) / common_den;   //  -b1=> A1

        filter_cf[2] = (1 + (-inverse_q + k ) * k) / common_den;	// a2 => B2
        filter_cf[3] = 2 * (k * k - 1) / common_den;     // a1 => B1
        filter_cf[4] = (1 + (+inverse_q + k) * k) / common_den; // a0 => B0

    }


}

void	set_EQ_Peak  (int EQ_ID, int Band_ID, int Band_count, float *filter_cf, float *fc, float *gain, float *q)
{

    float V0, Wc, k , common_den;
    float inverse_q = 1 / (*q);

    Wc = 2 * PI * (*fc);
    k = tan(Wc / (fs * 2));



    if (*gain > 0)
    {

        V0 = powf(10, *gain / 20);

        common_den = 1 + (inverse_q + k) * k;

        filter_cf[0] = -(1 + (-inverse_q + k ) * k) / common_den; // -b2 =>A2
        filter_cf[1] = -2 * (k * k - 1) / common_den;				//  -b1=> A1

        filter_cf[2] = (1 + k * (-V0 * inverse_q + k)) / common_den;	 // a2 => B2
        filter_cf[3] = 2 * (k * k - 1) / common_den;				 // a1 => B1
        filter_cf[4] = (1 + k * ( V0 * inverse_q + k)) / common_den; 	 // a0 => B0

    }
    else
    {
        V0 = powf(10, -*gain / 20);

        common_den = 1 + (V0 * inverse_q + k) * k;;
        filter_cf[0] = -(1 + ( -V0 * inverse_q + k ) * k) / common_den;	// -b2=>A2
        filter_cf[1] = -2 * (k * k - 1) / common_den;     //  -b1=> A1

        filter_cf[2] = (1 + (-inverse_q + k ) * k) / common_den;	// a2 => B2
        filter_cf[3] = 2 * (k * k - 1) / common_den;     // a1 => B1
        filter_cf[4] = (1 + (+inverse_q + k ) * k) / common_den; // a0 => B0

    }

}

float inverseq_HPF = 1.4142135;//(1/0.707)
void    set_EQ_HPF (int EQ_ID, float *filter_cf, float *fc)
{
    float V0, Wc, k , common_den, common_factor;

    //inverseq_HPF =sqrtf(2.0);//
    Wc = 2 * PI * (*fc);
    k = tan(Wc / (fs * 2));

    common_den = 1 + (inverseq_HPF + k) * k;
    common_factor = 1 / common_den;

    filter_cf[0] = -(1 + (-inverseq_HPF + k) * k) * common_factor; // -b2 =>A2
    filter_cf[1] = -2 * (k * k - 1) * common_factor;				//  -b1=> A1

    filter_cf[2] = 1 * common_factor;	 // a2 => B2
    filter_cf[3] = -2 * common_factor;				 // a1 => B1
    filter_cf[4] = 1 * common_factor; 	 // a0 => B0


}

void    set_EQ_HPF_dl (int EQ_ID, float *filter_cf, float *fc)
{
    double V0, Wc, k , common_den, common_factor;

    //inverseq_HPF =sqrtf(2.0);//
    Wc = 2 * PI * (*fc);
    k = tan(Wc / (fs * 2));

    common_den = 1 + (inverseq_HPF + k) * k;
    common_factor = 1 / common_den;

    filter_cf[0] = -(1 + (-inverseq_HPF + k) * k) * common_factor; // -b2 =>A2
    filter_cf[1] = -2 * (k * k - 1) * common_factor;				//  -b1=> A1

    filter_cf[2] = 1 * common_factor;	 // a2 => B2
    filter_cf[3] = -2 * common_factor;				 // a1 => B1
    filter_cf[4] = 1 * common_factor; 	 // a0 => B0


}

void set_EQ_LPF (int EQ_ID, float *filter_cf, float *fc)
{
    float V0, Wc, k , common_den, common_factor;

    Wc = 2 * PI * (*fc);
    k = tan(Wc / (fs * 2));

    common_den = 1 + (inverseq_HPF + k) * k;
    common_factor = 1 / common_den;

    filter_cf[0] = -(1 + (-inverseq_HPF + k) * k) * common_factor; // -b2 =>A2
    filter_cf[1] = -2 * (k * k - 1) * common_factor;				//  -b1=> A1

 
    filter_cf[2] = (k * (k)) / common_den;	 // a2 => B2
    filter_cf[3] = 2 * (k * k) / common_den;				 // a1 => B1
    filter_cf[4] = (k * (k)) / common_den; 	 // a0 => B0
}


//根据参数集重新设置分频点，分频滤波器系数

void sim_frequencydivisionfilter_outputparamset(void)
{
    int channelindex = 0;
    int bandx;

    ///////////////////////////////
    
    /////////
    for(channelindex=0;channelindex<OUTPUT_CHANNELS;channelindex++)
    {
    	 //修改分频点参数
    	divisionbandnum_set[channelindex] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][0];
    	
   		//修改到通路的映射表
   		if(divisionbandnum_set[channelindex]<3)	
   		outsignalchannel_book24m3[channelindex][2] =-1;
   		if(divisionbandnum_set[channelindex]<2)	
   		outsignalchannel_book24m3[channelindex][1] =-1;
   		if(divisionbandnum_set[channelindex]<1)	
   		outsignalchannel_book24m3[channelindex][0] =-1;
    	
		//更新分频滤波器系数   	
		fc_division[channelindex][0] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][1];
    		fc_division[channelindex][1] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][2];	
   		if(divisionbandnum_set[channelindex]==2)
  	   	{
	 		set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[channelindex][0]);
	        	set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[channelindex][0]);
  	    	}
		else if(divisionbandnum_set[channelindex]==3)
  	    	{
 			set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[channelindex][0]);//低频
	        	set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[channelindex][0]);    //中频
	        	set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][1*5], &fc_division[channelindex][1]);
	        	set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+2][0], &fc_division[channelindex][1]);		//高频
  	    	}
    }
 
  	/*
    int j = 0;
  	for(channelindex=0;channelindex<8;channelindex++)
  	{
  		for(j = 0 ; j < 3 ;  j ++)
  		{
  			printf("%d ",outsignalchannel_book24m3[channelindex][j]);	
  		}	
  		printf("\n");
	}
	*/
    //////////////////////////////////////////////////////////////////////////////////////////////////
    for(channelindex=0;channelindex<24;channelindex++)
    {
    	 //修改分频点参数
#if 0
    	divisionbandnum_set[channelindex] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][0];
#endif	
   		//修改到通路的映射表
#if 0
   		if(divisionbandnum_set[channelindex]<3)	
   		outsignalchannel_book24m3[channelindex][2] =-1;
   		if(divisionbandnum_set[channelindex]<2)	
   		outsignalchannel_book24m3[channelindex][1] =-1;
   		if(divisionbandnum_set[channelindex]<1)	
   		outsignalchannel_book24m3[channelindex][0] =-1;
#endif
 			
		//更新分频滤波器系数   	
#if 0
		fc_division[channelindex][0] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][1];
    		fc_division[channelindex][1] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][2];	
#endif
  
   		if(divisionbandnum_set[channelindex]==2)
  	   	{
#if 0
  	   	 	set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[channelindex][0]);
	        	set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[channelindex][0]);
#endif
	    }
		else if(divisionbandnum_set[channelindex]==3)
  	    	{
#if 0
 			set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[channelindex][0]);//低频
	        	set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[channelindex][0]);    //中频
	        	set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][1*5], &fc_division[channelindex][1]);
	        	set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+2][0], &fc_division[channelindex][1]);		//高频
#endif
  	    	}
  	    
  	    
   		
    }
}


int oneframeprocess_alignment()
{

	int i, j, m;

#if 0
	
	
//	zero_filterstate(); //临时加入做测试
	
	
 //   alignment_outpcmDAdatabuf_link(AlignmentChannel - 1);
		
//    int outputchannelnum = (OutputYtype == 3)? 24: ((OutputYtype == 2)? 12: 8);
    init_channeloutputDAflag(outputchannelnum);
#if 1    
    int bandz;
    for(bandz = 1; bandz <= 31; bandz++){
	    if(AlignmentChannel <= OUTPUT_CHANNELS){
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
#endif 
    //产生新的pcm数据
    testsignal_generator_oneframe(SignalModeVal, FFTSIZE / 2, p_multichannelinputbuf[0], 1);
    
    for(j = 0; j < OUTPUT_CHANNELS; j++)
    {

//    	if(channeloutputDAflag[j][0]+channeloutputDAflag[j][1]+channeloutputDAflag[j][2]==0)
 //   		continue;
        
        for(i = 0; i < FFTSIZE / 2; i++);
//            p_multichanneloutputbuf[j][i] = p_multichannelinputbuf[0][i]*fadeinoutgain;
		//31 band EQ
		filter_process_GEQ_block(p_multichannelinputbuf[0], p_multichanneloutputbuf[j], FFTSIZE / 2, j);
		//High Pass
		filter_process_HPF_block_dl(p_multichanneloutputbuf[j], p_multichanneloutputbuf[j], FFTSIZE / 2, j);
		//Low Pass
		filter_process_LPF_block(p_multichanneloutputbuf[j], p_multichanneloutputbuf[j], FFTSIZE / 2, j);

#if 0
        //声道开关
        if(AlignmentChannel <= OUTPUT_CHANNELS)
        	V0[j] = (j == AlignmentChannel - 1)? outputparam[OutputYtype].ChannelGain_Val[AlignmentChannel - 1]: 0;
       	else if (AlignmentChannel == 25)
       		V0[j] = (j <= 1)? outputparam[OutputYtype].ChannelGain_Val[j]: 0;
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
            p_multichanneloutputbuf[j][i] *= V0[j] * knobvolumegainvalue * fadeinoutgain;
    }
#endif
    
        
    //置零
//    int tx_blockoffset = NUM_SAMPLES * NUM_TX_SLOTS * tx_blockcount;
    for(j = 0; j < CHANNELS/*6*/; j++)
//    	 for(i = 0; i < NUM_SAMPLES * NUM_TX_SLOTS; i++);
//    		TxBlock_SPORT24[j][tx_blockoffset+i] = 0;
    
    //float to 24bit pcm
    //输出pcm数据
    for(j = 0; j < OUTPUT_CHANNELS; j++)
    {int bandx;
        int pcmval1, pcmval2, pcmval3, pcmval4;
        	for(bandx=0; bandx<3;bandx++)
        	{		int channelindex =j;
//            	int routeindex =outsignalchannel_book24m3[channelindex][bandx];
				int routeindex = 0;
            	if(routeindex<0)
            		continue;
//            	if(outpcmDAdatabuf[routeindex]<=0)
 //           		continue;
			//分频
//			p_divisionoutbuf=&inputmultichannelfftdata[0];	
				
//			frequencydivision_channel(channelindex,bandx, p_multichanneloutputbuf[channelindex],p_divisionoutbuf,FFTSIZE/2);
        		// for(i = 0; i < FFTSIZE / 2; i++)
           		//	 p_divisionoutbuf[i] = p_multichanneloutputbuf[j][i];
#if 0        	
	            if(outpcmDAdatabuf[routeindex] != 0)
	            {
	            	for(i = 0; i < FFTSIZE / 2; i++) //2*6*3/4 = 9
	        		{
//	                		pcmval1 = p_divisionoutbuf[i] * (0x1 << 23);//p_multichanneloutputbuf[j][i] * (0x1 << 23);
	                		outpcmDAdatabuf[routeindex][i * NUM_TX_SLOTS + NUM_SAMPLES * NUM_TX_SLOTS * tx_blockcount] += pcmval1;
	        		}
	            }
	            else
	            {
	            	for(i = 0; i < FFTSIZE / 2; i++) //2*6*3/4 = 9
	            		outpcmDAdatabuf[routeindex][i * NUM_TX_SLOTS + NUM_SAMPLES * NUM_TX_SLOTS * tx_blockcount] += 0;
	    		}
#endif
    		
    		}
    }
    
    //检查声道是否切换成功
    //如果两边不相等，则说明上面仍然在处理原来的声道，导致原来的声道有噪声
    //把数据删除以去除噪声
//    if(DSP23_AlignmentChannel != AlignmentChannel){
 //   	DSP23_AlignmentChannel = AlignmentChannel;
 //   	cleartDAbuff();
//    }

#endif

    return 0;
}



/*
*
*	这个部分是后来添加上去的。
*
*
*
*/
//根据spi指令中的参数值来设置分频点数量
/*

int channelindex, 0~23
 int pointnum, 1~3
*/
void 	sim_frequencydivision_setdivisionpointnum(int channelindex, int pointnum)
{
 		//修改分频点参数
    	divisionbandnum_set[channelindex] = pointnum;// outputparam[DSP23_OutputYtype].FreqDivbandNum.[channelindex];
    	//outputparam[DSP23_OutputYtype].FreqDivbandNum[channelindex]= pointnum;
   		//修改到通路的映射表
   		if(divisionbandnum_set[channelindex]<3)	
   		outsignalchannel_book24m3[channelindex][2] =-1;
   		if(divisionbandnum_set[channelindex]<2)	
   		outsignalchannel_book24m3[channelindex][1] =-1;
   		if(divisionbandnum_set[channelindex]<1)	
   		outsignalchannel_book24m3[channelindex][0] =-1;
    	
		//更新分频滤波器系数   		
		fc_division[channelindex][0] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][1];
    		fc_division[channelindex][1] = outputparam[DSP23_OutputYtype].FreqDivVal[channelindex][2];
   		if(divisionbandnum_set[channelindex]==2)
  	    {
	 		set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[channelindex][0]);
	        set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[channelindex][0]);
  	    }else
  	    if(divisionbandnum_set[channelindex]==3)
  	    {
 			set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[channelindex][0]);//低频
 			
	        set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[channelindex][0]);    //中频
	        set_EQ_LPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+1][1*5], &fc_division[channelindex][1]);
        
	        set_EQ_HPF(0, &filter_cf_frequencydivion[channelindex*MAX_FREQUNCYDIVISION_BANDNUM+2][0], &fc_division[channelindex][1]);		//高频
  	    }
  	    
  	    return;
}



//初始化分频滤波器参数
int init_divisionfilter(void)
{
	int i;
 
	for(i=0;i<24;i++)
	{
	    if(divisionbandnum_set[i]==2)
	    {
 			set_EQ_LPF(0, &filter_cf_frequencydivion[i*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[i][0]);
        	set_EQ_HPF(0, &filter_cf_frequencydivion[i*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[i][0]);
	    }
	    else  if(divisionbandnum_set[i]==3)
	    {
			set_EQ_LPF(0, &filter_cf_frequencydivion[i*MAX_FREQUNCYDIVISION_BANDNUM+0][0], &fc_division[i][0]);//低频
	
	        set_EQ_HPF(0, &filter_cf_frequencydivion[i*MAX_FREQUNCYDIVISION_BANDNUM+1][0], &fc_division[i][0]);    //中频
	        set_EQ_LPF(0, &filter_cf_frequencydivion[i*MAX_FREQUNCYDIVISION_BANDNUM+1][1*5], &fc_division[i][1]);

	        set_EQ_HPF(0, &filter_cf_frequencydivion[i*MAX_FREQUNCYDIVISION_BANDNUM+2][0], &fc_division[i][1]);		//高频
	    }
	}
   	return 0;
 }
 
 //对一个声道的数据进行分频，得到指定频带的信号
 int frequencydivision_channel(int ch, int bandindex,float *p_databuf,
 						float *p_divisionoutbuf,int samplenuminblock)
 {
 
 	 if(divisionbandnum_set[ch]==2)
  	 {
  	    
  	    	if(bandindex==0)
	 		    biquad (
		        p_databuf,
		        p_divisionoutbuf,
		        filter_cf_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+0] , //pm coeffs[]
		        filter_state_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+0] ,	//float dm state[2*sections + 1]
		        samplenuminblock, //int samples
		        1//int sections
		    	);
        
	    	if(bandindex==1)
		    	biquad (
		        p_databuf,
		        p_divisionoutbuf,
		        filter_cf_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+1] , //pm coeffs[]
		        filter_state_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+1] ,	//float dm state[2*sections + 1]
		        samplenuminblock, //int samples
		        1//int sections
		    	);	
  	    }
	    else  if(divisionbandnum_set[ch]==3)
  	    {
 			if(bandindex==0)
	  	    	   biquad (
		        p_databuf,
		        p_divisionoutbuf,
		        filter_cf_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+0] , //pm coeffs[]
		        filter_state_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+0] ,	//float dm state[2*sections + 1]
		        samplenuminblock, //int samples
		        1//int sections
		    	);
        
	    		if(bandindex==1)
		    	 biquad (
		        p_databuf,
		        p_divisionoutbuf,
		        filter_cf_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+1] , //pm coeffs[]
		        filter_state_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+1] ,	//float dm state[2*sections + 1]
		        samplenuminblock, //int samples
		        2//int sections
		    	);	
	    		if(bandindex==2)
		    	biquad (
		        p_databuf,
		        p_divisionoutbuf,
		        filter_cf_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+2] , //pm coeffs[]
		        filter_state_frequencydivion[ch*MAX_FREQUNCYDIVISION_BANDNUM+2] ,	//float dm state[2*sections + 1]
		        samplenuminblock, //int samples
		        1//int sections
		    	);	
  	    }
	    else
  	    {
  	    	int i;
  	    	if(bandindex==0)
  	    	{
  	    		for(i=0;i < samplenuminblock; i++)
  	    			p_divisionoutbuf[i] = p_databuf[i];
  	    	}else
  	    	{
  	    		for(i=0;i < samplenuminblock; i++)
  	    			p_divisionoutbuf[i] = 0;
  	    	}
  	    }

		
  	    return 0;
}



