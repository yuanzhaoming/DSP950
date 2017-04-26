#include <rtthread.h>
#include <lwip/sockets.h>  
#include "do_data.h"
#include "tcpserver.h"
#include "inc.h"
#include "app_cmd.h"
#include "enc28j60.h"
#include "protocol.h"
#include "spi_instruction.h"

//static const char send_data[] = "This is TCP Server.";  

#define RECEIVE_DATA_LEN  	1024	 
#define PORT				4819
unsigned char recv_data[RECEIVE_DATA_LEN];

#define HTONL(v)	 ( ((v) << 24) | (((v) >> 24) & 0xff) | (((v) << 8) & 0xff0000) | (((v) >> 8) & 0xff00) )

#define net_debug(value,fmt,...) \
	do{\
		if(value&0x01){\
		 		rt_kprintf("[%s],[%d]"fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);\
		}\
		else\
		{\
			 ;\
		}\
	}while(0);

typedef struct _CLIENT{        //�ͻ��˽ṹ��
	int       fd;        //�ͻ���socket������
 
	struct sockaddr_in addr;     //�ͻ��˵�ַ��Ϣ�ṹ��
                                      
} CLIENT;


static int g_i_connected_id = 0;
static int g_i_connected_flag = 0;

/*
* 		DSP��stm32�Ͻ���ͨ�ŵ���Ӧ������:
*			g_mute:
*			g_input_type:
*			g_volume:
*/
int g_mute;	     	//����
int g_input_type;	//��������
int g_volume;	 	//����

extern int g_i_key_led_handle_count;
extern int g_i_store_flag;

int g_set_volume_to_dsp_flag = 0;//��pc���úõ��������ݷ��ظ�dsp.

static int i_eth_interrupt_flag = 0;
void set_eth_interrupt_flag()
{
	i_eth_interrupt_flag = 1;	
}

int get_eth_interrupt_flag()
{
	return i_eth_interrupt_flag;
}

void clear_eth_interrupt_flag()
{
    rt_interrupt_enter();   	
	i_eth_interrupt_flag = 0;
	rt_interrupt_leave();
}

static void handle_net_data(int i_connected,unsigned char *p_data,int i_len);

void tcpserv(void* parameter)
{
   //char *recv_data; 
	rt_uint32_t sin_size;

	
	int count = 0;
	int i = 0;
	int maxi = -1;
//	int newClientId = 0;
	
	int sock, connected, bytes_received,sockfd;
	struct sockaddr_in server_addr, client_addr;
	rt_bool_t stop = RT_FALSE;  

   //recv_data = rt_malloc(1024);  
   //if (recv_data == RT_NULL)
   //{
   //   rt_kprintf("No memory\n");
   //   return;
   //}
 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
	   rt_kprintf("Socket error\n");
	   //rt_free(recv_data);
	   return;
	}

	{
		int opt = SO_REUSEADDR;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));    //����socket����
	}
 
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT); 
   server_addr.sin_addr.s_addr = INADDR_ANY;
   rt_memset(&(server_addr.sin_zero),8, sizeof(server_addr.sin_zero));
 
   if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
   {  
       rt_kprintf("Unable to bind\n");
       //rt_free(recv_data);
       return;
   }
 
   if (listen(sock, 5) == -1)
   {
       rt_kprintf("Listen error\n");
       /* release recv buffer */
       //rt_free(recv_data);
       return;
   }

