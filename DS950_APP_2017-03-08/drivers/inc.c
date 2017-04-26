#include "inc.h"
#include "do_data.h"
#include <stdio.h>
#include <rtthread.h>
					  
#include <lwip/sockets.h>  
#include <finsh.h>
#include "spi_flash.h"
#include "spi_instruction.h"
#include "protocol.h"
#include "app_cmd.h"

#ifdef PROJECT_DEBUG
#if 0
#define INC_DEBUG
#endif
#endif /* PROJECT_DEBUG */
							  
//#define INC_ERROR

#ifdef INC_DEBUG
#define inc_debug(fmt, ...) rt_kprintf("[INC][%d]"fmt"\n", __LINE__, ##__VA_ARGS__)
#else
#define inc_debug(fmt, ...)
#endif /* INC_DEBUG */

#ifdef INC_ERROR
#define inc_error(fmt, ...) rt_kprintf("[INC][ERR][%d]"fmt"\n", __LINE__, ##__VA_ARGS__)
#else
#define inc_error(fmt, ...)
#endif /* INC_ERROR */

//定义程序版本号,格式:头(4字节)-嵌入式程序版本(5字节)DSP程序版本(5字节),程序版本用主次版本号
//例如:WASV-00.01-00.01
char local_version[32] = "WASV-02.02-00.01";

//获取命令唯一ID(命令组号和组内序号组成)
#define GET_CMD_ID_P1(pMsg)		((pMsg->key << (8 + 5 + 5)) | (pMsg->type << (8 + 5)))
#define GET_CMD_ID_P2(pMsg)		((pMsg->key << (8 + 5 + 5)) | (pMsg->type << (8 + 5)) | (pMsg->param1 << 8))
#define GET_CMD_ID_P3(pMsg)		((pMsg->key << (8 + 5 + 5)) | (pMsg->type << (8 + 5)) | (pMsg->param1 << 8) | pMsg->param2)



#define flash_debug(value,fmt,...) \
	do{\
		if(value&0x02){\
		 		rt_kprintf("[%s],[%d]"fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);\
		}\
		else\
		{\
			 ;\
		}\
	}while(0);




/*
 *定义命令结构描述结构体
 *
 *根据组内序号,描述参数位置和组内序号对应的描述
 *注意:组内序号对应的描述是该类指令的名称,可以在导出数据时使用，但是不作为导入数据的索引
 */
typedef struct CmdDescStru {
	unsigned int type : 5;		//指令组内序号
	unsigned int index : 3;		//参数位置[0 / 1 / 2]
	char *type_desc;			//指令描述(指令组内序号对应名称)
}CmdDescStru_t;

#define CMD_DESC_INFO(iCmdType, iCmdIndex, iCmdDesc) {(iCmdType), (iCmdIndex), (iCmdDesc)}

//定义命令描述结构(CmdDescStru_t型数组首地址)和结构大小
struct CmdDescInfStru {
	const CmdDescStru_t *cmd_desc;	//命令结构描述结构体数组地址
	unsigned int cmd_desc_count;	//命令结构描述结构体数组大小
};

#define GROUP_AND_TABLE_INFO(iCmdType, pTable, iCount) {(iCmdType),(pTable), (iCount)}

static const CmdDescStru_t general_cmd_desc[] = {
		CMD_DESC_INFO(GENERAL_GROUP_SOUND_FIELD_X,			CMD_DATA_INDEX_1,	"SoundFieldX"),
		CMD_DESC_INFO(GENERAL_GROUP_SOUND_FIELD_Y,			CMD_DATA_INDEX_1,	"SoundFieldY"),
		CMD_DESC_INFO(GENERAL_GROUP_SOUND_FIELD_Z,			CMD_DATA_INDEX_1,	"SoundFieldZ"),
		CMD_DESC_INFO(GENERAL_GROUP_SPEAKER_NUMBER,			CMD_DATA_INDEX_0,	"SpeakerNumber"),
		CMD_DESC_INFO(GENERAL_GROUP_SPEAKER_X,				CMD_DATA_INDEX_1,	"SpeakerX"),
		CMD_DESC_INFO(GENERAL_GROUP_SPEAKER_Y,				CMD_DATA_INDEX_1,	"SpeakerY"),
		CMD_DESC_INFO(GENERAL_GROUP_SPEAKER_Z,				CMD_DATA_INDEX_1,	"SpeakerZ"),
		CMD_DESC_INFO(GENERAL_GROUP_KNOB_VOLUME,			CMD_DATA_INDEX_1,	"KnobVolume"),
		CMD_DESC_INFO(GENERAL_GROUP_INPUT_TYPE,				CMD_DATA_INDEX_0,	"InputXtype"),
		CMD_DESC_INFO(GENERAL_GROUP_FADE_IN_TIME,			CMD_DATA_INDEX_0,	"FadeInTime"),
		CMD_DESC_INFO(GENERAL_GROUP_FADE_OUT_TIME,			CMD_DATA_INDEX_0,	"FadeOutTime"),
		CMD_DESC_INFO(GENERAL_GROUP_SURROUND_TYPE,			CMD_DATA_INDEX_0,	"SurroundType"),
		CMD_DESC_INFO(GENERAL_GROUP_OUT_PIN_ASSIGN,			CMD_DATA_INDEX_2,	"OutPinAssign"),
		CMD_DESC_INFO(GENERAL_GROUP_MONITOR_OUT_STATE,		CMD_DATA_INDEX_0,	"MonitorOutState"),
		CMD_DESC_INFO(GENERAL_GROUP_MUTE,					CMD_DATA_INDEX_0,	"Mute"),
		CMD_DESC_INFO(GENERAL_GROUP_PC_SWITCH,				CMD_DATA_INDEX_0,	"PCSwitch"),
};

