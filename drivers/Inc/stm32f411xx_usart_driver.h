/*
 * stm32f411xx_usart_driver.h
 *
 *  Created on: Feb 25, 2026
 *      Author: Wiktor Majka
 */

#ifndef INC_STM32F411XX_USART_DRIVER_H_
#define INC_STM32F411XX_USART_DRIVER_H_


#include "stm32f411xx.h"

/*
 * 	Configuration structure for USART
 */
typedef struct
{
	uint8_t USART_Mode;
	uint32_t USART_Baud;
	uint8_t USART_NoOfStopBits;
	uint8_t USART_WordLength;
	uint8_t USART_ParityControl;
	uint8_t USART_HWFlowControl;

}USART_Config_t;


/*
 * 	Handle structure for USARTx peripheral
 */
typedef struct
{
	USART_RegDef_t *pUSARTx;
	USART_Config_t USARTConfig


}USART_Handle_t;


#endif /* INC_STM32F411XX_USART_DRIVER_H_ */
