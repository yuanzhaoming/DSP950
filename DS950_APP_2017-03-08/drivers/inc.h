/*
 * inc.h
 *
 *  Created on: 2016-4-22
 * Corporation: NanJingQingJin
 *      Author: wanghb
 * Description: ����ͨ�ýṹ��������������Դ
 */

#ifndef INC_H_
#define INC_H_

#include <stdio.h>

#define PROJECT_DEBUG

//����ȫ�ִ�ӡ��Ϣ,������Կ��عر�ʱ��Ҫ��Ϣ�޷���ӡ
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

//�������汾��,��ʽ:ͷ(4�ֽ�)Ƕ��ʽ����汾(3�ֽ�)DSP����汾(3�ֽ�)
extern char local_version[];

//�������ݿ�����
#define DATABASE_NAME		"Test_db"

//�������ݿ��б������
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

//����Э�������
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

//���巢������(4Byte)��������
typedef int (*TransDataFunc_t)(const unsigned char *cmd, int len);

extern TransDataFunc_t SendData2DSP_Handle;
extern TransDataFunc_t SendData2PC_Handle;
extern TransDataFunc_t SendData2OSC_Handle;

//�����ƽ(����),�˱�����SPI(��������)��LCD��ʾ�����õ�,������ȫ�ֱ���
extern int g_output_level;

struct CmdData {
	unsigned int id;
	unsigned int data;
};

//����Ӳ����Դ(SPI��KEY��UART��)��״̬
typedef enum  FileDescStateEnum {
	FD_CLOSE,
	FD_OPEN
}FileDescStateEnum_t;

//����Ӳ����Դ��Ϣ�ṹ
typedef struct FileDescInfStru {
	const char name[8];
	const char path[32];
	FileDescStateEnum_t state;
	int fd;
}FileDescInfStru_t;

//ָ���������
enum CmdOperEnum {
	CMD_OPER_SET,		//������
	CMD_OPER_REQU,		//��ѯ(����)��
	CMD_OPER_RESP,		//Ӧ����
	CMD_OPER_ERR,		//������
	CMD_OPER_MAX
};

//ָ����鶨��
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

//ͨ����������
#define INPUT_CHANNEL_COUNT		7
#define OUTPUT_CHANNEL_COUNT	4

//����������Ŷ���
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

//�����������λ�ü����ֵ����Чֵ(�������ֵ)
enum CmdDataIndexEnum {
	CMD_DATA_INDEX_MIN = 0,
	CMD_DATA_INDEX_0 = CMD_DATA_INDEX_MIN,
	CMD_DATA_INDEX_1,
	CMD_DATA_INDEX_2,
	CMD_DATA_INDEX_MAX = CMD_DATA_INDEX_2,
	CMD_DATA_INDEX_INVALID,
};

//�������SQLite�ӿڵĲ�����;��SQL������;,��Ҫ����SQL���ִ�еĻص����������ֲ�����Ӧ�Ĵ���ʽ
enum SqliteOperateId {
	ID_NULL,	//û���ض�����,���Բ��ô���
	ID_INIT,	//��ʼ��DSP����,�ص���������Ҫ���ô��亯��
	ID_ADD,		//�����ݿ��������(�����Ȳ�ѯ�����Ƿ����,��ʱ�ڻص��������Ƿ����(���ûص����ݾʹ���))
	ID_SHOW,	//��ͨ�Ĳ鿴���ݱ��е�����
	ID_GET,		//��ȡ���ݱ��е�ĳ������
	ID_EXPORT,	//��������(���浽�ļ�)
};


/*
 *������Ƶ��Ϣ�ṹ,��Э���4Byte���ݶ��屣��һ��
 *
 *Э�鴦��ʱ���յ���PCָ�����Ϊ�ýṹ����,��ȡ��\
 *���Ժ���Ҫת��Ϊ�ýṹ����,Ȼ�����ʹ��
 */
typedef struct CmdMsgStru {
	unsigned int op;				//ָ������
	unsigned int key;				//ָ�����
	unsigned int type;				//ָ���������
	unsigned int param1;			//ָ�����1
	unsigned int param2;			//ָ�����2
	unsigned int param3;			//ָ�����3
}CmdMsgStru_t;