static const CmdDescStru_t input_cmd_desc[] = {
		CMD_DESC_INFO(INPUT_GROUP_SAMPLING_RATE,		CMD_DATA_INDEX_0,	"SamplingRate"),
		CMD_DESC_INFO(INPUT_GROUP_DECODER,				CMD_DATA_INDEX_0,	"Decoder"),
		CMD_DESC_INFO(INPUT_GROUP_CHANNEL_NUMBER,		CMD_DATA_INDEX_0,	"ChannelNumber"),
		CMD_DESC_INFO(INPUT_GROUP_PIN_ASSIGN_LIST,		CMD_DATA_INDEX_1,	"PinAssignList"),
		CMD_DESC_INFO(INPUT_GROUP_DELAY,				CMD_DATA_INDEX_1,	"Delay"),
		CMD_DESC_INFO(INPUT_GROUP_PLAYBACK_SF,			CMD_DATA_INDEX_0,	"PlaybackSF"),
};

static const CmdDescStru_t output_cmd_desc[] = {
		CMD_DESC_INFO(OUTPUT_GROUP_FREQ_DIV_VAL,		CMD_DATA_INDEX_2,	"FreqDivVal"),
		CMD_DESC_INFO(OUTPUT_GROUP_LEVEL,				CMD_DATA_INDEX_1,	"Level"),
		CMD_DESC_INFO(OUTPUT_GROUP_DELAY,				CMD_DATA_INDEX_1,	"Delay"),
		CMD_DESC_INFO(OUTPUT_GROUP_HIGH_CUT_FREQ,		CMD_DATA_INDEX_1,	"HighCutFreq"),
		CMD_DESC_INFO(OUTPUT_GROUP_LOW_CUT_FREQ,		CMD_DATA_INDEX_1,	"LowCutFreq"),
		CMD_DESC_INFO(OUTPUT_GROUP_HIGH_CUT_SLOPE,		CMD_DATA_INDEX_1,	"HighCutSlope"),
		CMD_DESC_INFO(OUTPUT_GROUP_LOW_CUT_SLOPE,		CMD_DATA_INDEX_1,	"LowCutSlope"),
		CMD_DESC_INFO(OUTPUT_GROUP_EQ_BAND_GAIN,		CMD_DATA_INDEX_2,	"EQBandGain"),
};

static const CmdDescStru_t freq_div_cmd_desc[] = {
		CMD_DESC_INFO(FREQ_DIV_GROUP_01,		CMD_DATA_INDEX_1,	"FreqDiv_01"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_02,		CMD_DATA_INDEX_1,	"FreqDiv_02"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_03,		CMD_DATA_INDEX_1,	"FreqDiv_03"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_04,		CMD_DATA_INDEX_1,	"FreqDiv_04"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_05,		CMD_DATA_INDEX_1,	"FreqDiv_05"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_06,		CMD_DATA_INDEX_1,	"FreqDiv_06"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_07,		CMD_DATA_INDEX_1,	"FreqDiv_07"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_08,		CMD_DATA_INDEX_1,	"FreqDiv_08"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_09,		CMD_DATA_INDEX_1,	"FreqDiv_09"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_10,		CMD_DATA_INDEX_1,	"FreqDiv_10"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_11,		CMD_DATA_INDEX_1,	"FreqDiv_11"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_12,		CMD_DATA_INDEX_1,	"FreqDiv_12"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_13,		CMD_DATA_INDEX_1,	"FreqDiv_13"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_14,		CMD_DATA_INDEX_1,	"FreqDiv_14"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_15,		CMD_DATA_INDEX_1,	"FreqDiv_15"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_16,		CMD_DATA_INDEX_1,	"FreqDiv_16"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_17,		CMD_DATA_INDEX_1,	"FreqDiv_17"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_18,		CMD_DATA_INDEX_1,	"FreqDiv_18"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_19,		CMD_DATA_INDEX_1,	"FreqDiv_19"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_20,		CMD_DATA_INDEX_1,	"FreqDiv_20"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_21,		CMD_DATA_INDEX_1,	"FreqDiv_21"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_22,		CMD_DATA_INDEX_1,	"FreqDiv_22"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_23,		CMD_DATA_INDEX_1,	"FreqDiv_23"),
		CMD_DESC_INFO(FREQ_DIV_GROUP_24,		CMD_DATA_INDEX_1,	"FreqDiv_24"),
};


struct CmdData g_GROUP_GENERAL[GENERAL_PARA_SIZE];