//   rt_kprintf("\nTCPServer Waiting for client on port %d...\n",PORT);

   //��ȡϵͳ��������
   read_reboot_times();
   //��ȡ�����е�ǰ������
   read_w25q16();
   
   //��ȡvolume,inputtype��Ҫ�洢,mute�ǲ���Ҫ����洢�ģ�stm32�Լ�ȥ���ƣ�Ĭ�ϲ���������
   	{
		int i_getValue;
		//volume
   		i_getValue = do_get_data(1,8);
		if(i_getValue != -1)
		{
			g_volume = i_getValue;
		}
		else
		{
			//���ó��м�ֵ����
			//g_volume = 0x60 / 2;
			//g_volume = (HTONL(g_volume)>>8)&0xff;
			g_volume = ((0x60 /2) << 16) | 0x00000005;
		}
		//rt_kprintf("\n------------------volume:0x%0x------------\n",g_volume);

		//input_type
   		i_getValue = do_get_data(1,9);
		if(i_getValue != -1)
		{
			g_input_type = i_getValue;
		//	g_input_type = (HTONL(i_getValue)>>16)&0x1f;
		}
		else
		{
			//����ΪAD���뼴��
		//	g_input_type = 1;
			g_input_type = INIT_INPUT;	//0x00002205Ϊ��һ����������1��2�ȵ�
		}

		g_i_key_led_handle_count = g_input_type;
		//rt_kprintf("\n------------------input_type:0x%0x------------\n",g_input_type);
	//	g_input_type = do_get_data(1,9);		
	}
   //rt_kprintf("volume:0x%0x\n",i_getValue);
   send_data_to_dsp();

   #if 0
   while(stop != RT_TRUE)
   {
		sin_size = sizeof(struct sockaddr_in);
		
		connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
		
		
		if (connected > 0)
		{
			int timeout;
			
			/* set timeout option */
			timeout = 5000; /* 5second */
			setsockopt(connected, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

			/* handle this client */
			while (1)
			{			  
				//clear_eth_interrupt_flag();	
				/* receive data from this connection */
				bytes_received = recv(connected,recv_data, RECEIVE_DATA_LEN, 0);
				if (bytes_received <= 0)
				{
					rt_kprintf("close client connection:%d, socket error\n",bytes_received);
					/* connection closed. */
					lwip_close(connected);
					break;
				}
				recv_data[bytes_received] = '\0';
				if((bytes_received % 4 == 0) && (bytes_received != 0 ))
			   	{
					rt_kprintf("------------------ \n");
					handle_net_data(connected,recv_data,bytes_received);
			   	}

				/* send data to client */
				send(connected, recv_data, bytes_received, 0);
			}
		}
	}		
	#endif	
		
	#if 0		
   	while(stop != RT_TRUE)
   	{
		sin_size = sizeof(struct sockaddr_in);
		
		connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
		
		rt_kprintf("I got a connection from (%s , %d),count:%d\n",
		          inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),count);
		
		count ++;
 
       	while (1)
       	{   
           //send(connected, send_data, strlen(send_data), 0);
		   //if(get_eth_interrupt_flag() == 1)
		   {
		   		clear_eth_interrupt_flag();	
				
				memset(recv_data, 0, RECEIVE_DATA_LEN);
           		bytes_received = recv(connected,recv_data, RECEIVE_DATA_LEN, 0);
	           	if (bytes_received <= 0)
	           	{    
					rt_kprintf("bytes_received = %d \n" , bytes_received);	
	
	               	lwip_close(connected);
	               	break;
	           	}
				recv_data[bytes_received] = '\0';
				if((bytes_received % 4 == 0) && (bytes_received != 0 ))
			   	{
					rt_kprintf("------------------ \n");
					handle_net_data(connected,recv_data,bytes_received);
			   	}
		   }

		   #if 0
           {    
               rt_kprintf("RECIEVED DATA = %s \n" , recv_data);
			   #if 1
			   rt_kprintf("len = %d \n" , bytes_received);
			   {
				 	int i = 0;
					rt_kprintf("data: \n");
					for(i = 0 ;i < bytes_received ; i ++)
					{
					 	rt_kprintf("0x%0x " , recv_data[i]);
					}
					rt_kprintf("\n");
			   }
			   #endif
           }
		   #endif

       }   
   }
   #endif


   #if 1
   	while(stop != RT_TRUE)
   	{
		fd_set  rset,   allset;    	//select������ļ�����������
		int     maxfd  ;
		int     nready;

		CLIENT client[5];     		//FD_SETSIZEΪselect����֧�ֵ��������������
		sin_size = sizeof(struct sockaddr_in);

		maxfd =  sock;
		FD_ZERO(&allset);        	//���
		FD_SET(sock, &allset);    	//������socket����select��������������		 

		sin_size = sizeof(struct sockaddr_in);
 
		for (i = 0; i < 5; i++) {
			client[i].fd = -1;  
		}

      	while(1)
		{
			rset = allset; 
			
			nready = select(maxfd+1, &rset, NULL, NULL, NULL);    //����select
			net_debug(get_debug_value(),"select saw rset actions and the readfset num is %d. ",nready );  


			if (FD_ISSET(sock, &rset)) 
			{      //����Ƿ����¿ͻ�������
				net_debug(get_debug_value(),"accept a connection.");
				connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
			
				if(connected == -1)
				{
					net_debug(get_debug_value()," accept error \n");
					continue;
				}
				net_debug(get_debug_value(),"I got a connection from (%s , %d),count:%d",
			          inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),count);
				count ++; 

				//����в�������һ���˿ڣ���Ҫ�رյ�����Ϊ����ֻ֧��һ���ͻ��ˡ�
				for (i = 0; i < 5; i++)
				{
					if (client[i].fd >= 0) 
					{
						//���жϲ���
						g_i_connected_flag = 0;
						g_i_connected_id = 0; 

						//�ر�
						lwip_close(client[i].fd);
						net_debug(get_debug_value(),"close other client:%d ",i); 
						FD_CLR(client[i].fd, &allset);    //�Ӽ���������ɾ����socket����
						client[i].fd = -1;        //����Ԫ�����ʼֵ����ʾû�ͻ�������	
					}	
				}

				for (i = 0; i < 5; i++)
				{
					if (client[i].fd < 0) 
					{
						client[i].fd = connected;    //����ͻ���������
						client[i].addr = client_addr; 
						net_debug(get_debug_value(),"You got a connection from %s.  ",inet_ntoa(client[i].addr.sin_addr) );
						break;
					}
				}  
				net_debug(get_debug_value(),"add new connect fd.");

				g_i_connected_flag = 1;
				g_i_connected_id = connected; 


				if (i == 5)          
					net_debug(get_debug_value(),"too many clients");
				FD_SET(connected, &allset);   //����socket���ӷ���select��������
				if (connected > maxfd)  
					maxfd = connected;   //ȷ��maxfd�����������
				if (i > maxi)          //�������Ԫ��ֵ
					maxi = i; 
				if (--nready <= 0) 
					continue;      //���û���¿ͻ������ӣ�����ѭ�
			}


			for (i = 0; i <= maxi; i++) 
			{     
				if ( (sockfd = client[i].fd) < 0)       //����ͻ���������С��0����û�пͻ������ӣ������һ��
					continue;



				if (FD_ISSET(sockfd, &rset)) 
				{        //���˿ͻ���socket�Ƿ�������    
					net_debug(get_debug_value(),"recv occured for connect fd[%d].",i);

					if ((bytes_received = recv(sockfd, recv_data, RECEIVE_DATA_LEN,0)) <= 0) 
					{ //�ӿͻ���socket�����ݣ�����0��ʾ�����ж�

						//���жϲ���
						g_i_connected_flag = 0;
						g_i_connected_id = 0; 

						lwip_close(sockfd);        //�ر�socket����
						net_debug(get_debug_value(),"Client(  ) closed connection.%d ",bytes_received);
						FD_CLR(sockfd, &allset);    //�Ӽ���������ɾ����socket����
						client[i].fd = -1;        //����Ԫ�����ʼֵ����ʾû�ͻ�������
					} 
					else
					{
						net_debug(get_debug_value(),"now handle data");
						recv_data[bytes_received] = '\0';
						if((bytes_received % 4 == 0) && (bytes_received != 0 ))
					   	{	 
							handle_net_data(connected,recv_data,bytes_received);
					   	}						
					}
					if (--nready <= 0)     
						break;       //���û���¿ͻ��������ݣ�����forѭ���ص�whileѭ��
				}
			}  
		} 
	}


   #endif
	  
   lwip_close(sock); 
   //rt_free(recv_data);  
   return ;
}
 


