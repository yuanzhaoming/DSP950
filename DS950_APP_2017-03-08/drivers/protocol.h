/*
 * protocol.h
 *
 *  Created on: 2015年11月27日
 * Corporation: NanJingQingJin
 *      Author: wanghb
 * Description: 实现协议解析或组装功能
 */

#ifndef __PROTOCOL_H			  
#define __PROTOCOL_H

#include <stdio.h>

#include "inc.h"

/*
 *      Name: prot_cmd_to_msg
 *     Brief: 解析从PC端收到的命令,解析为AudioMsgStru_t格式的数据
 * Parameter:
 *      pCmd：[in]从PC端接收的数据缓冲区(4Byte)指针
 *      pMsg：[out]保存命令解析后的数据指针
 *    Return: 
 *      成功: 0
 *      失败: -1
 */
int prot_cmd_to_msg(const unsigned char *pCmd, CmdMsgStru_t *pMsg);

/*
 *      Name: prot_msg_to_cmd
 *     Brief: 将AudioMsgStru_t格式的数据编码为协议规定的PC和DSP可识别的命令格式
 * Parameter:
 *      pMsg：[in]音频设置属性数据
 *      pCmd：[out]协议定义的4Byte命令格式保存地址
 *    Return: 无
 */
void prot_msg_to_cmd(const CmdMsgStru_t *pMsg, unsigned char *pCmd);

#endif /* __PROTOCOL_H */
