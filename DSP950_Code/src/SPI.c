#include "./include/SPI.h"
#include <def21489.h>
#include <cdef21489.h>
#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <sysreg.h>
#include <stdio.h>
 
extern int g_spi_data;
int g_init_done;
 
unsigned int g_GetRXSPI;

void SPI_Pin_Init(void)
{
	SRU(HIGH, DPI_PBEN02_I);
	SRU(LOW, DPI_PBEN01_I);	
	SRU(LOW, DPI_PBEN03_I);
	SRU(LOW, DPI_PBEN04_I);
	
    SRU(SPI_MISO_O, DPI_PB02_I);	//connect MISO to DPI02
    SRU(DPI_PB01_O, SPI_MOSI_I);	//connect DPI01 to MOSI
    SRU(DPI_PB03_O, SPI_CLK_I);		//connect SPI CLK to DPI03
	SRU(DPI_PB04_O, SPI_DS_I);
	
    SRU(SPI_MISO_PBEN_O, DPI_PBEN02_I);
    SRU(SPI_MOSI_PBEN_O, DPI_PBEN01_I);
    SRU(SPI_CLK_PBEN_O,  DPI_PBEN03_I);    
}


void SPI_Config(void)
{
    *pSPICTL = (TXFLSH | RXFLSH);
    //*pSPIFLG = 0;

    //*pSPIBAUD = 100;
    //*pSPIFLG = (0xF00 			//8~11b:SPIFLGx SPI Device Select Control. Selects (if cleared, = 0) a corresponding
    //DPI pin (depending on pin routing) output to be asserted for an SPI slave-select.
    //For AUTOSDS=1, there is automatic HW control regardless of CPHASE setting
    //0000 = All SPIFLGx cleared
    //1111 = All SPIFLGx set (default)
    //				| SPI_FLAG);//0b~3b:SPI Device Select Enable. Enables or disables the corresponding
    //output signal to the SRU2 be used for SPI slave-select.
    //0 = Disable SPIFLGx output enable
    //1 = Enable SPIFLGx output enable
    //Note DS0EN bit is set in SPI master mode only.
    *pSPICTL = (SPIEN			// | SPIMS | WL32 | MSBF | TIMOD1 | CLKPL | GM | CPHASE | AUTOSDS);
                //| SPIMS		//SPI master select
                | WL32
                //| MSBF			//Most Significant Byte First.
                //| TIMOD1 		//Transfer Initiation Mode. Defines transfer initiation mode and interrupt generation.
                //00 = Initiate transfer by read of receive buffer. Interrupt active when receive buffer is full.
                //01 = Initiate transfer by write to transmit buffer. Interrupt active when transmit buffer is empty.
                //10 = Enable DMA transfer mode. Interrupt configured by DMA.
                //11 = Reserved
                | SENDZ
               	| CLKPL 	//Clock Polarity.
                //0 = Active high SPICLK (SPICLK low is the idle state)
                //1 = Active low SPICLK (SPICLK high is the idle state)
                //Note that the CLKPL/CPHASE bits define the SPI mode.
                | GM 		//Get Data. When RXSPI is full, get data or discard incoming data.
                //0 = Discard incoming data
                //1 = Get more data, overwrites the previous data
                | CPHASE 	//Clock Phase. Selects the transfer format.
                //0 = SPICLK starts toggling at the middle of 1st data bit
                //1 = SPICLK starts toggling at the start of 1st data bit (default setting)
                | AUTOSDS	//Auto Slave device Select.
                //0 = Auto slave device select controlled by SPI hardware only for CPHASE=0
                //1 = Auto slave device select controlled by SPI hardware regardless for CPHASE setting
                //Feature not supported if SMLS-bit is set
               	| ISSEN
               );
               
    
                  
}

int g_SPI_Interrupt_Flag = 0;

void SPIH_ISR(int sig_int)
{
	g_SPI_Interrupt_Flag = 1;
	g_GetRXSPI = *pRXSPI;
	//*pTXSPI = 0x11223344;
}


void Register_SPI_Interrupt( void )
{
	interrupt(SIG_SPIH, SPIH_ISR);
}
 

void SPI_Init(void)
{		
	SPI_Pin_Init();		
	SPI_Config();	
	Register_SPI_Interrupt();	
}

/*
*
*	主要是调用指令解析这块
*
*
*
*/
void handle_spi_raw_data(int i_value)
{
    unsigned int instructiongroup = getinstructiongroup(i_value);
    unsigned int instructionindexingroup = getinstructionindexingroup(i_value);

	//从PISP1016上移植过来
	if(i_value == 0 || i_value == 0x34000001)
      		return;

	if(i_value == 0x34000000)
	{
		g_init_done = 1;	
	//printf("---init done---\n");
	}
	
    if(instructiongroup == 1)
    {
    	if((instructionindexingroup<16)&&(instructiondecoder[1][instructionindexingroup]!=NULL))
        instructiondecoder[1][instructionindexingroup]();
    }
    else if(instructiongroup <= 8)
    {
    	if((instructionindexingroup<16)&&(instructiondecoder[2][instructionindexingroup]!=NULL))
        instructiondecoder[2][instructionindexingroup]();
    }
    else if(instructiongroup <= 12)
    {
    	if((instructionindexingroup<16)&&(instructiondecoder[3][instructionindexingroup]!=NULL))
        instructiondecoder[3][instructionindexingroup]();
    }
    else if(instructiongroup <= 13)
    {
    	if((instructionindexingroup<16)&&(instructiondecoder[4][instructionindexingroup]!=NULL))
        instructiondecoder[4][instructionindexingroup]();
    }
    else if(instructiongroup <= 14)
    {
    	if((instructionindexingroup<16)&&(instructiondecoder[5][instructionindexingroup]!=NULL))
        instructiondecoder[5][instructionindexingroup]();
    }
    else
    {
    	if(instructiondecoder[6][0]!=NULL)
        instructiondecoder[6][0]();
    }	
}

 

extern int g_volume_count_value;

/*
*		这一块主要是接收指令，抛弃一些不需要的指令数
*/
void handle_spi_code(void)
{
	if(g_SPI_Interrupt_Flag == 1)
	{
		g_SPI_Interrupt_Flag = 0;
		//g_GetRXSPI = *pRXSPI;
		//printf("----spi------\n");

		if((g_GetRXSPI != 0) && (g_GetRXSPI != 0xffffffff))
		{
			g_spi_data = *pRXSPI;
			#if 0
			printf("-----0x%08x---------\n",g_spi_data);
			if((g_spi_data >> 16) == 0x000500)
			{
				g_volume_count_value = (g_spi_data >> 8) & 0xff;
				printf("-----volume---------\n");
			}		
			#endif
			handle_spi_raw_data(g_spi_data);	
		}			
	}		
}












