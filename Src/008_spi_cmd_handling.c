/*
 * 006spi_tx_testing.c
 *
 *  Created on: Feb 8, 2026
 *      Author: Wiktor Majka
 */

#include <string.h>
#include "stm32f411xx.h"

/*
 * SPI1, Alternate functionality mode 0 AF05
 * PA4 -> NSS, CH4
 * PA5 -> SCLK, CH1
 * PA6 -> MISO, CH3
 * PA7 -> MOSI, CH2
 */

//COMMAND CODES
#define COMMAND_LED_CTRL				0x50
#define COMMAND_SENSOR_READ				0x51
#define COMMAND_LED_READ				0x52
#define COMMAND_PRINT					0x53
#define COMMAND_ID_READ					0x54

#define LED_ON 							1
#define LED_OFF 						0

//ARDUINO ANALOG PINS
#define ANALOG_PIN0						0
#define ANALOG_PIN1						1
#define ANALOG_PIN2						2
#define ANALOG_PIN3						3
#define ANALOG_PIN4						4
#define ANALOG_PIN5						5

//ARDUINO LED
#define ARDUINO_LED_PIN 				9



void delay(void)
{
	for(uint32_t i = 0 ; i< 500000/2 ; i ++);
}


void SPI1_GPIOInits(void)
{
	GPIO_Handle_t SPIpins = {0};

	SPIpins.pGPIOx = GPIOA;
	SPIpins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIpins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALTF_NO_5;
	SPIpins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIpins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIpins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;

	//SCLK
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_Init(&SPIpins);

	//MISO
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&SPIpins);

	//MOSI
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&SPIpins);

	//NSS
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_4;
	GPIO_Init(&SPIpins);

}

void SPI1_Inits(void)
{
	SPI_Handle_t SPI1handle;

	SPI1handle.pSPIx = SPI1;
	SPI1handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI1handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV64;
	SPI1handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI1handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI1handle.SPIConfig.SPI_CPOL = SPI_CPOL_HIGH;
	SPI1handle.SPIConfig.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&SPI1handle);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn;								//Stowrzenie handla

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOC, ENABLE);
	GPIO_Init(&GPIOBtn);
}

//code for ACK is 0xF5
uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{
	if (ackbyte == 0xF5)
	{
		//ack
		return 1;
	}
	//nack
	return 0;
}


int main(void){

	uint8_t dummy_write = 0xFF;
	uint8_t dummy_read;

	//This function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI1_GPIOInits();

	SPI1_Inits();

	SPI_SSOEConfig(SPI1, ENABLE);

	GPIO_ButtonInit();

	while(1)
	{

		while (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));

		//Debouncing
		delay();

		//enable the SPI1 peripheral
		SPI_PeripheralControl(SPI1, ENABLE);

		//1. CMD_LED_CTRL
		uint8_t commandcode = COMMAND_LED_CTRL;
		uint8_t ackbyte;
		uint8_t args[2];

		SPI_SendData(SPI1, &commandcode, 1); //sending the command code
		// we have to do dummy read in order to clear RXNE bit
		SPI_ReceiveData(SPI1, &dummy_read, 1);

		SPI_SendData(SPI1, &dummy_write, 1); //sending the dummy byte to push the answer from arudino
		SPI_ReceiveData(SPI1, &ackbyte, 1);

		if ( SPI_VerifyResponse(ackbyte) )
		{
			//Send arguments
			args[0] = ARDUINO_LED_PIN;
			args[1] = LED_ON;

			SPI_SendData(SPI1, args, 2);
		}

		while ( SPI_GetFlagStatus(SPI1, SPI_BSY_FLAG) );

		SPI_PeripheralControl(SPI1, DISABLE);
	}

	return 0;
}
