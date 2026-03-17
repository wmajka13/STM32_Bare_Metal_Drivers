/*
 * 001I2C_Master_Tx_Testing.c
 *
 *  Created on: Mar 5, 2026
 *      Author: Wiktor Majka
 */

#include<stdio.h>
#include<string.h>
#include "stm32f411xx.h"

#define MY_ADDR 		0x61
#define SLAVE_ADDR 		0x68
#define READ_LEN		0x51
#define READ_DATA		0x52


I2C_Handle_t I2C1Handle = {0};



void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}


//PB8 (stm32) - SCLK - Green - A5 (arduino)
//PB9 (stm32) - SDA - Yellow - A4 (arduino)
void I2C_GPIOInits(void)
{
	GPIO_Handle_t I2CPins = {0};

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALTF_NO_4;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCA
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&I2CPins);

	//SDA
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&I2CPins);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn = {0};

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_Init(&GPIOBtn);
}

void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM; //100kHz

	I2C_Init(&I2C1Handle);
}

int main()
{
	I2C1_Inits();
	I2C_GPIOInits();
	GPIO_ButtonInit();

	//I2C irq config
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	I2C_PeripheralControl(I2C1, ENABLE); //enabling the peripheral

	I2C_ManageAcking(I2C1, ENABLE);

	uint8_t DataLen = 0;
	uint8_t command = 0;
	uint8_t DataRead[100];

	while(1)
	{
		while (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));

		delay();

		command = READ_LEN;
		while (I2C_MasterSendDataIT(&I2C1Handle, &command, 1, SLAVE_ADDR, I2C_RS_ENABLED) != I2C_READY);

		while (I2C_MasterReceiveDataIT(&I2C1Handle, &DataLen, 1, SLAVE_ADDR, I2C_RS_ENABLED) != I2C_READY);

		delay();

		command = READ_DATA;
		while (I2C_MasterSendDataIT(&I2C1Handle, &command, 1, SLAVE_ADDR, I2C_RS_ENABLED) != I2C_READY);

		while (I2C_MasterReceiveDataIT(&I2C1Handle, DataRead, DataLen, SLAVE_ADDR, I2C_RS_DISABLED) != I2C_READY);

	}

}

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{

}