void send_instru_via_socket(unsigned int *i_value)
{
	int i_ret = 0;
	if(g_i_connected_flag == 1)
	{
		if(get_link_status() == 1)
		{
			i_ret = send(g_i_connected_id, i_value, 4 , 0);

			net_debug(get_debug_value(),"id:%d,value:0x%0x,i_ret:%d\n",g_i_connected_id,*i_value,i_ret);
		}
		else
		{
			net_debug(get_debug_value(),"link down,in suspend status\n");
		}
	}
}





#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(tcpserv, startup tcp server);
#endif

#if 0
//��ȡָ��       ��������    2
static unsigned int getinstructiontype(unsigned int instructionparaword)
{
    return (instructionparaword & 0xC0000000) >> 30;//ָ������
}

//��ȡָ��       �������    4 
static unsigned int getinstructiongroup(unsigned int instructionparaword)
{
    return (instructionparaword & 0x3C000000) >> 26;
}		    
     
//��ȡָ��        �������   5
static unsigned int getinstructionindexingroup(unsigned int instructionparaword)
{
    return (instructionparaword & 0x3E00000) >> 21;
}


//��ȡ			  ��һ����   5
static unsigned int getpara1_unsignedint(unsigned int instructionparaword)
{
    return (instructionparaword & 0x1F0000) >> 16;
}

