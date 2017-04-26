#ifndef _stm32_spi_instruction_dsp_h_
#define _stm32_spi_instruction_dsp_h_

#define SPI_INSTRUCTION_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_INSTRUCTION_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)

extern void SPI_DSP_SendBytes(unsigned char *buffer, unsigned int len);
extern unsigned int SPI_DSP_ReadByte(void);
extern void SPI_DSP_Init(void);

#endif



