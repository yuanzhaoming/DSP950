/*
 * inc.h
 *
 *  Created on: 2016-4-22
 * Corporation: NanJingQingJin
 *      Author: wanghb
 * Description: 定义通用结构、变量或其他资源
 */

#ifndef INC_H_
#define INC_H_

#include <stdio.h>

#define PROJECT_DEBUG

//定义全局打印信息,避免调试开关关闭时必要信息无法打印
#define DEBUG_GLOBAL
#define ERROR_GLOBAL

#ifdef DEBUG_GLOBAL
#define debug(fmt, ...) rt_kprintf(fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif /* MYSQL_DEBUG */

#ifdef ERROR_GLOBAL
#define error(fmt, ...) rt_kprintf("[ERROR][%s:%d]"fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define error(fmt, ...)
#endif /* ERROR_GLOBAL */

//定义程序版本号,格式:头(4字节)嵌入式程序版本(3字节)DSP程序版本(3字节)
extern char local_version[];

//定义数据库名称
#define DATABASE_NAME		"Test_db"

//定义数据库中表的名称
#define TABLE_GENERAL		"general_tb"
#define TABLE_INPUT1		"input1_tb"
#define TABLE_INPUT2		"input2_tb"
#define TABLE_INPUT3		"input3_tb"
#define TABLE_INPUT4		"input4_tb"
#define TABLE_INPUT5		"input5_tb"
#define TABLE_INPUT6		"input6_tb"
#define TABLE_INPUT7		"input7_tb"
#define TABLE_OUTPUT		"output_tb"
#define TABLE_OTHER			"other_tb"
#define TABLE_SOUND			"sound_tb"
#define TABLE_FERQDIV		"freqdiv_tb"

//定义协议命令长度
#define COMMAND_SIZE			4 //4Byte

#define TRANS_DSP_MASK	0x0001
#define TRANS_PC_MASK	0x0010
#define TRANS_OSC_MASK	0x0100
#define TRANS_ALL_MASK	(TRANS_DSP_MASK | TRANS_PC_MASK | TRANS_OSC_MASK)

typedef enum TransDataWhoEnum {
	TRANS_TO_DSP = TRANS_DSP_MASK,
	TRANS_TO_PC = TRANS_PC_MASK,
	TRANS_TO_OSC = TRANS_OSC_MASK,
	TRANS_TO_ALL = TRANS_ALL_MASK,
}TransDataEnum_t;

//定义发送命令(4Byte)函数类型
typedef int (*TransDataFunc_t)(const unsigned char *cmd, int len);

extern TransDataFunc_t SendData2DSP_Handle;
extern TransDataFunc_t SendData2PC_Handle;
extern TransDataFunc_t SendData2OSC_Handle;

//输出电平(音量),此变量在SPI(更新数据)和LCD显示都会用到,所以用全局变量
extern int g_output_level;

struct CmdData {
	unsigned int id;
	unsigned int data;
};

//定义硬件资源(SPI、KEY、UART等)打开状态
typedef enum  FileDescStateEnum {
	FD_CLOSE,
	FD_OPEN
}FileDescStateEnum_t;

//定义硬件资源信息结构
typedef struct FileDescInfStru {
	const char name[8];
	const char path[32];
	FileDescStateEnum_t state;
	int fd;
}FileDescInfStru_t;

//指令操作类型
enum CmdOperEnum {
	CMD_OPER_SET,		//设置类
	CMD_OPER_REQU,		//查询(请求)类
	CMD_OPER_RESP,		//应答类
	CMD_OPER_ERR,		//错误类
	CMD_OPER_MAX
};

