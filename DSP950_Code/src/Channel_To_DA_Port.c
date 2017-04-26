#include "./include/channel_To_DA_Port.h"
#include "./include/input_types.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <complex.h>
#include "./include/PCM4104.h"
#include "./include/cs5368.h"
#include "./include/spdif_coax.h"

#define ALLPORTNUM 30

int *outpcmDAdatabuf[MAXROUTENUM];


//只有8个输出通道
int outsignalchannel_book24m3[MAXCAHNNELNUM][3] =
{
    { -1,-1,-1}, 
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},
    { -1,-1,-1},  
};


		

/*
*
*			正常工作模式下，声道与输出接口缓冲区的连接
*
*
*
*/
int outpcmDAdatabuf_link( void )
{
    int i, j;
    
    //for(i = 0; i < MAXROUTENUM; i++)
    //	outpcmDAdatabuf[i] = 0;
    	
    //for(j = 0; j < 3; j++)
    {
        for(i = 0; i < MAXCAHNNELNUM; i++)
        {
            //12.01 
           	//从上位机获取输出的那个位置Y1..Yn.(实际也是对应硬件接口)
           	//值就是输出的声道，如1...24.
           // int routeindex= outsignalchannel_book24m3[i][j];
           int route_index = i;
           // if((route_index < 0)||(route_index>MAXROUTENUM))				
           // 	continue;
            
           	//这个直接映射出去，就可以了 2016-11-25
            outpcmDAdatabuf[route_index] = &TxBlock_PCM4104[route_index/2][route_index%2]; 
        }
    }	
    return 0;
}


int inputADdatabuf_link( void )
{
	int i = 0;
	for(i = 0; i < 8; i++)//有16个声道    
	{        
		p_multichanneloutputbuf[i] = &RxBlock_I2S_CS5368[i/2][i%2];//&outputmultichannelpcmdata[i * FFTSIZE];
	}
	return 0;
}
 
int inputSPDIFdatabuf_link( void )
{
	int i = 0;
	for(i = 0; i < 8; i++)//有16个声道    
	{        
		p_multichannel_input_spdif_buf[i] = &RxBlock_SPDIF[i/2][i%2];//&outputmultichannelpcmdata[i * FFTSIZE];
	}
	return 0;
}



//矩阵映射
int matrixtooutputchannel(float p_multichannelbuf[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2]
	,float p_outchanneloutputbuf[USE_PCM4104_DATALINE_MAX * 2][NUM_SAMPLES*NUM_TX_SLOTS/2] )
{
	int outch,i,j;
	
	for(outch = 0; outch < USE_PCM4104_DATALINE_MAX * 2 - 2; outch++)
	{
		for(j = 0; j < FFTSIZE / 2; j++)
			p_outchanneloutputbuf[outch][j] = 0;
					 
		for(i = 0; i < USE_PCM4104_DATALINE_MAX * 2 - 2; i++)
		{
			if((Matrix_ChtoOutch[i][outch] == 1) )
			{
				for(j = 0; j < FFTSIZE / 2; j++)
					p_outchanneloutputbuf[outch][j] += p_multichannelbuf[i][j];
			}
		}
	}
	return 0;
}


/*
*
*		对于左右声道的数据，保存到一个数组中，从而去放数据
*
*	示例如下:
 #include "time.h"
  2 #include "stdio.h"
  3 
  4 int main(int argc,char *argv[])
  5 {
  6         int arr[2][8];
  7 
  8         int array[2][8] = {{1,2,3,4,5,6,7,8},
  9                 {5,3,2,14,2,5,2,7}
 10         };
 11         int *br[4];
 12 
 13         br[0] = &arr[0][0];
 14         br[1] = &arr[0][1];
 15         br[2] = &arr[1][0];
 16         br[3] = &arr[1][1];
 17         int i,j;
 18 
 19         printf("\n--------origin array------\n");
 20         for(i = 0; i < 2 ;i ++)
 21         {       
 22                 for(j = 0 ; j < 8 ; j ++)
 23                 {       
 24                         printf("%d ",array[i][j]);
 25                 }
 26                 printf("\n");
 27         }       
 28         printf("\n------------now------------\n");
 29         
 30         printf("          fill arr              \n");
 31         int k = 0;
 32         for(i = 0 ; i < 4 ; i ++)
 33         {       
 34                 for(j = 0 ; j < 4 ; j ++)
 35                 {       
 36                         br[i][2*j] = array[i/2][i%2+j*2];
 37                         //printf("a:%d\n",array[i/2][i%2+j]);
 38                         printf("a:%d,k:%d\n",br[i][k],k);
 39                         k ++;
 40                 }
 41         }
 42         printf("----------------\n");
 43
 44         for(i = 0 ; i < 2 ; i ++)
 45         {       
 46                 for(j = 0 ; j < 8 ; j ++)
 47                 {       
 48                         printf("%d   ",arr[i][j]);
 49                 }
 50                 printf("\n");
 51         }
 52         printf("\n");
 53         return 0;
 54 }
*
*
*/
















