#include "protocol.h"
#include "inc.h"
#include "rtthread.h"

#ifdef PROJECT_DEBUG
#if 0
#define PROT_DEBUG 
#endif
#endif /* PROJECT_DEBUG */

//#define PROT_ERROR
   
#ifdef PROT_DEBUG
#define prot_debug(fmt, ...) rt_kprintf("[PROT][Line:%d]"fmt"\n", __LINE__, ##__VA_ARGS__)
#else
#define prot_debug(fmt, ...)
#endif

#ifdef PROT_ERROR
#define prot_error(fmt, ...) rt_kprintf("[ERR][PROT][Line:%d]"fmt"\n", __LINE__, ##__VA_ARGS__)
#else
#define prot_error(fmt, ...)
#endif


unsigned int opIndex[4] = {0x00, 0x40, 0x80, 0xC0};
unsigned int keyIndex[16] = {
	0x00, 0x04, 0x08, 0x0c,
	0x10, 0x14, 0x18, 0x1c,
	0x20, 0x24, 0x28, 0x2c,
	0x30, 0x34, 0x38, 0x3c};
unsigned int typeIndex[32][2] = { 
	{0x00, 0x00},{0x00, 0x20},{0x00, 0x40},{0x00, 0x60},
	{0x00, 0x80},{0x00, 0xa0},{0x00, 0xc0},{0x00, 0xe0},
	{0x01, 0x00},{0x01, 0x20},{0x01, 0x40},{0x01, 0x60},
	{0x01, 0x80},{0x01, 0xa0},{0x01, 0xc0},{0x01, 0xe0},
	{0x02, 0x00},{0x02, 0x20},{0x02, 0x40},{0x02, 0x60},
	{0x02, 0x80},{0x02, 0xa0},{0x02, 0xc0},{0x02, 0xe0},
	{0x03, 0x00},{0x03, 0x20},{0x03, 0x40},{0x03, 0x60},
	{0x03, 0x80},{0x03, 0xa0},{0x03, 0xc0},{0x03, 0xe0}};


int prot_cmd_to_msg(const unsigned char *pCmd, CmdMsgStru_t *pMsg)
{
	int i = 0;
      

	prot_debug("CMD:0x%02X %02X %02X %02X ", (unsigned int)pCmd[0],
											 (unsigned int)pCmd[1],
											 (unsigned int)pCmd[2],
		 									 (unsigned int)pCmd[3]);

	//解析指令类型
	pMsg->op = 0;
	for (i = 0; i < 4; ++i) {
		if ((pCmd[0] & 0xC0) == opIndex[i]) {
			pMsg->op = i;
			break;
		}
	}	    

	//解析指令分组
	pMsg->key = 0;
	for (i = 0; i < 16; ++i) {
		if ((pCmd[0] & 0x3C) == keyIndex[i]) {
			pMsg->key = i;
			break;
		}
	}

	if ((pMsg->key < CMD_GROUP_GENERAL) || (pMsg->key > CMD_GROUP_MAX)) {
		prot_error("[Key:%d]CMD Invalid: Key Error, Return", pMsg->key);

		return -1;
	}

	//解析组内序号
	pMsg->type = 0;
	for(i = 0; i < 32; ++i) {
		if (((pCmd[0]&0x03) == typeIndex[i][0]) && ((pCmd[1] & 0xE0) == typeIndex[i][1]) ) {
			pMsg->type = i;
			break;
		}
	}

	//解析指令参数
	pMsg->param1 = (int)(pCmd[1] & 0x1F);
	pMsg->param2 = (int)pCmd[2];
	pMsg->param3 = (int)pCmd[3];

	prot_debug("op key type param1 param2 param3");
	prot_debug("%X %3X %4X %5X %5X %5X\n",
				pMsg->op, pMsg->key, pMsg->type, pMsg->param1,
				pMsg->param2, pMsg->param3);

	return 0;
}


#if 0

unsigned int keyIndex[16] = {
	0x00, 0x04, 0x08, 0x0c,
	0x10, 0x14, 0x18, 0x1c,
	0x20, 0x24, 0x28, 0x2c,
	0x30, 0x34, 0x38, 0x3c};

unsigned int typeIndex[32][2] = {
	{0x00, 0x00},{0x00, 0x20},{0x00, 0x40},{0x00, 0x60},
	{0x00, 0x80},{0x00, 0xa0},{0x00, 0xc0},{0x00, 0xe0},
	{0x01, 0x00},{0x01, 0x20},{0x01, 0x40},{0x01, 0x60},
	{0x01, 0x80},{0x01, 0xa0},{0x01, 0xc0},{0x01, 0xe0},
	{0x02, 0x00},{0x02, 0x20},{0x02, 0x40},{0x02, 0x60},
	{0x02, 0x80},{0x02, 0xa0},{0x02, 0xc0},{0x02, 0xe0},
	{0x03, 0x00},{0x03, 0x20},{0x03, 0x40},{0x03, 0x60},
	{0x03, 0x80},{0x03, 0xa0},{0x03, 0xc0},{0x03, 0xe0}};

#endif

void prot_msg_to_cmd(const CmdMsgStru_t *pMsg, unsigned char *pCmd)
{
	unsigned int opIndex[4] = {0x00, 0x40, 0x80, 0xC0};

	prot_debug("op key type param1 param2 param3");
	prot_debug("%X %3X %4X %5X %5X %5X\n",
				pMsg->op, pMsg->key, pMsg->type, pMsg->param1,
				pMsg->param2, pMsg->param3);

	//组合指令参数
	pCmd[0] = (unsigned char)(opIndex[pMsg->op]
							+ keyIndex[pMsg->key]
							+ typeIndex[pMsg->type][0]);
	pCmd[1] = (unsigned char)(typeIndex[pMsg->type][1] + pMsg->param1);
	pCmd[2] = pMsg->param2;
	pCmd[3] = pMsg->param3;
//	prot_debug("CMD[0]:%X", (unsigned int)cmd[0]);
//	prot_debug("CMD[1]:%X", (unsigned int)cmd[1]);
//	prot_debug("CMD[2]:%X", (unsigned int)cmd[2]);
//	prot_debug("CMD[3]:%X", (unsigned int)cmd[3]);
	prot_debug("CMD:0x%2X %2X %2X %2X ", (unsigned int)pCmd[0],
										 (unsigned int)pCmd[1],
										 (unsigned int)pCmd[2],
										 (unsigned int)pCmd[3]);
	/*特别注意:
     *先将char型指正转强转为unsigned int型指针,然后再取值,再强转为unsigned int型数\
     *据,如果不取值,获取的数据是地址指针数据,而不是指针指向的数据
     *
     *注意: 强转打印的数据大小端与实际命令的大小端相反
	 */
//	prot_debug("CMD:%X", (unsigned int)*((unsigned int *)(cmd)));
}








