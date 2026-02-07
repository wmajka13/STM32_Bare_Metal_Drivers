/*
 * stm32f411xx_gpio_driver.h
 *
 *  Created on: Jan 9, 2026
 *      Author: Wiktor Majka
 */

#ifndef INC_STM32F411XX_GPIO_DRIVER_H_
#define INC_STM32F411XX_GPIO_DRIVER_H_

#include "stm32f411xx.h"

/*
 * 	Configuration structure for a GPIO pin
 */
typedef struct
{
	uint8_t GPIO_PinNumber;							/* possible values from @GPIO_PIN_NUMBER */
	uint8_t GPIO_PinMode;							/* possible values from @GPIO_PIN_MODES */
	uint8_t GPIO_PinSpeed;							/* possible values from @GPIO_PIN_SPEED */
	uint8_t GPIO_PinPuPdControl;					/* possible values from @GPIO_PIN_PUPD_CONTROL */
	uint8_t GPIO_PinOPType;							/* possible values from @GPIO_PIN_OP_TYPE */
	uint8_t GPIO_PinAltFunMode;						/* possible values from @GPIO_PIN_ALT_FUNC_MODE */
}GPIO_PinConfig_t;


/*
 * 	Handle structure for GPIO pin
 */
typedef struct
{
	GPIO_RegDef_t *pGPIOx;				/* This holds the base address of the GPIO port to which the pin belongs */
	GPIO_PinConfig_t GPIO_PinConfig;	/* This holds GPIO pin configuration settings */

}GPIO_Handle_t;

/*	@GPIO_PIN_NUMBER
 * 	GPIO pin numbers
 */
#define GPIO_PIN_NO_0 			0
#define GPIO_PIN_NO_1 			1
#define GPIO_PIN_NO_2 			2
#define GPIO_PIN_NO_3 			3
#define GPIO_PIN_NO_4 			4
#define GPIO_PIN_NO_5 			5
#define GPIO_PIN_NO_6 			6
#define GPIO_PIN_NO_7 			7
#define GPIO_PIN_NO_8 			8
#define GPIO_PIN_NO_9 			9
#define GPIO_PIN_NO_10 			10
#define GPIO_PIN_NO_11 			11
#define GPIO_PIN_NO_12 			12
#define GPIO_PIN_NO_13 			13
#define GPIO_PIN_NO_14 			14
#define GPIO_PIN_NO_15 			15


/*	@GPIO_PIN_MODES
 *  GPIO Pin possible modes
 */
#define GPIO_MODE_IN 		0
#define GPIO_MODE_OUT 		1
#define GPIO_MODE_ALTFN 	2
#define GPIO_MODE_ANALOG 	3
#define GPIO_MODE_IT_FT		4	/* Falling edge interrupt */
#define GPIO_MODE_IT_RT 	5	/* Rising edge interrupt */
#define GPIO_MODE_IT_RFT 	6	/* Rising and Falling edge interrupt */


/* 	@GPIO_PIN_OP_TYPE
 *  GPIO pin possible output types
 */
#define GPIO_OP_TYPE_PP 	0
#define GPIO_OP_TYPE_OD 	1


/*	@GPIO_PIN_SPEED
 *  GPIO pin possible output speeds
 */
#define GPIO_SPEED_LOW		0
#define GPIO_SPEED_MEDIUM	1
#define GPIO_SPEED_FAST 	2
#define GPIO_SPEED_HIGH 	3


/* 	@GPIO_PIN_PUPD_CONTROL
 *  GPIO pin possible pull-up/pull-down
 */
#define GPIO_NO_PUPD 		0
#define GPIO_PIN_PU			1
#define GPIO_PIN_PD			2


/* 	@GPIO_PIN_ALT_FUNC_MODE
 *  GPIO pin possible alt functions mode
 */
#define GPIO_ALTF_NO_0			0
#define GPIO_ALTF_NO_1			1
#define GPIO_ALTF_NO_2			2
#define GPIO_ALTF_NO_3			3
#define GPIO_ALTF_NO_4			4
#define GPIO_ALTF_NO_5			5
#define GPIO_ALTF_NO_6			6
#define GPIO_ALTF_NO_7			7
#define GPIO_ALTF_NO_8			8
#define GPIO_ALTF_NO_9			9
#define GPIO_ALTF_NO_10			10
#define GPIO_ALTF_NO_11			11
#define GPIO_ALTF_NO_12			12
#define GPIO_ALTF_NO_13			13
#define GPIO_ALTF_NO_14			14
#define GPIO_ALTF_NO_15			15

/*****************************************************************************************
 * 								APIs supported b this driver
 * 			For more information about APIs check the function definitions
 *****************************************************************************************/

/*
 *  Peripheral Clock setup
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDI); 		/* Enable or Disable clock for given GPIO address*/

/*
 * 	Init and De_init
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);								/* Setting registers back to theirs original state, done using RCC_AHB1RSTR (example)*/

/*
 * 	Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

/*
 * 	IRQ Configuration and ISR handling
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);			/* When there is an interrupt, this function is called */




















#endif /* INC_STM32F411XX_GPIO_DRIVER_H_ */