struct CmdData g_GROUP_INPUT1[INPUT_PARA_SIZE];
struct CmdData g_GROUP_INPUT2[INPUT_PARA_SIZE];
struct CmdData g_GROUP_INPUT3[INPUT_PARA_SIZE];
struct CmdData g_GROUP_INPUT4[INPUT_PARA_SIZE];
struct CmdData g_GROUP_INPUT5[INPUT_PARA_SIZE];
struct CmdData g_GROUP_INPUT6[INPUT_PARA_SIZE];
struct CmdData g_GROUP_INPUT7[INPUT_PARA_SIZE];

struct CmdData g_GROUP_OUTPUT[OUTPUT_PARA_SIZE];
struct CmdData g_GROUP_OTHER[OTHER_PARA_SIZE];
struct CmdData g_GROUP_SOUND[SOUND_PARA_SIZE];
struct CmdData g_GROUP_FREQDIV[FREQDIV_PARA_SIZE];


struct GroupAndTableStru group_table[] = {
		GROUP_AND_TABLE_INFO(CMD_GROUP_GENERAL,		g_GROUP_GENERAL, GENERAL_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_INPUT1,		g_GROUP_INPUT1, INPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_INPUT2,		g_GROUP_INPUT2, INPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_INPUT3,		g_GROUP_INPUT3, INPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_INPUT4,		g_GROUP_INPUT4, INPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_INPUT5,		g_GROUP_INPUT5, INPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_INPUT6,		g_GROUP_INPUT6, INPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_INPUT7,		g_GROUP_INPUT7, INPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_OUTPUT,		g_GROUP_OUTPUT, OUTPUT_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_OTHER,		g_GROUP_OTHER, OTHER_PARA_SIZE),		
		GROUP_AND_TABLE_INFO(CMD_GROUP_SOUND,		g_GROUP_SOUND, SOUND_PARA_SIZE),
		GROUP_AND_TABLE_INFO(CMD_GROUP_FERQ,		g_GROUP_FREQDIV, FREQDIV_PARA_SIZE),
	 
};

//表的总个数
#define TABLES_COUNT		(sizeof(group_table) / sizeof(group_table[0]))

//定义发送命令函数(句柄)
TransDataFunc_t SendData2DSP_Handle = NULL;
TransDataFunc_t SendData2PC_Handle = NULL;
TransDataFunc_t SendData2OSC_Handle = NULL;


 void store_w25q16_volume_input_type( void );

/*
* 	这里是数据存储
*
*		
*/
void store_w25q16( void )
{	  
	//模拟测试，只需测试通用参数部分就行了
	int i = 0;
	//int start_addr = SPI_CODE_BASE_ADDR;
	unsigned int *c_data = NULL;
//	unsigned char checksum_seed[4];
	unsigned int checksum_data = 0;
	struct GroupAndTableStru *table_name = NULL;
	int j = 0;
	int flash_offset = 0;

	//写第一个片区
	//擦除校验和区域
	SPI_FLASH_SectorErase(SPI_CODE_BASE_ADDR);
	//擦出数据区域
	for(i = 0 ; i < SECTOR_NUMBER ; i ++)
		SPI_FLASH_SectorErase((SPI_CODE_BASE_ADDR + 0x1000 + i * 0x1000));

	for (i = 0; i < (sizeof(group_table) / sizeof(group_table[0])); ++i)
	{
		table_name = &group_table[i];
		for (j = 0; j < table_name->count; ++j)
		{
			c_data = &table_name->table[j].data;
			
			checksum_data +=  *c_data;

			SPI_FLASH_BufferWrite((unsigned char *)c_data, (SPI_CODE_BASE_ADDR + 0x1000) + flash_offset * 4, 4);

			//校验和也加上这个数据
			//SPI_FLASH_BufferRead(checksum_seed, (SPI_CODE_BASE_ADDR + 0x1000) + flash_offset * 4, 4);
			//checksum_data += CheckSum(checksum_seed,4);
			flash_offset++;

			if(table_name->table[j].id == 0)
				continue;
			
			inc_debug("[store 11]val=0x%08X", *c_data);	
		}
	}
	flash_debug(get_debug_value(),"flash_offset：%d, SPI_CODE_SIZE:%d \n", flash_offset,SPI_CODE_SIZE);

	//这是写校验和区数据
	SPI_FLASH_BufferWrite((unsigned char *)&checksum_data, (SPI_CODE_BASE_ADDR), 4);

	{
		unsigned int checksum1 = 0;
		SPI_FLASH_BufferRead((unsigned char *)&checksum1, (SPI_CODE_BASE_ADDR), 4);
		flash_debug(get_debug_value(),"--------------read checksum 1------0x%08x-----------------\r\n",checksum1);	
	}


	flash_debug(get_debug_value(),"----1-------checksum_data: 0x%08x------------\n", checksum_data);

	/****************************写第二个片区的数据内容********************************************/
	//写第二个片区
	//擦除CRC区域
	flash_offset = 0;
	checksum_data = 0;

	SPI_FLASH_SectorErase((SPI_CODE_BASE_ADDR + 0x1000 + 0x1000 * SECTOR_NUMBER));
	//擦除数据区域
	for(i = 0 ; i < SECTOR_NUMBER ; i ++)
		SPI_FLASH_SectorErase(((SPI_CODE_BASE_ADDR + 0x1000 * SECTOR_NUMBER + 0x1000) + 0x1000 + i * 0x1000));

	for (i = 0; i < (sizeof(group_table) / sizeof(group_table[0])); ++i)
	{
		table_name = &group_table[i];
		for (j = 0; j < table_name->count; ++j)
		{
			c_data = &table_name->table[j].data;

			checksum_data +=  *c_data;
		//	inc_debug("-----------c_data: 0x%08x------------\n", *c_data);	
			SPI_FLASH_BufferWrite((unsigned char *)c_data, 
							((SPI_CODE_BASE_ADDR + 0x1000 * SECTOR_NUMBER + 0x1000) + 0x1000) + flash_offset * 4, 4);
			//SPI_FLASH_BufferRead(checksum_seed, 
			//				((SPI_CODE_BASE_ADDR + 0x1000 * SECTOR_NUMBER + 0x1000) + 0x1000) + flash_offset * 4, 4);
			//checksum_data += CheckSum(checksum_seed,4);
			flash_offset++;
			if(table_name->table[j].id == 0)
				continue;	
			inc_debug("[store 21]val=0x%08X", *c_data);	
		}
	}
	//这是写校验和区数据
	SPI_FLASH_BufferWrite((unsigned char *)&checksum_data, ((SPI_CODE_BASE_ADDR + 0x1000 + 0x1000 * SECTOR_NUMBER)), 4);


	{
		unsigned char arr[4];
		SPI_FLASH_BufferRead(arr, (((SPI_CODE_BASE_ADDR + 0x1000 + 0x1000 * SECTOR_NUMBER))), 4);
		flash_debug(get_debug_value(),"--------------read checksum 2------0x%08x-----------------\r\n",(arr[3]<<24)+(arr[2]<<16)+(arr[1]<<8)+(arr[0]));	
	}	    	 

	flash_debug(get_debug_value(),"----2-------checksum_data: 0x%08x------------\n", checksum_data);


	store_w25q16_volume_input_type();
}

