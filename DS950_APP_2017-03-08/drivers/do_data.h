/*
 * do_data.h
 *
 *  Created on: 2016-4-22
 * Corporation: NanJingQingJin
 *      Author: wanghb
 * Description: ʵ�����ݴ���Ĺ���,������PC�·������������������ݿ��,����ʱ��ȡ���Ե�
 */

#ifndef DO_DATA_H_
#define DO_DATA_H_

#include "inc.h"
//#include "sqlite/sqlite.h"

//���彫Ҫ��ӵ����ݿ�����ݵ�����
typedef enum SetEntryTypeEnum {
	SET_ENTRY_CMD, //��������
	SET_ENTRY_MSG  //����ṹ������
}SetEntryTypeEnum_t;

/*
 *      Name: do_transfer_data
 *     Brief: ��������ӿ�,�������CmdMsgStru_t���ݱ���Ϊ4Byte����ͳ�ȥ
 * Parameter:
 *      iWho: [in]���øýӿڻ�ȡ��������������������(PC��OSC)
 *      pMsg: [in]��Ҫ���͵�CmdMsgStru_t����
 *    Return: ��
 */
void do_transfer_data(TransDataEnum_t iWho, const CmdMsgStru_t *pMsg);

/*
 *      Name: do_add_cmd / do_add_cmd_msg
 *     Brief: ��ӻ�����������ݵ���Ӧ����(�������á�������á�ͨ�����õ�) \
 *            do_add_cmd��do_add_cmd_msg����һ��,ֻ�ǽ��ܵ��������Ͳ�ͬ, \
 *            do_add_cmd����4Byte����,��do_add_cmd_msg���ܽ���Ϊ�ṹ�����������
 * Parameter:
 *      iCmd: [in]��Ҫ��ӻ���µ�4Byte��������
 *      pMsg: [in]��Ҫ��ӻ���µ�CmdMsgStru_t����
 *    Return:
 *         0: �ɹ�
 *        -1: ʧ��
 */
int do_add_cmd(const unsigned char *pCmd);
int do_add_cmd_msg(CmdMsgStru_t *pMsg);

/*
 *      Name: do_set_cmd_entry
 *     Brief: ��ӻ�����������ݵ���Ӧ����,���Դ����������ṹ��Ϊ�������,��
 *            ����do_add_cmd / do_add_cmd_msgʵ��
 * Parameter:
 *  iSetType: [in]�������ݵ�����(4Byte���������ṹ��)
 *      pMsg: [in]��Ҫ��ӻ���µ�����(4Byte���������ṹ��)������
 *    Return: ��
 */
void do_set_cmd_entry(SetEntryTypeEnum_t iSetType, const void *pCmdOrMsg);

/*
 *      Name: do_show_data
 *     Brief: ��ȡ(��ѯ)������Ϣ(�������á�������á�ͨ�����õ�),������(iCmdKey)��ѯ
 * Parameter:
 *      iWho: [in]��ѯ�������ݷ��Ͷ���(��Ҫ����˭)
 *   iCmdKey: [in]�������
 *       iId: [in]��ѯ�����ݺ�Ĳ�����ʽ���(�ڻص������ж�Ӧ��ͬ�Ĵ���ʽ)
 *    Return: ��
 */
void do_show_data(TransDataEnum_t iWho, unsigned int iCmdKey, enum SqliteOperateId iId);

/*
 *      Name: do_show_data_all
 *     Brief: ��ȡ(��ѯ)���ݿ������б����������,���ڲ鿴�������ݺ�ϵͳ������ʼ��DSP,PC��
 * Parameter:
 *      iWho: [in]��ѯ�������ݷ��Ͷ���(��Ҫ����˭)
 *       iId: [in]��ѯ�����ݺ�Ĳ�����ʽ���(�ڻص������ж�Ӧ��ͬ�Ĵ���ʽ)
 *    Return: ��
 */
void do_show_data_all(TransDataEnum_t iWho, enum SqliteOperateId iId);

/*
 *      Name: GetCfgAndSendHandle
 *     Brief: �ⲿ��λ��(OSC��PC)��ȡ������������,��ֱ�ӷ��͸���λ��
 * Parameter:
 *      iWho: [in]��ȡ���õ�������(OSC��PC)
 *    Return:
 */
void GetCfgAndSendHandle(TransDataEnum_t iWho);

/*
 *      Name: do_get_data
 *     Brief: ������ź�������Ż�ȡ�����ֵ,ֻ�ܻ�ȡһ��������Ŷ�Ӧһ��ֵ�Ĳ���(ĳЩ������Ŷ�
 *            Ӧ��������,��ʱ��Ҫ��1��2�������ȡֵ,��ʱֻ�ܸ�������ID����,�˲��ֻ�������,δʵ��)
 *
 *      ע��: �ýӿڲ����Զ��жϻ�ȡֵ�Ƿ�ֻ��һ��ֵ,���øýӿ�ʱ�������ж�
 *            �Ƿ�ֻ��Ψһֵ,������������øýӿ�
 *
 * Parameter:
 *   iCmdKey: [in]�������
 *  iCmdType: [in]�����������
 *    Return:
 *      �ɹ�: ���ҵĲ���ֵ
 *      ʧ��: -1
 */
int do_get_data(unsigned int iCmdKey, unsigned int iCmdType);
#define DO_GET_INPUT_TYPE()			do_get_data(CMD_GROUP_GENERAL, GENERAL_GROUP_INPUT_TYPE)
#define DO_GET_OUTPUT_LEVEL()		do_get_data(CMD_GROUP_GENERAL, GENERAL_GROUP_KNOB_VOLUME)
#define DO_GET_SURROUND_TYPE()		do_get_data(CMD_GROUP_GENERAL, GENERAL_GROUP_SURROUND_TYPE)
/*input��������Ŵ�2��ʼ,����input�ı����1��ʼ,���1*/
#define DO_GET_SAMPLING_RATE(input)	do_get_data((input + 1), INPUT_GROUP_SAMPLING_RATE)
#define DO_GET_PLAYBACK_SF()		do_get_data(CMD_GROUP_INPUT1, INPUT_GROUP_PLAYBACK_SF)
#define DO_GET_CHANNEL_NUMBER()		do_get_data(CMD_GROUP_INPUT1, INPUT_GROUP_CHANNEL_NUMBER)

/*
 *      Name: do_delete_data
 *     Brief: ��������ɾ�����ݿ��е�������Ϣ(�������á�������á�ͨ�����õ�)����
 * Parameter:
 *      pMsg: [in]��Ҫɾ����CmdMsgStru_t����(��Ҫ��������Ĳ�������ȷ��ɾ�����ݵ�����)
 *    Return: ��
 */
void do_delete_data(CmdMsgStru_t *pMsg);

/*
 *      Name: do_export_data
 *     Brief: ��������,�����ݿ��ļ��������ı��ļ�
 * Parameter:
 * file_name: [in]�������ݱ�����ļ���(·��)
 *    Return:
 *         0: �ɹ�
 *        -1: ʧ��
 */
int do_export_data(const char *file_name);

/*
 *      Name: do_import_data
 *     Brief: ��������,���ı��ļ����뵽���ݿ���
 * Parameter:
 * file_name: [in]�������ݵ��ļ���(·��)
 *    Return:
 *         0: �ɹ�
 *        -1: ʧ��
 */
int do_import_data(const char *file_name);

#endif /* DO_DATA_H_ */
