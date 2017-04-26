#ifndef _input_types_h_
#define _input_types_h_

extern void Init_all_input_types( void );

extern void handle_all_input_data(int i_input_types);

extern void clear_ad_interrupt_index( void);


extern int *p_multichanneloutputbuf[16]; //与上位机对应的数组
extern int *p_multichannel_input_spdif_buf[16]; //与上位机对应的数组

extern int i_l_spdif_current_index;

extern void test_audio_mode( void );

extern void handle_ad_aes_data(void);

extern void set_interrupt_times( void  );


#endif

