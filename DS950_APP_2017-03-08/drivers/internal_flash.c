///////////////////////////////////////copyright yuan QinJing 2016-10-27////////////////////////////////
#include "internal_flash.h"
#include "stm32f10x.h"
#define PageSize 1024

/*
* 			将数据写入stm32的内部flash中，操作方法为:
*
*
*			flash_write(0x0807F000,&APP_FLAG,1)	;
*
*/
uint8_t Internal_Flash_Write(uint32_t StartAddr,uint32_t *p_data,uint32_t size)
{	
	volatile FLASH_Status FLASHStatus;	
	uint32_t EndAddr=StartAddr+size*4;	
	uint32_t NbrOfPage = 0;	
	uint32_t EraseCounter = 0x0, Address = 0x0;
	int i;
	int MemoryProgramStatus=1;

	FLASH_Unlock();          
	NbrOfPage=((EndAddr-StartAddr)>>10)+1;	
	//FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP );
	FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);

	FLASHStatus=FLASH_COMPLETE;


	for(EraseCounter=0;(EraseCounter<NbrOfPage)&&(FLASHStatus==FLASH_COMPLETE);EraseCounter++)
	{		
		FLASHStatus=FLASH_ErasePage(StartAddr+(PageSize*EraseCounter));	
	}

	Address = StartAddr;
	i=0;
	while((Address<EndAddr)&&(FLASHStatus==FLASH_COMPLETE))	
	{	
		//FLASHStatus=FLASH_FastProgramWord(Address,p_data[i++]);
		FLASHStatus= FLASH_ProgramWord(Address,p_data[i++]);
		
		Address=Address+4;
	}

	Address = StartAddr;
	i=0;
	while((Address < EndAddr) && (MemoryProgramStatus != 0))
	{	
		if((*(uint32_t*) Address) != p_data[i++])
		{
				MemoryProgramStatus = 0;

				return 1;
		}
		Address += 4;
	}

	FLASH_Lock();

	return 0;
}

int Internal_Flash_Read(uint32_t StartAddr,uint32_t *p_data,uint32_t size)
{
	uint32_t EndAddr=StartAddr+size*4;
	int MemoryProgramStatus=1;
	uint32_t Address = 0x0;
	int i=0;
	Address = StartAddr;
	while((Address < EndAddr) && (MemoryProgramStatus != 0))
	{
		p_data[i++]=(*(uint32_t*) Address);
		Address += 4;	
	}
	return 0;	
}

/////////////////////////////////copyright yuan Qinjing 2016-10-27////////////////////////////////////// 