#define MAX(iA, iB) (((iA) >= (iB)) ? (iA) : (iB))
#define MIN(iA, iB) (((iA) <= (iB)) ? (iA) : (iB))

//����洢��������ݿ�����������ŵĶ�Ӧ��ϵ
struct GroupAndTableStru {
	unsigned int key; //���
	struct CmdData *table; //�����Ӧ����������ݿ�����
	unsigned int count;
};

/*
 *       Name: get_table_name
 *Description: ����������Ż�ȡ��Ӧ����
 *  Parameter:
 *    iCmdKey: [in]�������
 *     Return:
 *       ����: �ɹ�
 *       NULL: ʧ��
 */
struct GroupAndTableStru *get_table_name(unsigned int iCmdKey);
#define GET_TABLE_NAME(iCmdKey)	get_table_name(iCmdKey)

/*
 *       Name: get_cmd_index
 *Description: ����������ź�������Ż�ȡ��Ӧ����Ĳ���λ��[0 / 1 / 2]
 *  Parameter:
 *    iCmdKey: [in]�������
 *      iType: [in]�������
 *     Return:
 *       �ɹ�: ����λ��[CMD_DATA_INDEX_0 / CMD_DATA_INDEX_1 / CMD_DATA_INDEX_2]
 *       ʧ��: CMD_DATA_INDEX_INVALID
 */
unsigned int get_cmd_index(unsigned int iCmdKey, unsigned int iCmdType);

/*
 *       Name: get_cmd_id
 *Description: ��������ṹ��ȡ����ID(������ź�������ż����ֲ������,������ݲ���λ�þ���)
 *  Parameter:
 *       pMsg: [in]����ṹָ��
 *     Return:
 *         ID: �ɹ�
 *         -1: ʧ��
 */
int get_cmd_id(CmdMsgStru_t *pMsg);
#define GET_CMD_ID(pMsg)		get_cmd_id(pMsg)

//����ÿһ��������������󳤶�,�ɴ�����ʱ�����ֶεĳ���
#define CMD_DESC_SIZE		50
#define CMD_DESC_DEFAULT	"[Default]New Data"

/*
 *       Name: get_cmd_desc
 *Description: ��ȡ��������(��˵��)
 *  Parameter:
 *       pMsg: [in]����ṹָ��
 *      pDesc: [out]�������������Ļ�����
 *     Return:
 *       �ɹ�: pDesc��ַ
 *       ʧ��: NULL
 */
char *get_cmd_desc(CmdMsgStru_t *pMsg, char *pDesc);

/*
 *       Name: database_init
 *Description: ���ݿ��ʼ��(�����ݿⲢ������)
 *  Parameter: ��
 *     Return:
 *         0: �ɹ�
 *         -1: ʧ��
 */
//int database_init();


/*
 *       Name: database_exit
 *Description: ���ݿ��˳�
 *  Parameter: ��
 *     Return: ��
 */
//void database_exit();

/*
 *       Name: database_reboot
 *Description: ���ݿ�����,����д���ݵ�����
 *  Parameter: ��
 *     Return: ��
 */
//void database_reboot();

/*
 *            Name: transfer_handle_register
 *           Brief: ע�ᷢ�����ݺ���,��Ҫע��ĺ�������DSP��OSC��PC�˷������ݵĺ���
 *       Parameter: ��
 *          Return: ��
 */
//void transfer_handle_register();

/*
 *      Name: resource_init
 *     Brief: ��Դ��ʼ��,���豸,����SPI/KEY/UART��
 * Parameter: ��
 *    Return:
 *      �ɹ�: 0
 *      ʧ��: -1
 */
//int resource_init();

/*
 *      Name: resource_exit
 *     Brief: �رո��豸,��Դ����
 * Parameter: ��
 *    Return: ��
 */
//void resource_exit();
		     
/*
 *      Name: dsp_init
 *     Brief: DSP��ʼ��,��ȡ�����������ݲ����͸�DSP/PC��
 * Parameter: ��
 *    Return: ��
 */
//void dsp_init();

/*	  
*   
*	����PISP1016�ĳ�������������ӿ�˳��Ϊ:
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
