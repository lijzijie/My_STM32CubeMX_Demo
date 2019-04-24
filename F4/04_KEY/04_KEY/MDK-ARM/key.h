#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx_hal.h"

#define KEY_NO_ONE      0
#define KEY_0_PRESS     1
#define KEY_1_PRESS     2
#define KEY_2_PRESS     3

extern volatile uint8_t key_scan_flag;

extern uint8_t key_scan(void);

#endif