//指令分组定义
enum CmdGroupEnum {
	CMD_GROUP_MIN = 1,
	CMD_GROUP_GENERAL = CMD_GROUP_MIN,
	CMD_GROUP_INPUT1,
	CMD_GROUP_INPUT2,
	CMD_GROUP_INPUT3,
	CMD_GROUP_INPUT4,
	CMD_GROUP_INPUT5,
	CMD_GROUP_INPUT6,
	CMD_GROUP_INPUT7,
	CMD_GROUP_RESERVED_1,
	CMD_GROUP_RESERVED_2,	  //10
	CMD_GROUP_OUTPUT,
	CMD_GROUP_RESERVED_3,
	CMD_GROUP_OTHER,
	CMD_GROUP_SOUND,
	CMD_GROUP_FERQ,
	CMD_GROUP_MAX = CMD_GROUP_FERQ,//15
};

//通道个数定义
#define INPUT_CHANNEL_COUNT		7
#define OUTPUT_CHANNEL_COUNT	4

//部分组内序号定义
enum GeneralGroupEnum {
	GENERAL_GROUP_SOUND_FIELD_X = 1,
	GENERAL_GROUP_SOUND_FIELD_Y,
	GENERAL_GROUP_SOUND_FIELD_Z,
	GENERAL_GROUP_SPEAKER_NUMBER,
	GENERAL_GROUP_SPEAKER_X,
	GENERAL_GROUP_SPEAKER_Y,
	GENERAL_GROUP_SPEAKER_Z,
	GENERAL_GROUP_KNOB_VOLUME,
	GENERAL_GROUP_INPUT_TYPE,
	GENERAL_GROUP_FADE_IN_TIME,
	GENERAL_GROUP_FADE_OUT_TIME,
	GENERAL_GROUP_SURROUND_TYPE,
	GENERAL_GROUP_OUT_PIN_ASSIGN,
	GENERAL_GROUP_MONITOR_OUT_STATE,
	GENERAL_GROUP_MUTE,
	GENERAL_GROUP_PC_SWITCH,
};

enum InputGroupEnum {
	INPUT_GROUP_SAMPLING_RATE = 1,
	INPUT_GROUP_DECODER,
	INPUT_GROUP_CHANNEL_NUMBER,
	INPUT_GROUP_PIN_ASSIGN_LIST,
	INPUT_GROUP_DELAY,
	INPUT_GROUP_PLAYBACK_SF,
};

enum OutputGroupEnum {
	OUTPUT_GROUP_FREQ_DIV_VAL = 1,
	OUTPUT_GROUP_LEVEL,
	OUTPUT_GROUP_DELAY,
	OUTPUT_GROUP_HIGH_CUT_FREQ,
	OUTPUT_GROUP_LOW_CUT_FREQ,
	OUTPUT_GROUP_HIGH_CUT_SLOPE,
	OUTPUT_GROUP_LOW_CUT_SLOPE,
	OUTPUT_GROUP_EQ_BAND_GAIN,
};

enum OtherGroupEnum {
	OTHER_GROUP_SPLREFERENCE_VALUE = 1,
	OTHER_GROUP_SPLALIGNMENT_VALUE,
	OTHER_GROUP_CUTLEVEL_CHANNELY,
	OTHER_GROUP_PLALIGNMENTENABLE_VALUE,
	OTHER_GROUP_CENTERSIGNALENABLE_VALUE,
	OTHER_GROUP_INVERTENABLE_VALUE,
	OTHER_GROUP_PLFILTER_FRENQUENCY,
	OTHER_GROUP_RESERVED_1,
	OTHER_GROUP_ALIGNMENT_CHANNEL,
	OTHER_GROUP_SIGNAL_MODEVAL,
	OTHER_GROUP_SYSTEM_STATE,
	OTHER_GROUP_RESERVED_2,
	OTHER_GROUP_OUTPUT_VOLUME,
	OTHER_GROUP_RESERVED_3,
	OTHER_GROUP_MICIN_BANDENG,
	OTHER_GROUP_INPUT_VOLUME,
};

