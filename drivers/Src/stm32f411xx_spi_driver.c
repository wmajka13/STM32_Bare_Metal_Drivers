/*
 * stm32f411xx_spi_driver.c
 *
 *  Created on: Feb 7, 2026
 *      Author: Wiktor Majka
 */
#include "stm32f411xx_spi_driver.h"

/*
 *  Peripheral Clock setup
 */

/***********************************************************
 * @fn			- SPI_PeriClockControl
 *
 * @brief 		- Enables or disables clock on GPIO's bus
 *
 * @param[pSPIx]		- Pointer to structure defining SPI's registers (it is also a base address of SPI)
 * @param[EnorDi]		- ENABLE or DISABLE macros
 *
 * @return		- None
 *
 * @note		- None
 *
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDI)
{

}


/*
 * 	Init and De_init
 */

/***********************************************************
 * @fn			- SPI_Init
 *
 * @brief 		- Initializes SPI
 *
 * @param[pSPIHandle]		- Pointer to structure defining SPI's handle
 *
 * @return		- None
 *
 * @note		- None
 *
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{

}

/***********************************************************
 * @fn			- SPI_DeInit
 *
 * @brief 		- Deinitializes SPI
 *
 * @param[pSPIx]		- Pointer to structure defining SPI's registers
 *
 * @return		- None
 *
 * @note		- None
 *
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{

}


/*
 *  Data send and receive
 */

/***********************************************************
 * @fn			- SPI_SendData
 *
 * @brief 		- Allows to send data
 *
 * @param[pSPIx]			- Pointer to structure defining SPI's registers
 * @param[pTxBuffer]		- pointer to a buffer that holds message to be transmitted
 * @param[Len]				- Length of this message
 *
 * @return		- None
 *
 * @note		- None
 *
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{

}

/***********************************************************
 * @fn			- SPI_ReceiveData
 *
 * @brief 		- allows to receive data through SPI
 *
 * @param[pSPIx]		- Pointer to structure defining SPI's registers
 * @param[pRxBuffer]	- Pointer to a buffer in which received data will be written
 * @param[Len]			- length of received data
 *
 * @return		- None
 *
 * @note		- None
 *
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{

}


/*
 * 	IRQ Configuration and ISR handling
 */

/***********************************************************
 * @fn			- SPI_IRQInterruptConfig
 *
 * @brief 		- Enables/Disables ability to use Interrupt from SPI
 *
 * @param[IRQNumber]		- Number of interrupt
 * @param[EnorDI]			- Enable or Disable macro
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI)
{

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
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{

}

/***********************************************************
 * @fn			- GPIO_IRQHandling
 *
 * @brief 		- Clearing interrupt flag, function is called when there is an interrupt, it is called in real Handler
 *
 * @param[pSPIHandle]		- structure - handle of SPI
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{

}


/*
 * 	Other peripheral control APIs
 */