//��ȡ			  �ڶ�����   8	
static unsigned int getpara2_unsignedint(unsigned int instructionparaword)
{
    return (instructionparaword & 0xFF00) >> 8;
}

//��ȡ			  ��������   8
static unsigned int getpara3_unsignedint(unsigned int instructionparaword)
{
    return instructionparaword & 0xFF;
}
#endif

unsigned int g_i_instruction = 0;	 

static void handle_net_data(int i_connected,unsigned char *p_data,int i_len)
{
	int i = 0;
	int i_value = 0;

//	int i_getValue = 0;

	//rt_kprintf("handle net data ...\n");

	for(i = 0 ; i < i_len ; i = i + 4)
	{
		if((p_data[i] == 0x06) && (p_data[i+1] == 0x01) && (p_data[i+2] == 0x00) && (p_data[i+3] == 0x00))
		{
			net_debug(get_debug_value(),"PC Software Open ...\n");		 
			//read_w25q16();//PC���������ʱ����Ҫ����ʼ�������ݷ��͸�PC  
			send_data_to_pc(i_connected);
					
		}
		else if((p_data[i] == 0x06) && (p_data[i+1] == 0x02) && (p_data[i+2] == 0x00) && (p_data[i+3] == 0x00))
		{
			net_debug(get_debug_value(),"PC Software Close ...,and then store data...\n");

			//��Ҫ��pc���Ѿ����úõ��������ݷ��ظ�dsp.
			g_set_volume_to_dsp_flag = 1;
			
			store_w25q16();	
		}
		else
		{
			i_value =  (p_data[i]) | (p_data[i+1] << 8) | ((p_data[i+2] << 16)) | ((p_data[i+3] << 24));
			{
			//int ivalue1 =  ((p_data[i] & 0xC0) << 30) | ((p_data[i] & 0x3C) << 26) | 
			CmdMsgStru_t cmd;
		  	prot_cmd_to_msg(p_data,&cmd);
			//rt_kprintf("%X %3X %4X %5X %5X %5X\n",
			//	cmd.op, cmd.key, cmd.type, cmd.param1,
			//	cmd.param2, cmd.param3);

			//rt_kprintf("receive:0x%0x,0x%0x,0x%0x,0x%0x ,code:0x%0x,0x%0x,0x%0x,0x%0x,0x%0x,0x%0x \n" , 
			//	p_data[i],p_data[i+1],p_data[i+2],p_data[i+3],
			//	getinstructiontype(i_value),getinstructiongroup(i_value),getinstructionindexingroup(i_value),
			//	getpara1_unsignedint(i_value),getpara2_unsignedint(i_value),getpara3_unsignedint(i_value));
			///}		  
			do_set_cmd_entry(SET_ENTRY_CMD, &i_value);

			{
				int i_dsp_get = 0;
				i_dsp_get = (p_data[i] << 24) | (p_data[i+1] << 16) | ((p_data[i+2] << 8)) | ((p_data[i+3] << 0));

				g_i_instruction = i_dsp_get;
				release_sem();
				//need_to_send_instruction_to_dsp(1,i_dsp_get);	
			}
			//ע������������Ҫ��ʱ���£���Ϊ��stm32��Ҫ��ȡ
			//��dsp�϶�ȡ������������stm32����ѡ�񰴼���stm32,mute���š�
			}	 
		}  
	}
}



void tcp_send_instruction_to_dsp(void)
{
	static int i_code_count = 0;
	//��Ҫ��ָ��͸�dsp
	if(0x05260000 == g_i_instruction)
	{
		//rt_kprintf("select mic now \n" );
		code_to_dsp(6); 	
	} 
	else
	{
		//rt_kprintf("0x%08x\n",i_dsp_get);
		//ע�⻹Ҫ����Ͻ����ݴ���dsp��
		SPI_DSP_SendBytes((unsigned char *)&g_i_instruction,4);
		
   	}

	i_code_count ++;
   	//rt_kprintf("---tcp send code to dsp:0x%08x,%d---\n",g_i_instruction,i_code_count);
}








/************************/
  