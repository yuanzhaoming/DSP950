#include "do_data.h"
#include "inc.h"
#include "protocol.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h> //atoi()
#include <rtthread.h>

#ifdef PROJECT_DEBUG
#if 0
#define DO_DEBUG
#endif
#endif /* PROJECT_DEBUG */

//#define DO_ERROR
      
#ifdef DO_DEBUG
#define do_debug(fmt, ...) rt_kprintf("[DO][%d]"fmt"\n", __LINE__, ##__VA_ARGS__)
#else
#define do_debug(fmt, ...)
#endif /* DO_DEBUG */

#ifdef DO_ERROR
#define do_error(fmt, ...) rt_kprintf("[DO][ERR][%d]"fmt"\n", __LINE__, ##__VA_ARGS__)
#else
#define do_error(fmt, ...)
#endif /* DO_ERROR */

#define CMD_DATA_LINE_SIZE	256		//���������ļ�һ����󳤶�

void do_transfer_data(TransDataEnum_t who, const CmdMsgStru_t *pMsg)
{
	unsigned char command[COMMAND_SIZE];

	do_debug("do_transfer_data(Hex):");
	do_debug("op key type param1 param2 param3");
	do_debug("%X %3X %4X %5X %5X %5X",
			pMsg->op, pMsg->key, pMsg->type, pMsg->param1, pMsg->param2, pMsg->param3);

	memset(command, 0, COMMAND_SIZE);
	prot_msg_to_cmd(pMsg, command);

	//Send Data to DSP
	if (who & TRANS_DSP_MASK) {
		if (SendData2DSP_Handle != NULL) {
			if (SendData2DSP_Handle(command, COMMAND_SIZE) < 0) {
				do_error("SendData2DSP_Handle Error, Continue");
			} else {
				do_debug("SendData2DSP_Handle Ok");
			}
		}
	}
}

int do_add_cmd_msg(CmdMsgStru_t *pMsg)
{
	struct GroupAndTableStru *table_name = NULL;
	int cmd_id = 0;
	int i = 0;
	int data = 0;
	
	if (pMsg == NULL)
		return -1;
		 
	if ((table_name = GET_TABLE_NAME(pMsg->key)) == NULL) {
		do_error("Table name is NULL");

		return -1;
	}

	if ((cmd_id = GET_CMD_ID(pMsg)) < 0) {
		do_error("Get CMD ID error");

		return -1;
	}

	do_debug("cmd_id = %d", cmd_id);

	prot_msg_to_cmd(pMsg, (unsigned char *)&data);
	for (i = 0; (i < table_name->count) && (table_name->table[i].id != 0); ++i) {
		if (table_name->table[i].id == cmd_id) {
			 table_name->table[i].data = data;

			 return 0;
		}
	}

	if (i < table_name->count) {
	   table_name->table[i].id = cmd_id;
	   table_name->table[i].data = data;

	   do_debug("add ok");

	   return 0;
	}

	do_debug("[%d-%d]add error", table_name->count, i);

	return -1;
}

int do_add_cmd(const unsigned char *pCmd)
{
	//��4Byte����ת��Ϊ����ṹ��,Ȼ�����do_add_cmd_msg()����
	CmdMsgStru_t msg = {0};

	if (prot_cmd_to_msg(pCmd, &msg) < 0) {
		do_error("prot_cmd_to_msg Error, Return");

		return -1;
	}
	return do_add_cmd_msg(&msg);
}

void do_set_cmd_entry(SetEntryTypeEnum_t iSetType, const void *pCmdOrMsg)
{
	int ret = -1;

	switch (iSetType) {
	case SET_ENTRY_CMD:
		ret = do_add_cmd((unsigned char *)pCmdOrMsg);
		break;
	case SET_ENTRY_MSG:
		ret = do_add_cmd_msg((CmdMsgStru_t *)pCmdOrMsg);
		break;
	default:
		break;
	}

	if (ret < 0) {
		do_debug("!!! do_set_cmd_entry Error !!!");
	}
}



int do_get_data(unsigned int iCmdKey, unsigned int iCmdType)
{  
	unsigned int index = CMD_DATA_INDEX_INVALID; 

	struct GroupAndTableStru *table_name = NULL;
	unsigned int cmd_id = 0xffffffff;
	int i = 0;
	CmdMsgStru_t msg = {0};
	(void)index; //��ֹ������������
	if ((index = get_cmd_index(iCmdKey, iCmdType)) == CMD_DATA_INDEX_INVALID) {
		do_error("Get's CMD Index Invalid");
		return -1;
	}

#if 0
	//����ֻ����index�Ƿ�ΪCMD_DATA_INDEX_0�ж��Ƿ�ֻ��һ������
	//��ʵ�ǲ����жϵ�,��Ϊ���������������ڴ���32,ֻ�ܷ��ڵڶ�����,��ʱindexΪCMD_DATA_INDEX_1
	//Ϊ�˱�֤��ȡ������Ψһ�ҳ��򲻳���,���øýӿ�ʱ�������ж��Ƿ�ֻ��Ψһֵ,������������øýӿ�

	//ֻ�ܻ�ȡһ��������Ŷ�Ӧһ��ֵ�Ĳ���(ĳЩ������Ŷ�Ӧ��������,��ʱ��Ҫ��1��2�������ȡֵ)
	if (index != CMD_DATA_INDEX_0) {
		do_error("[index:%d]index != CMD_DATA_INDEX_0", index);

		return -1;
	}
#endif
	//index = CMD_DATA_INDEX_0ʱֻ��Ҫkey��type�Ϳ��Ի�ȡ����ID,������һ��������ȷ��ȡ,���Բ��ü��ж�
	msg.key  =  iCmdKey;
	msg.type = 	iCmdType;
	cmd_id   =  GET_CMD_ID(&msg);

	if ((table_name = GET_TABLE_NAME(iCmdKey)) == NULL) {
		do_error("Table name is NULL");

		return -1;
	}

	for (i = 0; (i < table_name->count) && (table_name->table[i].id != 0); ++i) {
		if (table_name->table[i].id == cmd_id) {
			 return table_name->table[i].data;
		}
	}

	return -1;
}

