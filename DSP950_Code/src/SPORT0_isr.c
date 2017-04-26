///////////////////////////////////////////////////////////////////////////////////////
//NAME:     SPORT1_isr.c (Block-based Talkthrough)
//DATE:     02/06/10
//PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1939.
//
//USAGE:    This file contains SPORT1 Interrupt Service Routine. Four buffers are used
//          for this example: Two input buffers, and two output buffers.
///////////////////////////////////////////////////////////////////////////////////////

#include "./include/ADDS_21489_EzKit.h"
#include <sru.h>
#include <time.h>
#include <string.h>
#include "stdio.h"
#include "math.h"
#include "./include/pcm4104.h"
#include "./include/input_types.h"

//If the processing takes too long, the program will be stuck in this infinite loop.
void ProcessingTooLong(void){
    while(1);
}

int g_interruptFlag = 0;

int g_i_da_interrupt_flag = 0;
  
int g_i_da_write_addr = 0;
 


int g_i_test_da_interrupt_times = 0;

  




void SP0ADCSend_ISR(int sig_int){
	g_i_da_interrupt_flag = 1;
	//g_i_da_write_count = get_current_dma_tx_block();
	//g_i_da_write_addr = (g_i_da_write_count - 2 + NUMDMAUSED) % NUMDMAUSED;

	g_i_da_write_addr = get_current_dma_tx_block();

	set_interrupt_times();	 
} 


