#include "./include/mic.h"
#include <def21489.h>
#include <cdef21489.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>
#include "./include/cs5368.h"
/*	 
*	����:
*		MIC����
*/
void MIC_Pin_Init(void)
{
    //ʹ��PCG��Ϊ��Ƶ���ݷ�Ƶ��
    PCG_Init_GLOBAL(); 
	/**ʱ�Ӳ���BCLK**/
    SRU(HIGH, PBEN10_I);
    SRU(PCG_CLKA_O, DAI_PB10_I);  
    SRU(PCG_CLKA_O, SPORT6_CLK_I); 
    /**֡ʱ�Ӳ���LRCLK**/		  
    SRU(HIGH, PBEN20_I); 		  
    SRU(PCG_FSA_O, DAI_PB20_I); 
    SRU(PCG_FSA_O, SPORT6_FS_I); 
    //������
    //RCA
    SRU(LOW, PBEN14_I);			
	SRU(DAI_PB14_O, SPORT6_DA_I);
}

void MIC_Init( void )
{		
	AD_AES_Init(0);	
	AD_MIC_Select(1);//ѡ��RCA����	
}

