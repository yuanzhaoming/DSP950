#include <rtthread.h>
#include <board.h>

//#ifdef RT_USING_LWIP
//#include "stm32_eth.h"
//#endif /* RT_USING_LWIP */
//
#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"
//
//#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
//#include "msd.h"
//#endif /* RT_USING_DFS */
//
#define RT_USING_SPI3 
/*
 * SPI3_MOSI: PB5
 * SPI3_MISO: PB4
 * SPI3_SCK : PB3
 * SPI3_CS	: PA15  
*/
static void rt_hw_spi_init(void)
{
#ifdef RT_USING_SPI3

	static struct stm32_spi_bus stm32_spi3;
    /* register spi bus */
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        /* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO ,ENABLE);
	
  		//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SPI3,ENABLE);		

 		/*!< Configure SPI_FLASH_SPI pins: SCK */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		/*!< Configure SPI_FLASH_SPI pins: MISO */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		/*!< Configure SPI_FLASH_SPI pins: MOSI */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

        stm32_spi_register(SPI3, &stm32_spi3, "spi3");
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device3;
        static struct stm32_spi_cs  spi_cs3;
        GPIO_InitTypeDef GPIO_InitStructure;
        /* spi3: PA15 */
        spi_cs3.GPIOx = GPIOA;
        spi_cs3.GPIO_Pin = GPIO_Pin_15;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Pin = spi_cs3.GPIO_Pin;
        GPIO_Init(spi_cs3.GPIOx, &GPIO_InitStructure);

		GPIO_SetBits(spi_cs3.GPIOx, spi_cs3.GPIO_Pin);

        rt_spi_bus_attach_device(&spi_device3, "w25q16", "spi3", (void*)&spi_cs3);

    }
#endif /* RT_USING_SPI3 */
}
#endif 

void rt_platform_init(void)
{
#ifdef RT_USING_SPI
    rt_hw_spi_init();
#endif 


}

void rt_spi_flash_device_init(void)
{ 
	rt_hw_spi_init();

	w25qxx_init("spi3", "w25q16");



#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT) 
    w25qxx_init("flash0", "spi-elm");
#endif /* RT_USING_DFS && RT_USING_DFS_ELMFAT */
}























