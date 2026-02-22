/*
 * stm32f411xx_i2c_driver.c
 *
 *  Created on: Feb 18, 2026
 *      Author: Wiktor Majka
 */


#include "stm32f411_i2c_driver.h"

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint8_t APB1_PreScaler[4] = {2, 4, 8, 16};
/*
 *  Peripheral Clock setup
 */


void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDI)
{
	if (EnorDI == ENABLE)
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		} else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		} else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	} else
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		} else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		} else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}


/*
 * 	Init and De_init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t tempreg = 0;
	//1. Configure the Mode (standard or fast )

	//2.1 Configure the speed of the serial clock (SCL), configuring the freq in cr2
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value() / 10e6U;
	pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F); //masking because we only want to se first 5bits

	//2.2 Configure the CCR
	uint16_t ccr_value = 0;
	tempreg = 0;

	//setting the F/S bit and CCRbits in CCR
	if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{

		ccr_value = RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		tempreg |= (ccr_value & 0xFFF);
	} else
	{
		tempreg |= (1 << I2C_CCR_FS);
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY);
		if (pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
		{
			ccr_value = RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		} else
		{
			ccr_value = RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}
		tempreg |= (ccr_value & 0xFFF);
	}

	pI2CHandle->pI2Cx->CCR = tempreg;


	//3. Configure the device address (Applicable when device is slave)
	tempreg = 0;
	tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
	tempreg |= ( 1 << 14 ); //manual says that this bit should always be kept high by software
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	//4. Enable the Acking
	tempreg = 0;
	tempreg |= pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	//5. Configure the rise time for I2C pins (will discuss later )
}


void I2C_DeInit(I2C_RegDef_t *pI2Cx) /* Setting registers back to theirs original state, done using RCC_AHB1RSTR (example)*/
{
	if(pI2Cx == I2C1)
	{
		I2C1_REG_RESET();
	} else if(pI2Cx == I2C2)
	{
		I2C2_REG_RESET();
	} else if(pI2Cx == I2C3)
	{
		I2C3_REG_RESET();
	}
}


/*
 * 	IRQ Configuration and ISR handling
 */

/***********************************************************
 * @fn			- I2C_IRQInterruptConfig
 *
 * @brief 		- Enables/Disables ability to use Interrupt from I2C
 *
 * @param[IRQNumber]		- Number of interrupt
 * @param[EnorDI]			- Enable or Disable macro
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI)
{
	if(EnorDI == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 Reg
			*NVIC_ISER0 |= ( 1 << IRQNumber );

		} else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//program ISER1 Reg
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );

		}
		else if(IRQNumber >= 64 && IRQNumber <= 96)
		{
			//program ISER2 reg
			*NVIC_ISER2 |= ( 1 << (IRQNumber % 64) );
		}
	} else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 Reg
			*NVIC_ICER0 |= ( 1 << IRQNumber );

		} else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//program ICER1 Reg
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );

		}
		else if(IRQNumber >= 64 && IRQNumber <= 96)
		{
			//program ICER2 reg
			*NVIC_ICER2 |= ( 1 << (IRQNumber % 64) );
		}
	}
}

/***********************************************************
 * @fn			- GPIO_IRQPriorityConfig
 *
 * @brief 		- allows to set a interrupt priority
 *
 * @param[IRQNumber]		- Number of interrupt
 * @param[IRQPriority]		- Priority of interrupt
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. finding out IPR reg
	uint32_t iprx = IRQNumber / 4;
	uint8_t iprx_byte = IRQNumber % 4;

	uint8_t shift_amount = ( 8 * iprx_byte ) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= ( IRQPriority << shift_amount );
}


/*
 * 	Other peripheral control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	} else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}

}

uint32_t RCC_GetPullOutputClock(void)
{
	return 0;
}



uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t pclk1, SystemClk;
	uint8_t clksrc, temp, ahbp, apb1p;

	clksrc = ( (RCC->CFGR >> 2) & 0x3); //moves bit 3 and 2 at the beggings and masks

	if (clksrc == 0)
	{
		//sysclk = HSI
		SystemClk = 16e6; //16Mhz

	} else if (clksrc == 1)
	{
		//HSE
		SystemClk = 8e6; //8Mhz
	} else if (clksrc == 2)
	{
		//PLL - another func to calculate this needed - for now not implementend TODO
		SystemClk = RCC_GetPullOutputClock();
	}

	temp = ( (RCC->CFGR >> 4) & 0xF); //value of ahb prescaler in code
	if (temp < 8)
	{
		ahbp = 1;
	} else
	{
		ahbp = AHB_PreScaler[temp-8]; //We have an array that holds possible prescalers: for prescaler=2 code is 8, 4->9, 8->10
	}

	temp = ( (RCC->CFGR >> 10) & 0x7); //value of ahb prescaler in code
	if (temp < 4)
	{
		apb1p = 1;
	} else
	{
		apb1p = APB1_PreScaler[temp-4]; //We have an array that holds possible prescalers: for prescaler=2 code is 8, 4->9, 8->10
	}

	pclk1 = (SystemClk / ahbp) / apb1p;

	return pclk1;
}
