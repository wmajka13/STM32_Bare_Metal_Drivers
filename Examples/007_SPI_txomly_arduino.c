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
 * PA4 -> NSS
 * PA5 -> SCLK
 * PA6 -> MISO
 * PA7 -> MOSI
 */

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

int main(void){

	char user_data[] = "Hello world";

	//This function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI1_GPIOInits();

	SPI1_Inits();

	SPI_SSOEConfig(SPI1, ENABLE);

	GPIO_ButtonInit();

	while(1)
	{

		while (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
		delay();

		//enable the SPI1 peripheral
		SPI_PeripheralControl(SPI1, ENABLE);

		//first send lentgh of data
		uint8_t DataLen = strlen(user_data);
		SPI_SendData(SPI1, &DataLen, 1);

		//sending data
		SPI_SendData(SPI1, (uint8_t*)user_data, strlen(user_data));

		while ( SPI_GetFlagStatus(SPI1, SPI_BSY_FLAG) );

		SPI_PeripheralControl(SPI1, DISABLE);
	}

	return 0;
}
