#include "delay.h"

volatile static uint32_t count = 0;
volatile static uint32_t delay_count = 0;
void SysTick_Handler(){
	count++;
	delay_count++;
}
void Delay(int milli){
	delay_count = 0;
	while(delay_count < milli);
	delay_count = 0;
}
void Freeze_delay(int milli){
	for(int counter = 8000 * milli; counter != 0; counter--);
}
uint32_t GetMicro(){
	return count;
}
uint32_t GetMili(){
	return count;
}

void delay_action(uint32_t milli, uint8_t process_num, void (*do_action)()){
	static uint8_t flag[8] = {0};
	static uint32_t counter[8] = {0};

	if(!flag[process_num]){
		counter[process_num] = GetMili();
		flag[process_num] = 1;
	}
	else{
		if(GetMili() - counter[process_num] > milli){
			counter[process_num] = GetMili();
			do_action();
			flag[process_num] = 0;
		}
	}

}
