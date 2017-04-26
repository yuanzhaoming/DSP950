#ifndef _spi_h_
#define _spi_h_

#if 1
enum{
	INPUT_TYPE_INIT = 0x00002105,
	INPUT_TYPE_AES = 0x00002105,
	INPUT_TYPE_SPDIF=0x00002205,
	INPUT_TYPE_COAX =0x00002305,
	INPUT_TYPE_DVD  =0x00002405,
	INPUT_TYPE_AD   =0x00002505,
	INPUT_TYPE_MIC  =0x00002605,//MIC��Ϊ����ʹ��
	INPUT_TEST      =0x00002705,//��������ģʽ����
	INPUT_MAX       =0x00002805,	
};
#else
enum{
	INPUT_TYPE_INIT = 0x00002105,
	INPUT_TYPE_AD = 0x00002105,
	INPUT_TYPE_AES=0x00002205,
	INPUT_TYPE_RCA =0x00002305,
	INPUT_TYPE_SPDIF  =0x00002405,
	INPUT_TYPE_COAX   =0x00002505,
	INPUT_TYPE_MIC  =0x00002605,//MIC��Ϊ����ʹ��
	INPUT_TEST      =0x00002705,//��������ģʽ����
	INPUT_MAX       =0x00002805,	
};

#endif

extern int g_init_done;


extern void SPI_Init(void);

extern void handle_spi_code(void);

#endif

