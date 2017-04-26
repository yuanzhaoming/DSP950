
///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
// NAME:     initDAI.c(Block-based Talkthrough)                                     //
// DATE:                                                                  //
// PURPOSE:      //
//                                    //
//                                                                                   //
// USAGE:    This file initializes the DAI for accessing the ADC and DACs.           //
//                                                                                    //
///////////////////////////////////////////////////////////////////////////////////////




// The following definition allows the SRU macro to check for errors. Once the routings have
// been verified, this definition can be removed to save some program memory space.
// The preprocessor will issue a warning stating this when using the SRU macro without this
// definition
#define SRUDEBUG  // Check SRU Routings for errors.

#include <def21489.h>
#include <cdef21489.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>

#define NOP asm volatile("nop;")

#define PI 3.14159265358979323846

void delay()
{
	int i;	
	for (i = 0; i < 4096 * 1024 * 20; i++)
		NOP;	
}


#if 0 
void SinWave()
{
	int i = 0;
	for(i = 0 ; i < NUM_SAMPLES * NUM_TX_SLOTS * CIRCLEDMADATABUFFNUM ;i ++)
	{
		TxBlock_TDM[i] = sin(2*PI*i/(NUM_SAMPLES * NUM_TX_SLOTS * CIRCLEDMADATABUFFNUM));			
	}			
}
#endif

static int g_i_pcg_used = 0;

void PCG_Init_GLOBAL()
{
	if(g_i_pcg_used == 0)
	{
		g_i_pcg_used = 1;
		SRU(LOW, PBEN03_I); 												//PCG DAI_P01  £¨PCK clk input£©
		SRU(DAI_PB03_O, PCG_EXTA_I);										//PCG clock in

		*pPCG_CTLA1 = (2* 2) | CLKASOURCE | FSASOURCE | ((2 * 2 /2)<<20);	//Division for clock
		*pPCG_CTLA0 = 512 | ENCLKA | ENFSA; 								// Frame syn Division  		
	}		
}


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



