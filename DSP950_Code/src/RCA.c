#include "./include/RCA.h"
#include <def21489.h>
#include <cdef21489.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>
#include "./include/cs5368.h"
 
/*	
*	功能:
*		RCA输入
*/
void RCA_Pin_Init(void)
{
    //使用PCG，为音频数据分频等
    PCG_Init_GLOBAL(); 
	/**时钟部分BCLK**/
    SRU(HIGH, PBEN10_I);
    SRU(PCG_CLKA_O, DAI_PB10_I);  
    SRU(PCG_CLKA_O, SPORT6_CLK_I); 
    /**帧时钟部分LRCLK**/		  
    SRU(HIGH, PBEN20_I); 		  
    SRU(PCG_FSA_O, DAI_PB20_I); 
    SRU(PCG_FSA_O, SPORT6_FS_I); 
    //数据线
    //RCA
    SRU(LOW, PBEN05_I);			
	SRU(DAI_PB05_O, SPORT6_DA_I);
}
  
void RCA_Init( void )
{		
	AD_AES_Init(0);	
	AD_MIC_Select(1);//选择RCA输入	
}






