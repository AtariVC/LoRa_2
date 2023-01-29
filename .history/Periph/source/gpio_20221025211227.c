#include "gpio.h"

/*
 * 	 _______________________________________________________
 * 	| MODE | OTYPER | OSPEED | PUPD  |	I/O configuration	|
 * 	|______|________|________|_______|______________________|
 * 	|	   |	0	|		 | 0 | 0 | GP output | PP		|
 * 	|	   |	0	|		 | 0 | 1 | GP output | PP + PU	|
 * 	|	   |	0	|	00	 | 1 | 0 | GP output | PP + PD	|
 * 	|	   |	0	|	01	 | 1 | 1 | Reserved				|
 * 	|  01  |	1	|	10	 | 0 | 0 | GP output | OD		|
 * 	|	   |	1	|	11	 | 0 | 1 | GP output | OD + PU	|
 * 	|	   |	1	|		 | 1 | 0 | GP output | OD + PD	|
 * 	|______|____1___|________|_1_|_1_|_Reserved_____________|
 * 	|	   |	0	|		 | 0 | 0 | AF 		 | PP		|
 * 	|	   |	0	|		 | 0 | 1 | AF 		 | PP + PU	|
 * 	|	   |	0	|	00	 | 1 | 0 | AF 		 | PP + PD	|
 * 	|	   |	0	|	01	 | 1 | 1 | Reserved				|
 * 	|  10  |	1	|	10	 | 0 | 0 | AF 		 | OD		|
 * 	|	   |	1	|	11	 | 0 | 1 | AF 		 | OD + PU	|
 * 	|	   |	1	|		 | 1 | 0 | AF 		 | OD + PD	|
 * 	|______|____1___|________|_1_|_1_|_Reserved_____________|
 * 	|	   |	x	| x  | x | 0 | 0 | Input	 | Floating |
 * 	|	   |	x	| x  | x | 0 | 1 | Input	 | PU		|
 * 	|  00  |	x	| x  | x | 1 | 0 |_Input_____|_PD_______|
 * 	|	   |	x	| x  | x | 1 | 1 |       Reserved		|
 * 	|______|________|____|___|___|___|______________________|
 * 	|	   |	x	| x  | x | 0 | 0 |_In/Out____|_Analog___|
 * 	|	   |	x	| x  | x | 0 | 0 | 						|
 * 	|  11  |	x	| x  | x | 0 | 0 | 		Reserved		|
 * 	|	   |	x	| x  | x | 0 | 0 | 						|
 * 	|______|________|____|___|___|___|______________________|
 *
 */
#define NUMBER_OF_PINS_ON_EACH_PORT 	16

enum GPIO_Port {
	Port_A, Port_B, Port_C, Port_D, Port_E, Port_F, Port_G, Port_H
};
typedef struct GPIO_Struct{
    GPIO_TypeDef *port;
    uint8_t pin;
} GPIO_Struct;

/// @brief Инициализация пина
/// @param gpio номер пина (PA1, PB12, PC0, ...)
/// @param mode режим работы (Input_mode, General_output, Analog_mode, or Alternatuve functions like AF0, AF1, ...)
/// @param config конфигурация выхода (Push_pull, Open_drain)
/// @param pull_up_down подтяжка (no_pull, pull_up, pull_down)
/// @param speed скорость работы (Low_speed, Medium_speed, High_speed, Very_high_speed, Input)
void gpio_init(GPIO_Pin gpio, GPIO_Mode mode, GPIO_Config config, GPIO_Pull pull_up_down, GPIO_Speed speed){
	if(gpio == uninitialized) return;
    gpio--;
    uint8_t pin = gpio % NUMBER_OF_PINS_ON_EACH_PORT;
    uint8_t port_num = gpio / NUMBER_OF_PINS_ON_EACH_PORT;  // GPIOA = 1, GPIOB = 2, ..., GPIOE = 5
	GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)(AHB2PERIPH_BASE + (0x0400UL << port_num));
    RCC->AHB2ENR |= 1 << (port_num);
	gpio_port->PUPDR &= ~(3 << (pin * 2));  // pull up / pull down register
	gpio_port->PUPDR |= pull_up_down << (pin * 2);

	gpio_port->OTYPER &= ~(1 << pin);
	gpio_port->OTYPER |= (config << pin);

	gpio_port->OSPEEDR &= ~(3 << (pin * 2));
	if(speed != Input) gpio_port->OSPEEDR |= speed << (pin * 2);

	if(mode > 3){
		gpio_port->MODER &= ~(3 << (pin * 2));
		gpio_port->MODER |= 2 << (pin * 2);

		gpio_port->AFR[pin/8] &= ~(0x0F << (pin * 4));
		pin/8 == 0 ? (gpio_port->AFR[pin/8] |= ((mode-4) << (pin * 4))) : (gpio_port->AFR[pin/8] |= ((mode-4) << ((pin-8) * 4)));
	}
	else{
		gpio_port->MODER &= ~(3 << (pin * 2));
		gpio_port->MODER |= mode << (pin * 2);
	}
}

/// @brief Устанавливает выбранный пин в состояние state (LOW or HIGH)
/// @param gpio номер вывода (PA0, PA1, ...)
/// @param state LOW or HIGH
void gpio_state(GPIO_Pin gpio, GPIO_State state){
    GPIO_Struct gpio_struct = gpio_calculate(gpio);
	gpio_struct.port->BSRR = 0x01 << (gpio_struct.pin + (!state) * 16);
}

/// @brief Инвертирует состояние пина
/// @param gpio номер вывода (PA0, PA1, ...)
void gpio_toggle(GPIO_Pin gpio){
    GPIO_Struct gpio_struct = gpio_calculate(gpio);
	gpio_struct.port->ODR ^= 0x01 << gpio_struct.pin;
}

/// @brief Возвращает логический уровень указанного пина
/// @param gpio номер вывода (PA0, PA1, ...)
/// @return LOW or HIGH
GPIO_State gpio_read(GPIO_Pin gpio){
    GPIO_Struct gpio_struct = gpio_calculate(gpio);
    return (gpio_struct.port->IDR & (0x01 << gpio_struct.pin)) >> gpio_struct.pin;
}

GPIO_Struct gpio_calculate(GPIO_Pin gpio){
    if(gpio == uninitialized) return;
    gpio--;
    static GPIO_Struct temp = {0};
	uint8_t port = gpio / NUMBER_OF_PINS_ON_EACH_PORT;
	temp.pin = gpio % NUMBER_OF_PINS_ON_EACH_PORT;
    temp.port = (GPIO_TypeDef *)(AHB2PERIPH_BASE + (0x0400UL << port));
    return temp;
}
