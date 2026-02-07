/*
 * stm32f411xx_spi_driver.h
 *
 *  Created on: Feb 7, 2026
 *      Author: Wiktor Majka
 */

#ifndef INC_STM32F411XX_SPI_DRIVER_H_
#define INC_STM32F411XX_SPI_DRIVER_H_


#include "stm32f411xx.h"

/*
 * 	Configuration structure for SPI
 */
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
}SPI_Config_t;


/*
 * 	Handle structure for SPIx peripheral
 */
typedef struct
{
	SPI_RegDef_t	*pSPIx;			//Holds the base addres of SPIx
	SPI_Config_t	SPIConfig;
}SPI_Handle_t;


/*****************************************************************************************
 * 								APIs supported by this driver
 * 			For more information about APIs check the function definitions
 *****************************************************************************************/
/*
 *  Peripheral Clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDI); 		/* Enable or Disable clock for given SPI address*/


/*
 * 	Init and De_init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);								/* Setting registers back to theirs original state, done using RCC_AHB1RSTR (example)*/


/*
 *  Data send and receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);


/*
 * 	IRQ Configuration and ISR handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);			/* When there is an interrupt, this function is called */

/*
 * 	Other peripheral control APIs
 */









#endif /* INC_STM32F411XX_SPI_DRIVER_H_ */
