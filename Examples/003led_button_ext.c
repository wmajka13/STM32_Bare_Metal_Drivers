/*
 * 003led_button_ext.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Wiktor Majka
 */


#include "stm32f411xx.h"

#define HIGH 1
#define LOW 0
#define BUTTON_PRS HIGH

void delay(void)
{
	for(uint32_t i = 0 ; i< 500000/2 ; i ++);
}

int main(void)
{
	GPIO_Handle_t GpioLed, GPIOBtn;								//Stowrzenie handla

	GpioLed.pGPIOx = GPIOB;									//Przypisanie portu
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GpioLed);

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;

	GPIO_PeriClockControl(GPIOC, ENABLE);
	GPIO_Init(&GPIOBtn);

	while(1)
	{
		if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_10) == BUTTON_PRS)
		{
			delay();
			GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_12);
		}
	}

	return 0;
}
