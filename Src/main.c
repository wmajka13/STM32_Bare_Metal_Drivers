/*
 * 003led_button_ext.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Wiktor Majka
 */


#include "stm32f411xx.h"


void delay(void)
{
	for(uint32_t i = 0 ; i< 500000/2 ; i ++);
}

int main(void)
{
	GPIO_Handle_t GpioLed, GPIOBtn = {0};	//Stowrzenie handla

	GpioLed.pGPIOx = GPIOB;					//Przypisanie portu
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GpioLed);

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOC, ENABLE);
	GPIO_Init(&GPIOBtn);


	//Configuring the IRQ
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRI15);

	while(1);

	return 0;
}

//Overwrite the ISR function
void EXTI15_10_IRQHandler(void)
{
	delay();
	GPIO_IRQHandling(GPIO_PIN_NO_10);
	GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_12);
}