/*
* 	这里是数据存储
*
*		
*/
void store_w25q16_volume_input_type( void )
{
	//模拟测试，只需测试通用参数部分就行了
//	int i = 0;
	extern int g_input_type;
	//int start_addr = SPI_CODE_BASE_ADDR;
	unsigned int c_data ;
//	unsigned char checksum_seed[4];
	unsigned int checksum_data = 0;
	//struct GroupAndTableStru *table_name = NULL;
//	int j = 0;
//	int flash_offset = 0;
	//写音量和输入类型数据
	//写第一个片区
	//擦除校验和区域
	checksum_data = 0;
	SPI_FLASH_SectorErase(VOLUME_INPUT_BASE_ADDR);
	//擦出数据区域
	SPI_FLASH_SectorErase((VOLUME_INPUT_BASE_ADDR + 0x1000));
	//获取音量数据，input_type数据
	{
		//音量数据
		c_data = do_get_data(1,8);

		//rt_kprintf("\n-----------volume:0x%0x------------\n",c_data);

		SPI_FLASH_BufferWrite((unsigned char *)&c_data,VOLUME_INPUT_BASE_ADDR + 0x1000,4);
		checksum_data +=  c_data;
		//输入类型数据
		//c_data = do_get_data(1,9);

		//rt_kprintf("\n-----------input_type:0x%0x------------\n",g_input_type);
		SPI_FLASH_BufferWrite((unsigned char *)&g_input_type,VOLUME_INPUT_BASE_ADDR + 0x1000 + 4,4);
		checksum_data +=  g_input_type;
		//写校验和
		SPI_FLASH_BufferWrite((unsigned char *)&checksum_data, (VOLUME_INPUT_BASE_ADDR), 4);
		//rt_kprintf("\n----------1--------volume:0x%0x------------\n",g_volume);
	}

	//写第二个片区
	//擦除校验和区域
	checksum_data = 0;
	SPI_FLASH_SectorErase(VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000);
	//擦出数据区域
	SPI_FLASH_SectorErase(VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000 + 0x1000);
	//获取音量数据，input_type数据
	{
		//音量数据
		c_data = do_get_data(1,8);

		//rt_kprintf("\n-----------volume:0x%0x------------\n",c_data);

		SPI_FLASH_BufferWrite((unsigned char *)&c_data,VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000 + 0x1000,4);
		checksum_data +=  c_data;
		//输入类型数据
		//c_data = do_get_data(1,9);
		//rt_kprintf("\n-----------input_type:0x%0x------------\n",g_input_type);
		SPI_FLASH_BufferWrite((unsigned char *)&g_input_type,VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000 + 0x1000 + 4,4);
		checksum_data +=  g_input_type;
		//写校验和
		SPI_FLASH_BufferWrite((unsigned char *)&checksum_data, (VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000), 4);
		//rt_kprintf("\n-----------2-------volume:0x%0x------------\n",g_volume);
	}
}


/*
*
* 	说明:
*		 获取系统启动的次数
*
*/
int g_system_reboot_times = 0;
void read_reboot_times(void)
{
	SPI_FLASH_BufferRead((unsigned char *)&g_system_reboot_times, REBOOT_TIMES_ADDR, 4);
	if((g_system_reboot_times == 0xffffff) || (g_system_reboot_times == -1))
		g_system_reboot_times = 0;
	//rt_kprintf("\n --------0x%08x--------- \n",g_system_reboot_times);
	g_system_reboot_times += 1;
	SPI_FLASH_SectorErase(REBOOT_TIMES_ADDR);
	SPI_FLASH_BufferWrite((unsigned char *)&g_system_reboot_times,REBOOT_TIMES_ADDR,4);		
}

