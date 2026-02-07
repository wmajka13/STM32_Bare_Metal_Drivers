/*
 * 001led_toggle.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Wiktor Majka
 */

#include "stm32f411xx.h"


void delay(void)
{
	for(uint32_t i = 0 ; i< 500000 ; i ++);
}

int main(void)
{
	GPIO_Handle_t GpioLed; 									//Stowrzenie handla
	GpioLed.pGPIOx = GPIOA;									//Przypisanie portu
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GpioLed);

	while(1)
	{
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
		delay();
	}

	return 0;
}
