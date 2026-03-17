/*
 * 013_I2C_Slave_Tx_Testing.c
 *
 *  Created on: Mar 13, 2026
 *      Author: Wiktor Majka
 */


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

uint8_t TxBuffer[32] = "Hello Hello Hello!";

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
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
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
	I2C1Handle.I2C_Config.I2C_DeviceAddress = SLAVE_ADDR;
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

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	I2C_PeripheralControl(I2C1, ENABLE); //enabling the peripheral

	I2C_ManageAcking(I2C1, ENABLE);
}

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}


volatile uint8_t command = 0;
volatile uint8_t cnt = 0;
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{


	if (AppEv == I2C_EV_DATA_REQ)
	{
		//Master is requesting the data
		if (command == READ_LEN)
		{
			I2C_SlaveSendData(pI2CHandle->pI2Cx, strlen((char*)TxBuffer));
		} else if (command == READ_DATA)
		{
			I2C_SlaveSendData(pI2CHandle->pI2Cx, TxBuffer[cnt++]);
		} else
		{
			I2C_SlaveSendData(pI2CHandle->pI2Cx, 0xFF);
		}

	} else if (AppEv == I2C_EV_DATA_RCV)
	{
		//Master is sending data
		command = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);
	} else if (AppEv == I2C_ERROR_AF)
	{
		//Master has send NACK - no more data
		command = 0xFF;
		cnt = 0;
	} else if (AppEv == I2C_EV_STOP)
	{
		//Master has ended the communication
	}
}



