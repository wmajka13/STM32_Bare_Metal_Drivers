/*
 * stm32f411_i2c_driver.h
 *
 *  Created on: Feb 18, 2026
 *      Author: Wiktor Majka
 */

#ifndef INC_STM32F411_I2C_DRIVER_H_
#define INC_STM32F411_I2C_DRIVER_H_

#include "stm32f411xx.h"

/*
 * 	Configuration structure for I2C
 */

typedef struct
{
	uint32_t I2C_SCLSpeed;
	uint8_t I2C_DeviceAddress;
	uint8_t I2C_ACKControl;
	uint16_t I2C_FMDutyCycle;

}I2C_Config_t;


/*
 * 	Handle structure for I2Cx peripheral
 */
typedef struct
{
	I2C_RegDef_t *pI2Cx;
	I2C_Config_t I2C_Config;

}I2C_Handle_t;


/*
 * 	@I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_SM			100000
#define I2C_SCL_SPEED_FM4K			400000
#define I2C_SCL_SPEED_FM2K			200000

/*
 * 	@I2C_ACKControl
 */
#define I2C_ACK_ENABLE				1
#define I2C_ACK_DISABLE				0

/*
 * 	@I2C_FMDutyCycle
 */
#define I2C_FM_DUTY_2				0
#define I2C_FM_DUTY_16_9			1



/*****************************************************************************************
 * 								APIs supported by this driver
 * 			For more information about APIs check the function definitions
 *****************************************************************************************/
/*
 *  Peripheral Clock setup
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDI); 		/* Enable or Disable clock for given I2C address*/


/*
 * 	Init and De_init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);								/* Setting registers back to theirs original state, done using RCC_AHB1RSTR (example)*/

/*
 * 	IRQ Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);

/*
 * 	Other peripheral control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);

/*
 * 	Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);


#endif /* INC_STM32F411_I2C_DRIVER_H_ */