enum FreqDivGroupEnum {
	FREQ_DIV_GROUP_01 = 1,
	FREQ_DIV_GROUP_02,
	FREQ_DIV_GROUP_03,
	FREQ_DIV_GROUP_04,
	FREQ_DIV_GROUP_05,
	FREQ_DIV_GROUP_06,
	FREQ_DIV_GROUP_07,
	FREQ_DIV_GROUP_08,
	FREQ_DIV_GROUP_09,
	FREQ_DIV_GROUP_10,
	FREQ_DIV_GROUP_11,
	FREQ_DIV_GROUP_12,
	FREQ_DIV_GROUP_13,
	FREQ_DIV_GROUP_14,
	FREQ_DIV_GROUP_15,
	FREQ_DIV_GROUP_16,
	FREQ_DIV_GROUP_17,
	FREQ_DIV_GROUP_18,
	FREQ_DIV_GROUP_19,
	FREQ_DIV_GROUP_20,
	FREQ_DIV_GROUP_21,
	FREQ_DIV_GROUP_22,
	FREQ_DIV_GROUP_23,
	FREQ_DIV_GROUP_24,
};

//定义命令参数位置及最大值和无效值(大于最大值)
enum CmdDataIndexEnum {
	CMD_DATA_INDEX_MIN = 0,
	CMD_DATA_INDEX_0 = CMD_DATA_INDEX_MIN,
	CMD_DATA_INDEX_1,
	CMD_DATA_INDEX_2,
	CMD_DATA_INDEX_MAX = CMD_DATA_INDEX_2,
	CMD_DATA_INDEX_INVALID,
};

//定义调用SQLite接口的操作用途或SQL语句的用途,主要是在SQL语句执行的回调函数中区分操作对应的处理方式
enum SqliteOperateId {
	ID_NULL,	//没有特定功能,可以不用处理
	ID_INIT,	//初始化DSP操作,回调函数中需要调用传输函数
	ID_ADD,		//向数据库添加数据(会首先查询数据是否存在,此时在回调中区分是否存在(调用回调数据就存在))
	ID_SHOW,	//普通的查看数据表中的数据
	ID_GET,		//获取数据表中的某个数据
	ID_EXPORT,	//导出数据(保存到文件)
};


/*
 *定义音频消息结构,与协议的4Byte数据定义保持一致
 *
 *协议处理时将收到的PC指令解析为该结构类型,获取到\
 *属性后需要转换为该结构类型,然后传输或使用
 */
typedef struct CmdMsgStru {
	unsigned int op;				//指令类型
	unsigned int key;				//指令分组
	unsigned int type;				//指令组内序号
	unsigned int param1;			//指令参数1
	unsigned int param2;			//指令参数2
	unsigned int param3;			//指令参数3
}CmdMsgStru_t;

#define MAX(iA, iB) (((iA) >= (iB)) ? (iA) : (iB))
#define MIN(iA, iB) (((iA) <= (iB)) ? (iA) : (iB))

//定义存储命令的数据库表表名和组序号的对应关系
struct GroupAndTableStru {
	unsigned int key; //组别
	struct CmdData *table; //保存对应类命令的数据库表表名
	unsigned int count;
};

/*
 *       Name: get_table_name
 *Description: 根据命令组号获取对应表名
 *  Parameter:
 *    iCmdKey: [in]命令组号
 *     Return:
 *       表名: 成功
 *       NULL: 失败
 */
struct GroupAndTableStru *get_table_name(unsigned int iCmdKey);
#define GET_TABLE_NAME(iCmdKey)	get_table_name(iCmdKey)

/*
 *       Name: get_cmd_index
 *Description: 根据命令组号和组内序号获取对应命令的参数位置[0 / 1 / 2]
 *  Parameter:
 *    iCmdKey: [in]命令组号
 *      iType: [in]组内序号
 *     Return:
 *       成功: 参数位置[CMD_DATA_INDEX_0 / CMD_DATA_INDEX_1 / CMD_DATA_INDEX_2]
 *       失败: CMD_DATA_INDEX_INVALID
 */
