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
	uint8_t SPI_DeviceMode;						// Choose from @SPI_DeviceMode
	uint8_t SPI_BusConfig;						// Choose from @SPI_BusConfig
	uint8_t SPI_SclkSpeed;						// Choose from @SPI_SclkSpeed
	uint8_t SPI_DFF;							// Choose from @SPI_DFF
	uint8_t SPI_CPOL;							// Choose from @SPI_CPOL
	uint8_t SPI_CPHA;							// Choose from @SPI_CPHA
	uint8_t SPI_SSM;							// Choose from @SPI_SSM
}SPI_Config_t;


/*
 * 	Handle structure for SPIx peripheral
 */
typedef struct
{
	SPI_RegDef_t	*pSPIx;			//Holds the base addres of SPIx
	SPI_Config_t	SPIConfig;
}SPI_Handle_t;


/*
 * @SPI_DeviceMode
 */
#define SPI_DEVICE_MODE_MASTER			1
#define SPI_DEVICE_MODE_SLAVE			0


/*
 * @SPI_BusConfig
 */
#define SPI_BUS_CONFIG_FD						0
#define SPI_BUS_CONFIG_HD						1
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY			2


/* Written in SPI register - SPI control register 1 - BR (Baud rate control) part - chooses the F_PCLK prescaler 2, 4, 8 ... 256
 * @SPI_SclkSpeed
 */
#define SPI_SCLK_SPEED_DIV2						0
#define SPI_SCLK_SPEED_DIV4						1
#define SPI_SCLK_SPEED_DIV8						2
#define SPI_SCLK_SPEED_DIV16					3
#define SPI_SCLK_SPEED_DIV32					4
#define SPI_SCLK_SPEED_DIV64					5
#define SPI_SCLK_SPEED_DIV128					6
#define SPI_SCLK_SPEED_DIV256					7

/*
 * @SPI_DFF
 */
#define SPI_DIFF_8BITS							0
#define SPI_DIFF_16BITS 						1


/*
 * @SPI_CPOL
 */
#define SPI_CPOL_HIGH							1
#define SPI_CPOL_LOW							0


/*
 * @SPI_CPHA
 */
#define SPI_CPHA_HIGH							1
#define SPI_CPHA_LOW							0


/*
 * @SPI_SSM
 */
#define SPI_SSM_EN								1
#define SPI_SSM_DI								0


/*
 *  SPI related status flag deifinitions
 */
#define SPI_RXNE_FLAG 					( 1 << SPI_SR_RXNE )
#define SPI_TXE_FLAG 					( 1 << SPI_SR_TXE )
#define SPI_CHSIDE_FLAG 				( 1 << SPI_SR_CHSIDE )
#define SPI_UDR_FLAG 					( 1 << SPI_SR_UDR )
#define SPI_CRCERR_FLAG 				( 1 << SPI_SR_CRCERR )
#define SPI_MODF_FLAG 					( 1 << SPI_SR_MODF )
#define SPI_OVR_FLAG 					( 1 << SPI_SR_OVR )
#define SPI_BSY_FLAG 					( 1 << SPI_SR_BSY )
#define SPI_FRE_FLAG 					( 1 << SPI_SR_FRE )



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
