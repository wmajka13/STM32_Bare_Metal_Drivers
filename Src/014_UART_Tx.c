/*
 * 001I2C_Master_Tx_Testing.c
 *
 *  Created on: Mar 5, 2026
 *      Author: Wiktor Majka
 */

#include<stdio.h>
#include<string.h>
#include "stm32f411xx.h"


//PA11 - USART6_TX (alt. mode 8) - green
//PA12 - USART6_RX (alt. mode 8) - yellow

void UART_GPIOInits(void)
{
	GPIO_Handle_t UART_pins = {0};

	UART_pins.pGPIOx = GPIOA;
	UART_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	UART_pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALTF_NO_8;
	UART_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	UART_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	UART_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//Tx
	UART_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_11;
	GPIO_Init(&UART_pins);

	//Rx
	UART_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&UART_pins);
}

//PC13 - button
void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn = {0};

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_Init(&GPIOBtn);
}


USART_Handle_t USART6Handle = {0};

void UART_Inits(void)
{
	USART6Handle.pUSARTx = USART6;
	USART6Handle.USARTConfig.USART_Mode = USART_MODE_ONLY_TX;
	USART6Handle.USARTConfig.USART_Baud = USART_STD_BAUD_115200;
	USART6Handle.USARTConfig.USART_NoOfStopBits = USART_STOPBITS_1;
	USART6Handle.USARTConfig.USART_ParityControl = USART_PARITY_DISABLE;
	USART6Handle.USARTConfig.USART_WordLength = USART_WORDLEN_8BITS;
	USART6Handle.USARTConfig.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;

	USART_Init(&USART6Handle);
}

char msg[1024] = "hello\n";

int main()
{
	UART_GPIOInits();
	GPIO_ButtonInit();
	UART_Inits();
	USART_PeripheralControl(USART6, ENABLE);

	while(1)
	{

		while( GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) ) {};

		USART_SendData(&USART6Handle, (uint8_t*)msg, strlen(msg));
	}

	return 0;
}





