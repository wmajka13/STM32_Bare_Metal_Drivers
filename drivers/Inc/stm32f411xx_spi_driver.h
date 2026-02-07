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












#endif /* INC_STM32F411XX_SPI_DRIVER_H_ */
