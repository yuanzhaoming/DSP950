#ifndef _volume_h_
#define _volume_h_

extern float volumegain;

extern void knobvolume_SIGIRQ2(int sig_int);

extern void Volume_Init(void);

extern int g_volume_count_value;

#endif