void reboot_times()
{
	rt_kprintf("\n reboot times: %d \n",g_system_reboot_times);	
}
FINSH_FUNCTION_EXPORT(reboot_times, show system reboot times);







/*
*
* 	读取w25q16的数据
*
*	逻辑:(2016-11-09)
*		1.先读取两个校验和区 比较校验和是否相等，相等，则读取任何一个区就可以了
*		2.如果不相等，则读取数据区，看那一个数据区和校验和相等，取校验和是正确的区即可
*
*/
void read_w25q16( void )
{			  
	int i = 0;
	//int start_addr = 0xA000;
	int flash_offset = 0;
//	unsigned char c_temp[4];
	int value;
	unsigned int checksum_seed;
	unsigned int crc_data = 0;
	unsigned int checksum1 = 0;
	unsigned int checksum2 = 0;
	//先读取校验和部分
	SPI_FLASH_BufferRead((unsigned char *)&checksum1, (SPI_CODE_BASE_ADDR), 4);
	SPI_FLASH_BufferRead((unsigned char *)&checksum2, ((SPI_CODE_BASE_ADDR + 0x1000 + 0x1000 * SECTOR_NUMBER)), 4);

	if((checksum1 == checksum2) && (checksum1 != 0xffffffff) )
	{
		inc_debug("--------------read checksum ok------0x%08x-----------------\r\n",checksum1);	
		//下面只需要读取第一个区内容即可
		for(i = 0 ;i < 	(SPI_CODE_SIZE); i ++)
		{	
			SPI_FLASH_BufferRead((unsigned char *)&value, (SPI_CODE_BASE_ADDR + 0x1000) + i * 4, 4);
	
			if((value == 0xffffffff) || (value == 0))
				continue;
	
			//inc_debug("-------value = 0x%0x-------\n",value);
			do_set_cmd_entry(SET_ENTRY_CMD, &value);	
		}
	}
	else
	{
		//需要检测两个区的校验和数据
		for(i = 0 ;i < 	(SPI_CODE_SIZE); i ++)
		{
			SPI_FLASH_BufferRead((unsigned char *)&checksum_seed, ((SPI_CODE_BASE_ADDR + 0x1000)) + flash_offset * 4, 4);
			flash_offset++;
		   	//计算校验和
			crc_data += checksum_seed;//CheckSum(checksum_seed,4);	
		}
		
		//按照存储的数据量来看，不可能为0xffffff
		if((checksum1 == crc_data) && (checksum1 != 0xffffff))
		{
			inc_debug("--------------read checksum sector 1 ok-----------------------\r\n");	
			for(i = 0 ;i < 	(SPI_CODE_SIZE); i ++)
			{	
				SPI_FLASH_BufferRead((unsigned char *)&value, (SPI_CODE_BASE_ADDR + 0x1000) + i * 4, 4);
		
				if((value == 0xffffffff) || (value == 0))
					continue;
				//inc_debug("-------value = 0x%0x-------\n",value);
				do_set_cmd_entry(SET_ENTRY_CMD, &value);
				
				goto back1;		
			}					
		}
		else
		{
			inc_debug("------crc_data:0x%08x,checksum1:0x%08x----------\r\n",crc_data,checksum1);
		}
		//注意，如果第一部分数据读出来的是正确的，那么就要考虑将数据在读出来的时候，就要拷贝到第二个区域，防止这么个情况:
		/*
		* 	第一次存储，在要存第二个区域的时候，电就给断掉了
		*	第二次开机，再要存储的时候，存第一个区，又将电断掉了，这个时候，数据全没了。这样的情况要杜绝出现。
		*
		*	可以不考虑,这个问题出现的可能，除非是w25q16出现问题了。
		*/	
		//读第二个区的数据
		crc_data = 0;
		flash_offset = 0;

		for(i = 0 ;i < (SPI_CODE_SIZE); i ++)
		{
			SPI_FLASH_BufferRead((unsigned char *)&checksum_seed, ((SPI_CODE_BASE_ADDR + 0x1000 + 0x1000 * SECTOR_NUMBER ) + 0x1000) + flash_offset * 4, 4);
			flash_offset++;
		   	//计算校验和
			crc_data += checksum_seed;//CheckSum(checksum_seed,4);	
		}
		
		//按照存储的数据量来看，不可能为0xffffff
		if((checksum2 == crc_data) && (checksum2 != 0xffffff))
		{
			inc_debug("--------------read checksum sector 2 ok-----------------------\r\n");	

			for(i = 0 ;i < 	(SPI_CODE_SIZE); i ++)
			{	
				SPI_FLASH_BufferRead((unsigned char *)&value, ((SPI_CODE_BASE_ADDR + 0x1000 + 0x1000 * SECTOR_NUMBER + 0x1000)) + i * 4, 4);
		
				if((value == 0xffffffff) || (value == 0))
					continue;             
				//inc_debug("-------value = 0x%0x-------\n",value);
				do_set_cmd_entry(SET_ENTRY_CMD, &value);
				
				goto back1;		
			}
		}
		else
		{
			inc_debug("------crc_data:0x%08x,checksum2:0x%08x----------\r\n",crc_data,checksum2);
		}
	}

back1:
	//第二部分就是读取音量数据和输入类型
	SPI_FLASH_BufferRead((unsigned char *)&checksum1, (VOLUME_INPUT_BASE_ADDR), 4);
	SPI_FLASH_BufferRead((unsigned char *)&checksum2, ((VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000)), 4);
	if((checksum1 == checksum2) && (checksum1 != 0xffffffff) )
	{
		inc_debug("--------------read volume checksum ok------0x%08x-----------------\r\n",checksum1);	
		SPI_FLASH_BufferRead((unsigned char *)&value, (VOLUME_INPUT_BASE_ADDR + 0x1000), 4);
		inc_debug("-------value = 0x%0x-------\n",value);
		do_set_cmd_entry(SET_ENTRY_CMD, &value);
		//设置输入类型
		SPI_FLASH_BufferRead((unsigned char *)&value, (VOLUME_INPUT_BASE_ADDR + 0x1000 + 4), 4);
		//rt_kprintf("-------value = 0x%0x-------\n",value);
		if(	(value < INIT_INPUT) || (value > MIC_INPUT))
		{
			value = INIT_INPUT;	
		}
		
		do_set_cmd_entry(SET_ENTRY_CMD, &value);
	}
	else
	{
		//需要检测两个区的校验和数据
		crc_data = 0;
		for(i = 0 ;i < 	(2); i ++)
		{
			SPI_FLASH_BufferRead((unsigned char *)&checksum_seed, ((VOLUME_INPUT_BASE_ADDR + 0x1000)) + i * 4, 4);
		   	//计算校验和
			crc_data += checksum_seed;//CheckSum(checksum_seed,4);	
		}
		if((checksum1 == crc_data) && (checksum1 != 0xffffff))
		{
			inc_debug("--------------read volume checksum ok------0x%08x-----------------\r\n",checksum1);	
			SPI_FLASH_BufferRead((unsigned char *)&value, (VOLUME_INPUT_BASE_ADDR + 0x1000), 4);
			//inc_debug("-------value = 0x%0x-------\n",value);
			do_set_cmd_entry(SET_ENTRY_CMD, &value);
			//设置输入类型
			SPI_FLASH_BufferRead((unsigned char *)&value, (VOLUME_INPUT_BASE_ADDR + 0x1000 + 4), 4);
			//若输入类型不在AD_INPUT和MIC_INPUT范围内，则设置为AD输入
			if(	(value < INIT_INPUT) || (value > MIC_INPUT))
			{
				value = INIT_INPUT;	
			}			

			//rt_kprintf("-------value = 0x%0x-------\n",value);
			do_set_cmd_entry(SET_ENTRY_CMD, &value);
			return;
		}

		//检测第二个区的数据
		crc_data = 0;
		for(i = 0 ;i < 	(2); i ++)
		{
			SPI_FLASH_BufferRead((unsigned char *)&checksum_seed, ((VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000 + 0x1000)) + i * 4, 4);
		   	//计算校验和
			crc_data += checksum_seed;//CheckSum(checksum_seed,4);	
		}
		if((checksum2 == crc_data) && (checksum2 != 0xffffff))
		{
			inc_debug("--------------read volume 2 checksum ok------0x%08x-----------------\r\n",checksum2);	
			SPI_FLASH_BufferRead((unsigned char *)&value, (VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000 + 0x1000), 4);

			//inc_debug("-------value = 0x%0x-------\n",value);
			do_set_cmd_entry(SET_ENTRY_CMD, &value);
			//设置输入类型
			SPI_FLASH_BufferRead((unsigned char *)&value, (VOLUME_INPUT_BASE_ADDR + 0x1000 + 0x1000 + 0x1000 + 4), 4);
			if(	(value < INIT_INPUT) || (value > MIC_INPUT))
			{
				value = INIT_INPUT;	
			}	

			//inc_debug("-------value = 0x%0x-------\n",value);
			do_set_cmd_entry(SET_ENTRY_CMD, &value);

			return;
		}
	}
	//什么都不对，啥都不用管了，退出即可  2016-11-09
}


