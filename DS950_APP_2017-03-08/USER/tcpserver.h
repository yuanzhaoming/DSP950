#ifndef _tcp_server_h_
#define _tcp_server_h_

extern void tcpserv(void* parameter);
extern void tcp_send_instruction_to_dsp(void);
extern void send_instru_via_socket(unsigned int *i_value);

extern void release_sem( void);

extern void code_to_dsp(int i_code_number);

#endif





