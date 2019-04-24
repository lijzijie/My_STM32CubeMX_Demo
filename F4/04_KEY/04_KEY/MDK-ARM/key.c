#include "key.h"

#define KEY_DELAY  5

volatile uint8_t key_scan_flag = 0;

static uint8_t cn = 0;
static uint16_t new_value = 0;
static uint16_t old_value = 0;

// 按键的引脚输入
uint16_t get_key_input(void)
{
	//PE4~PE2 读取IO输入
	uint32_t TEST =  GPIOE->IDR;
	return (GPIOE->IDR & (0xffff));
}

//按键扫描
uint8_t key_scan(void)
{
	uint16_t val_read;
	
	key_scan_flag = 0;
	val_read = get_key_input();
	
	if(0xffff != val_read)
	{
		if(0 == cn)
		{
			old_value =  val_read;
		
			HAL_Delay(5);
		}
        if(cn <= KEY_DELAY)
		{
			cn++;
		}
		 if(KEY_DELAY == cn)
		 {
			  new_value =  val_read;
			 if(new_value == old_value)
			 {
				 switch(new_value)
				 {
					 case 0xffef: return KEY_0_PRESS;
					 case 0xfff7: return KEY_1_PRESS;
					 case 0xfffb: return KEY_2_PRESS;
					 default :return KEY_NO_ONE;
				 }
			 }
		 }
		
	}    else {
			cn = 0;
		 }
		return KEY_NO_ONE;
}