//输出电平(音量),此变量在SPI(更新数据)和LCD显示都会用到,所以用全局变量
int g_output_level = 0;

struct GroupAndTableStru *get_table_name(unsigned int iCmdKey)
{
	int i = 0;
	struct GroupAndTableStru *p = NULL;

	if ((iCmdKey < CMD_GROUP_MIN) || (iCmdKey > CMD_GROUP_MAX))
		return NULL;
	else {
		for (i = 0; i < (sizeof(group_table) / sizeof(group_table[0])); ++i) {
			if (group_table[i].key == iCmdKey)
				p = &group_table[i];
		}
	}

	return p;
}

/*
 *       Name: get_cmd_desc_s
 *Description: 根据命令组号获取对应命令描述结构数组和数组大小
 *  Parameter:
 *    iCmdKey: [in]命令组号
 *pCmdDescInf: [out]保存命令描述结构信息缓冲区
 *     Return:
 *          0: 成功
 *         -1: 失败
 */
static int get_cmd_desc_s(unsigned int iCmdKey, struct CmdDescInfStru *pCmdDescInf)
{
	const CmdDescStru_t *cmd_desc = NULL;
	unsigned int cmd_desc_count = 0;

	if (pCmdDescInf == NULL)
		return -1;

	switch (iCmdKey) {
	case CMD_GROUP_GENERAL:
		cmd_desc = general_cmd_desc;
		cmd_desc_count = sizeof(general_cmd_desc) / sizeof(general_cmd_desc[0]);
		break;
	case CMD_GROUP_INPUT1:
	case CMD_GROUP_INPUT2:
	case CMD_GROUP_INPUT3:
	case CMD_GROUP_INPUT4:
	case CMD_GROUP_INPUT5:
	case CMD_GROUP_INPUT6:
	case CMD_GROUP_INPUT7:
		cmd_desc = input_cmd_desc;
		cmd_desc_count = sizeof(input_cmd_desc) / sizeof(input_cmd_desc[0]);
		break;
	case CMD_GROUP_OUTPUT:
		cmd_desc = output_cmd_desc;
		cmd_desc_count = sizeof(output_cmd_desc) / sizeof(output_cmd_desc[0]);
		break;
	case CMD_GROUP_OTHER:
		break;
	case CMD_GROUP_SOUND:
		break;
	case CMD_GROUP_FERQ:
		cmd_desc = freq_div_cmd_desc;
		cmd_desc_count = sizeof(freq_div_cmd_desc) / sizeof(freq_div_cmd_desc[0]);
		break;
	default:
		break;
	}

	if ((cmd_desc != NULL) && (cmd_desc_count != 0)) {
		pCmdDescInf->cmd_desc = cmd_desc;
		pCmdDescInf->cmd_desc_count = cmd_desc_count;

		return 0;
	}

	return -1;
}

