/*
 * do_data.h
 *
 *  Created on: 2016-4-22
 * Corporation: NanJingQingJin
 *      Author: wanghb
 * Description: 实现数据处理的功能,包括将PC下发的命令解析后更新数据库表,启动时获取属性等
 */

#ifndef DO_DATA_H_
#define DO_DATA_H_

#include "inc.h"
//#include "sqlite/sqlite.h"

//定义将要添加到数据库的数据的类型
typedef enum SetEntryTypeEnum {
	SET_ENTRY_CMD, //命令类型
	SET_ENTRY_MSG  //命令结构体类型
}SetEntryTypeEnum_t;

/*
 *      Name: do_transfer_data
 *     Brief: 发送命令接口,将传入的CmdMsgStru_t数据编码为4Byte命令发送出去
 * Parameter:
 *      iWho: [in]调用该接口获取并发送配置命令所有者(PC、OSC)
 *      pMsg: [in]需要发送的CmdMsgStru_t数据
 *    Return: 无
 */
void do_transfer_data(TransDataEnum_t iWho, const CmdMsgStru_t *pMsg);

/*
 *      Name: do_add_cmd / do_add_cmd_msg
 *     Brief: 添加或更新配置数据到对应表中(输入配置、输出配置、通用配置等) \
 *            do_add_cmd和do_add_cmd_msg功能一样,只是接受的数据类型不同, \
 *            do_add_cmd接受4Byte命令,而do_add_cmd_msg接受解析为结构体的命令数据
 * Parameter:
 *      iCmd: [in]需要添加或更新的4Byte命令数据
 *      pMsg: [in]需要添加或更新的CmdMsgStru_t数据
 *    Return:
 *         0: 成功
 *        -1: 失败
 */
int do_add_cmd(const unsigned char *pCmd);
int do_add_cmd_msg(CmdMsgStru_t *pMsg);

/*
 *      Name: do_set_cmd_entry
 *     Brief: 添加或更新配置数据到对应表中,可以传命令或命令结构体为输入参数,内
 *            部用do_add_cmd / do_add_cmd_msg实现
 * Parameter:
 *  iSetType: [in]输入数据的类型(4Byte命令或命令结构体)
 *      pMsg: [in]需要添加或更新的数据(4Byte命令或命令结构体)缓冲区
 *    Return: 无
 */
void do_set_cmd_entry(SetEntryTypeEnum_t iSetType, const void *pCmdOrMsg);

/*
 *      Name: do_show_data
 *     Brief: 获取(查询)配置信息(输入配置、输出配置、通用配置等),按表名(iCmdKey)查询
 * Parameter:
 *      iWho: [in]查询到的数据发送对象(需要发给谁)
 *   iCmdKey: [in]命令组号
 *       iId: [in]查询到数据后的操作方式标号(在回调函数中对应不同的处理方式)
 *    Return: 无
 */
void do_show_data(TransDataEnum_t iWho, unsigned int iCmdKey, enum SqliteOperateId iId);

/*
 *      Name: do_show_data_all
 *     Brief: 获取(查询)数据库中所有表的所有数据,用于查看所有数据和系统启动初始化DSP,PC等
 * Parameter:
 *      iWho: [in]查询到的数据发送对象(需要发给谁)
 *       iId: [in]查询到数据后的操作方式标号(在回调函数中对应不同的处理方式)
 *    Return: 无
 */
void do_show_data_all(TransDataEnum_t iWho, enum SqliteOperateId iId);

/*
 *      Name: GetCfgAndSendHandle
 *     Brief: 外部上位机(OSC或PC)获取本地配置数据,并直接发送给上位机
 * Parameter:
 *      iWho: [in]获取配置的所有者(OSC或PC)
 *    Return:
 */
void GetCfgAndSendHandle(TransDataEnum_t iWho);

/*
 *      Name: do_get_data
 *     Brief: 根据组号和组内序号获取命令的值,只能获取一个组内序号对应一个值的参数(某些组内序号对
 *            应多组数据,此时需要第1、2参数配合取值,此时只能根据命令ID查找,此部分基本不用,未实现)
 *
 *      注意: 该接口不能自动判断获取值是否只有一个值,调用该接口时需自行判断
 *            是否只有唯一值,如果不是则不能用该接口
 *
 * Parameter:
 *   iCmdKey: [in]命令组号
 *  iCmdType: [in]命令组内序号
 *    Return:
 *      成功: 查找的参数值
 *      失败: -1
 */
int do_get_data(unsigned int iCmdKey, unsigned int iCmdType);
#define DO_GET_INPUT_TYPE()			do_get_data(CMD_GROUP_GENERAL, GENERAL_GROUP_INPUT_TYPE)
#define DO_GET_OUTPUT_LEVEL()		do_get_data(CMD_GROUP_GENERAL, GENERAL_GROUP_KNOB_VOLUME)
#define DO_GET_SURROUND_TYPE()		do_get_data(CMD_GROUP_GENERAL, GENERAL_GROUP_SURROUND_TYPE)
/*input的命令组号从2开始,但是input的编码从1开始,相差1*/
#define DO_GET_SAMPLING_RATE(input)	do_get_data((input + 1), INPUT_GROUP_SAMPLING_RATE)
#define DO_GET_PLAYBACK_SF()		do_get_data(CMD_GROUP_INPUT1, INPUT_GROUP_PLAYBACK_SF)
#define DO_GET_CHANNEL_NUMBER()		do_get_data(CMD_GROUP_INPUT1, INPUT_GROUP_CHANNEL_NUMBER)

/*
 *      Name: do_delete_data
 *     Brief: 根据命令删除数据库中的配置信息(输入配置、输出配置、通用配置等)数据
 * Parameter:
 *      pMsg: [in]需要删除的CmdMsgStru_t数据(需要根据命令的部分数据确定删除数据的条件)
 *    Return: 无
 */
void do_delete_data(CmdMsgStru_t *pMsg);

/*
 *      Name: do_export_data
 *     Brief: 导出数据,将数据库文件导出到文本文件
 * Parameter:
 * file_name: [in]导出数据保存的文件名(路径)
 *    Return:
 *         0: 成功
 *        -1: 失败
 */
int do_export_data(const char *file_name);

/*
 *      Name: do_import_data
 *     Brief: 导入数据,将文本文件导入到数据库中
 * Parameter:
 * file_name: [in]导入数据的文件名(路径)
 *    Return:
 *         0: 成功
 *        -1: 失败
 */
int do_import_data(const char *file_name);

#endif /* DO_DATA_H_ */
