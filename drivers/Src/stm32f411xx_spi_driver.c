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
	if(EnorDI == ENABLE)
	{

		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		} else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		} else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		} else if(pSPIx == SPI4)
		{
			SPI4_PCLK_EN();
		} else if(pSPIx == SPI5)
		{
			SPI5_PCLK_EN();
		}

	} else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		} else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		} else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		} else if(pSPIx == SPI4)
		{
			SPI4_PCLK_DI();
		} else if(pSPIx == SPI5)
		{
			SPI5_PCLK_DI();
		}
	}
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
	//0. enable peripheral clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	//Configuring the SPI_CR1 register
	uint32_t tempreg = 0;

	//1. Configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_BusConfig << 2;

	//2. Configure the bus config
	if (pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_BUS_CONFIG_FD)
	{
		tempreg &= ~( 1 << SPI_CR1_BIDIMODE );
	} else if (pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_BUS_CONFIG_FD)
	{
		tempreg |= ( 1 << SPI_CR1_BIDIMODE );
	} else if (pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_BUS_CONFIG_FD)
	{
		tempreg &= ~( 1 << SPI_CR1_BIDIMODE );
		tempreg |= ( 1 << SPI_CR1_BIDIOE );
	}

	//3. Configure the SCLK speed
	tempreg |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);

	//4. Configure the DFF
	tempreg |= (pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);

	//5. Configure the CPOL
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);

	//6. Configure the CPHA
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);

	//7. Configure the SSM
	tempreg |= (pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

	pSPIHandle->pSPIx->CR1 = tempreg;
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
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	} else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	} else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	} else if(pSPIx == SPI4)
	{
		SPI4_REG_RESET();
	} else if(pSPIx == SPI5)
	{
		SPI5_REG_RESET();
	}
}



uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
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
 * @param[Len]				- Length of this message in bytes
 *
 * @return		- None
 *
 * @note		- This is a blocking call - 2 while functions.
 *
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while (Len > 0)
	{
		//1. wait until TXE is SET
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		//2. Check the dff bit in cr1
		if( (pSPIx->CR1 & (1 << SPI_CR1_DFF) ) )
		{
			//16bit DFF
			//1. Load data into DR
			pSPIx->DR = *((uint16_t*)pTxBuffer); //Trzeba ztypecastowac żeby to był pointer pokazujący na 2bajty, wtedy dereferncja wyciągnie 2 bajty
			Len--;
			Len--;
			(uint16_t*)pTxBuffer++;
		} else
		{
			//8bit DFF
			pSPIx->DR = *pTxBuffer;
			Len--;
			pTxBuffer++;
		}
	}
}

/***********************************************************
 * @fn			- SPI_ReceiveData
 *
 * @brief 		- allows to receive data through SPI
 *
 * @param[pSPIx]		- Pointer to structure defining SPI's registers
 * @param[pRxBuffer]	- Pointer to a buffer in which received data will be written
 * @param[Len]			- length of received data in bytes
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

/***********************************************************
 * @fn			- SPI_PeripheralControl
 *
 * @brief 		- Enables the SPE bit in SPI_CR1 reg that enables the SPI
 *
 * @param[pSPIHandle]		- structure of spi registers
 * @param[EnOrDi]			- ENABLE OR DISABLE
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	} else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}

}