unsigned int get_cmd_index(unsigned int iCmdKey, unsigned int iCmdType)
{
	unsigned int index = CMD_DATA_INDEX_INVALID;
	struct CmdDescInfStru cmd_desc_inf = {0};
	int i = 0;

	if (get_cmd_desc_s(iCmdKey, &cmd_desc_inf) < 0) {
		inc_error("get_cmd_desc_s Error, Key error");

		return index;
	}
	inc_debug("cmd_desc = 0x%X, cmd_desc_count = %d", (unsigned int)cmd_desc_inf.cmd_desc, cmd_desc_inf.cmd_desc_count);

	for (i = 0; i < cmd_desc_inf.cmd_desc_count; ++i) {
		if (cmd_desc_inf.cmd_desc[i].type == iCmdType) {
			index = cmd_desc_inf.cmd_desc[i].index;
			break;
		}
	}

	//if ((index < CMD_DATA_INDEX_MIN ) || (index > CMD_DATA_INDEX_MAX)) {
	//	index = CMD_DATA_INDEX_INVALID;
	//}
	if ((index > CMD_DATA_INDEX_MAX)) {
		index = CMD_DATA_INDEX_INVALID;
	}
	return index;
}

int get_cmd_id(CmdMsgStru_t *pMsg)
{
	unsigned int index = CMD_DATA_INDEX_INVALID;
	int id = -1;

	if (pMsg == NULL)
		return -1;

	if ((index = get_cmd_index(pMsg->key, pMsg->type)) == CMD_DATA_INDEX_INVALID) {
		inc_error("Get index invalid");

		return -1;
	}

	inc_debug("index = %d", index);

	switch (index) {
	case CMD_DATA_INDEX_0:
		id = GET_CMD_ID_P1(pMsg);
		break;
	case CMD_DATA_INDEX_1:
		id = GET_CMD_ID_P2(pMsg);
		break;
	case CMD_DATA_INDEX_2:
		id = GET_CMD_ID_P3(pMsg);
		break;
	default:
		break;
	}

	return id;
}

char *get_cmd_desc(CmdMsgStru_t *pMsg, char *pDesc)
{
	struct CmdDescInfStru cmd_desc_inf = {0};
	const char *cmd_desc_head = NULL;
	unsigned int index = CMD_DATA_INDEX_INVALID;
	int i = 0;

	if ((pMsg == NULL) || (pDesc == NULL))
		return NULL;

	if (get_cmd_desc_s(pMsg->key, &cmd_desc_inf) < 0) {
		inc_error("get_cmd_desc_s Error, Key error");

		return NULL;
	}
	inc_debug("cmd_desc = 0x%X, cmd_desc_count = %d", (unsigned int)cmd_desc_inf.cmd_desc, cmd_desc_inf.cmd_desc_count);

	//从命令描述结构数组中获取描述头和数据存放位置
	for (i = 0; i < cmd_desc_inf.cmd_desc_count; ++i) {
		if (cmd_desc_inf.cmd_desc[i].type == pMsg->type) {
			cmd_desc_head = cmd_desc_inf.cmd_desc[i].type_desc;
			index = cmd_desc_inf.cmd_desc[i].index;

			break;
		}
	}

	if (i >= cmd_desc_inf.cmd_desc_count) {
		//没有找到与命令相符的描述,组内序号无效
		inc_error("Not find description, Type error");

		return NULL;
	}

	if (cmd_desc_head == NULL) {
		inc_error("Get cmd_desc_head invalid");

		return NULL;
	}

//	if ((index < CMD_DATA_INDEX_MIN) || (index > CMD_DATA_INDEX_MAX)) {
//		inc_error("Get index invalid");
//
//		return NULL;
//	}
	if ((index > CMD_DATA_INDEX_MAX)) {
		inc_error("Get index invalid");

		return NULL;
	}

	inc_debug("cmd_desc_head = %s, index = %d", cmd_desc_head, index);

	switch (index) {
	case CMD_DATA_INDEX_0:
		sprintf(pDesc, "%s", cmd_desc_head);
		break;
	case CMD_DATA_INDEX_1:
		sprintf(pDesc, "%s_%02d", cmd_desc_head, pMsg->param1);
		break;
	case CMD_DATA_INDEX_2:
		sprintf(pDesc, "%s_%02d_%02d", cmd_desc_head, pMsg->param1, pMsg->param2);
		break;
	default:
		break;
	}

	inc_debug("cmd_desc: %s", pDesc);

	return pDesc;
}


