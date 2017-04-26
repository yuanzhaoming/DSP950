#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"
 /*******
 *�������±���
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	0
#define KEY_OFF	1

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

extern void rt_hw_key_init(void);
extern int rt_handle_key_input(void);
extern int rt_read_key_value(int i_key_number);
#endif /* __LED_H */

