/*
 * stm32f411xx_i2c_driver.c
 *
 *  Created on: Feb 18, 2026
 *      Author: Wiktor Majka
 */


#include "stm32f411_i2c_driver.h"



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
			I2C1_PCLK_DI()();
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