#include <stdlib.h> //system()
void database_reboot()
{

}

/*
 *          Name: transfer_fun_register
 *         Brief: 注册发送数据函数,需要注册的函数有向DSP、OSC和PC端发送数据的函数
 *     Parameter:
 *          iWho: [in]发送的目的端(DSP、OSC和PC)
 *    pFunHandle: [in]发送数据(数据或命令)的函数指针
 *        Return: 无
 */
static void transfer_fun_register(TransDataEnum_t iWho, TransDataFunc_t pFunHandle)
{
	if (iWho & TRANS_DSP_MASK) {
		SendData2DSP_Handle = pFunHandle;
	} else if (iWho & TRANS_PC_MASK) {
		SendData2PC_Handle = pFunHandle;
	} else if (iWho & TRANS_OSC_MASK) {
		SendData2OSC_Handle = pFunHandle;
	} else {
		inc_error("inc_transfer_fun_register Error");

		return;
	}
}	   

/*
*
*
*
*
*
*/
//void send_data_to_pc(int i_connected,unsigned char *buf, int len)
void send_data_to_pc(int i_connected)
{
	int i = 0 ;
	unsigned int *send_data ;
	int j = 0;
	struct GroupAndTableStru *table_name = NULL;
	
	for (i = 0; i < (sizeof(group_table) / sizeof(group_table[0])); ++i)
	{
		table_name = &group_table[i];
		for (j = 0; (j < table_name->count) && (table_name->table[j].id != 0); ++j)
		{
			send_data = &table_name->table[j].data;
			send(i_connected, (unsigned char *)send_data, 4 , 0);
			inc_debug(">>>0x%08X", *send_data);	
		}
	}	
}


void send_data_to_dsp(   )
{
	int i = 0 ;
	unsigned int *send_data ;
	int i_value;
	int j = 0;
	struct GroupAndTableStru *table_name = NULL;
	
	for (i = 0; i < (sizeof(group_table) / sizeof(group_table[0])); ++i)
	{
		table_name = &group_table[i];
		for (j = 0; (j < table_name->count) && (table_name->table[j].id != 0); ++j)
		{
			send_data = &table_name->table[j].data;
			i_value = *send_data;
			//send(i_connected, (unsigned char *)send_data, 4 , 0);
			i_value = ((i_value >> 24) & 0xff) | (((i_value >> 16) & 0xff) << 8) | ((i_value & 0xff) << 24 ) | (((i_value >> 8) & 0xff) << 16);

			SPI_DSP_SendBytes((unsigned char *)&i_value,4);
			//rt_thread_delay(10);

			//rt_kprintf(">>>0x%08X", i_value);		
		}
	}
	
	//另外发送一条指令,表示命令结束
	i_value = 0x34000000;
	SPI_DSP_SendBytes((unsigned char *)&i_value,4);

	//rt_kprintf(">>>end sending instruction\n");	
		
}

											  


//注销发送数据函数
#define transfer_fun_unregister(iWho) transfer_fun_register(iWho, NULL)

void transfer_handle_register()
{
	//transfer_fun_register(TRANS_TO_DSP, (TransDataFunc_t)spi_send_data);
	transfer_fun_register(TRANS_TO_PC, (TransDataFunc_t)send_data_to_pc);
}



		   
void show_arr()
{
//	int i = 0;
	int j = 0;
//	struct GroupAndTableStru *table_name = NULL;
	
#if 0
	for (i = 0; i < (sizeof(group_table) / sizeof(group_table[0])); ++i)
	{
		table_name = &group_table[i];
		inc_debug("-------------------------------------------");
		for (j = 0; (j < table_name->count) && (table_name->table[j].id != 0); ++j)
		{
			inc_debug("[store]val=0x%08X", table_name->table[j].data);	
		}
	}
#endif
	CmdMsgStru_t msg;
	inc_debug("-------------------------------------------");
	for (j = 0; (j < FREQDIV_PARA_SIZE) && (g_GROUP_FREQDIV[j].id != 0); ++j)
	{
	//	inc_debug("[store][%d]val=0x%08X", j, g_GROUP_FREQDIV[j].data);
		prot_cmd_to_msg((unsigned char *)&g_GROUP_FREQDIV[j].data, &msg);	
	}

	inc_debug("\n");
	 
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(show_arr, show data);
#endif