unsigned int get_cmd_index(unsigned int iCmdKey, unsigned int iCmdType);

/*
 *       Name: get_cmd_id
 *Description: 根据命令结构获取数据ID(命令组号和组内序号及部分参数组成,具体根据参数位置决定)
 *  Parameter:
 *       pMsg: [in]命令结构指针
 *     Return:
 *         ID: 成功
 *         -1: 失败
 */
int get_cmd_id(CmdMsgStru_t *pMsg);
#define GET_CMD_ID(pMsg)		get_cmd_id(pMsg)

//定义每一个命令描述的最大长度,由创建表时描述字段的长度
#define CMD_DESC_SIZE		50
#define CMD_DESC_DEFAULT	"[Default]New Data"

/*
 *       Name: get_cmd_desc
 *Description: 获取命令描述(或说明)
 *  Parameter:
 *       pMsg: [in]命令结构指针
 *      pDesc: [out]保存命令描述的缓冲区
 *     Return:
 *       成功: pDesc地址
 *       失败: NULL
 */
char *get_cmd_desc(CmdMsgStru_t *pMsg, char *pDesc);

/*
 *       Name: database_init
 *Description: 数据库初始化(打开数据库并创建表)
 *  Parameter: 无
 *     Return:
 *         0: 成功
 *         -1: 失败
 */
//int database_init();


/*
 *       Name: database_exit
 *Description: 数据库退出
 *  Parameter: 无
 *     Return: 无
 */
//void database_exit();

/*
 *       Name: database_reboot
 *Description: 数据库重启,用于写数据到磁盘
 *  Parameter: 无
 *     Return: 无
 */
//void database_reboot();

/*
 *            Name: transfer_handle_register
 *           Brief: 注册发送数据函数,需要注册的函数有向DSP、OSC和PC端发送数据的函数
 *       Parameter: 无
 *          Return: 无
 */
//void transfer_handle_register();

/*
 *      Name: resource_init
 *     Brief: 资源初始化,打开设备,包括SPI/KEY/UART等
 * Parameter: 无
 *    Return:
 *      成功: 0
 *      失败: -1
 */
//int resource_init();

/*
 *      Name: resource_exit
 *     Brief: 关闭各设备,资源回收
 * Parameter: 无
 *    Return: 无
 */
//void resource_exit();
		     
/*
 *      Name: dsp_init
 *     Brief: DSP初始化,获取所有配置数据并发送给DSP/PC等
 * Parameter: 无
 *    Return: 无
 */
//void dsp_init();

/*	  
*   
*	根据PISP1016的程序来看，输入接口顺序为:
*	#define AES_Digitalin 1 
*   #define RCA_AD 2 
*	#define DB25_AD 3 
*	#define Coax_SPDIF 4  
*	#define Fiber_SPDIF 5  
*	#define MIC_in 6	 	  
*	#define DVD_in 7  
*   
*/

#if 0
enum{
	INIT_INPUT = 0x00002105,
	AD_INPUT=0x00002105,
	AES_INPUT=0x00002205,
	RCA_INPUT=0x00002305,
	SPDIF_INPUT=0x00002405,
	COAX_INPUT=0x00002505,
	MIC_INPUT=0x00002605,
};
#else
enum{
	INIT_INPUT = 0x00002105,
	AES_INPUT=0x00002105,
	SPDIF_INPUT=0x00002205,
	COAX_INPUT=0x00002305,
	DVD_INPUT=0x00002405,
	AD_INPUT=0x00002505,
	MIC_INPUT=0x00002605,
};

#endif

extern void store_w25q16_volume_input_type( void );
extern void send_data_to_dsp( void );

extern void read_reboot_times(void);
extern void read_w25q16( void );

extern void send_data_to_pc(int i_connected);

extern void store_w25q16( void );

#endif /* INC_H_ */
