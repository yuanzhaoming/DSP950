#ifndef _EQ_H_
#define _EQ_H_


extern void sim_31band_EQ_setgaintofilters(int Band_ID, int channelindex, float gain);
extern void set_EQ_LPF (int EQ_ID, float *filter_cf, float *fc);


extern void SPI_set_EQ_LPfilter(int channelindex);
extern void filter_process_GEQ_block(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex/*0,1,2...*/);
extern void	filter_process_HPF_block_dl(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex);

extern void	filter_process_LPF_block(float *p_EQ_In, float *p_EQ_Out, int samplenuminblock, int channelindex);
extern  int frequencydivision_channel(int ch, int bandindex,float *p_databuf,
 						float *p_divisionoutbuf,int samplenuminblock);


extern void 	SPI_set_EQ_HPfilter(int channelindex);


extern void 	sim_frequencydivision_setdivisionpointnum(int channelindex, int pointnum);
 
extern void sim_31band_EQ(void);

extern void zero_filterstate(void);

extern int init_divisionfilter(void);

extern void sim_31band_EQ_outputparamset(void);

extern void sim_frequencydivisionfilter_outputparamset(void);
						
#endif



