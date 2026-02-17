/*
 * stm32f411xx_spi_driver.c
 *
 *  Created on: Feb 7, 2026
 *      Author: Wiktor Majka
 */
#include "stm32f411xx_spi_driver.h"


static void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static void SPI_RXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static void SPI_OVR_ERR_Interrupt_Handle(SPI_Handle_t *pSPIHandle);



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
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << 2;

	//2. Configure the bus config
	if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		tempreg &= ~( 1 << SPI_CR1_BIDIMODE );
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		tempreg |= ( 1 << SPI_CR1_BIDIMODE );
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
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
	while (Len > 0)
	{
		while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG)) //return 1 if its not empty
		{
			if ( pSPIx->CR1 & (1 << SPI_CR1_DFF) )
			{
				*((uint16_t*)pRxBuffer) = pSPIx->DR;
				Len--;
				Len--;
				(uint16_t*)pRxBuffer++;
			} else
			{
				pRxBuffer = pSPIx->DR;
				Len--;
				pRxBuffer++;
			}
		}
	}
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
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. finding out IPR reg
	uint32_t iprx = IRQNumber / 4;
	uint8_t iprx_byte = IRQNumber % 4;

	uint8_t shift_amount = ( 8 * iprx_byte ) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= ( IRQPriority << shift_amount );
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
	uint8_t temp1, temp2;
	//check SR for TXE
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_TXE);
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_TXEIE);

	if ( temp1 && temp2 )
	{
		//Handle TXE
		SPI_TXE_Interrupt_Handle();
	}

	//check SR for RXNE
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_RXNEIE);

	if ( temp1 && temp2 )
	{
		//Handle RXE
		SPI_RXE_Interrupt_Handle();
	}

	//check for OVR flag
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_ERRIE);

	if ( temp1 && temp2 )
	{
		//Handle RXE
		SPI_OVR_ERR_Interrupt_Handle();
	}

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

/***********************************************************
 * @fn			- SPI_SSIConfig
 *
 * @brief 		- Enables the SSI bit in SPI_CR1 reg that sets NSS to '1' innternally
 *
 * @param[pSPIHandle]		- structure of spi registers
 * @param[EnOrDi]			- ENABLE OR DISABLE
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	} else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}

}


/***********************************************************
 * @fn			- SPI_SSOEConfig
 *
 * @brief 		- Enables the SSOE bit in SPI_CR2 reg that sets NSS to '0' when SPE bit is '1'
 *
 * @param[pSPIHandle]		- structure of spi registers
 * @param[EnOrDi]			- ENABLE OR DISABLE
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	} else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}

}


uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if (state != SPI_BUSY_IN_TX)
	{
	//1. Save the Tx buffer address and Len info into global variables
	pSPIHandle->pTxBuffer = pTxBuffer;
	pSPIHandle->TxLen = Len;
	//2. Mark the SPI as busy
	pSPIHandle->TxState = SPI_BUSY_IN_TX;
	//3. Enable the TXEIE control bit to get interrrupt whenever Txe flag is set in SR
	pSPIHandle->pSPIx->CR2 |= ( 1 << SPI_CR2_TXEIE );
	//4. Data transmission will be handeled by the ISR code.
	}

	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if (state != SPI_BUSY_IN_RX)
	{
	//1. Save the Tx buffer address and Len info into global variables
	pSPIHandle->pRxBuffer = pRxBuffer;
	pSPIHandle->RxLen = Len;
	//2. Mark the SPI as busy
	pSPIHandle->RxState = SPI_BUSY_IN_RX;
	//3. Enable the TXEIE control bit to get interrrupt whenever Txe flag is set in SR
	pSPIHandle->pSPIx->CR2 |= ( 1 << SPI_CR2_RXNEIE );
	//4. Data transmission will be handeled by the ISR code.
	}

	return state;
}



/*
 * 	Handling helper functions
 */

static void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
	if( (pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF) ) )
	{
		//16bit DFF
		//1. Load data into DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		(uint16_t*)pSPIHandle->pTxBuffer++;
	} else
	{
		//8bit DFF
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if (! pSPIHandle->TxLen)
	{
		//Txlen is zero - close the spi transsmision and inform the application taht tx is over
		pSPIHandle->pSPIx->CR2 &= ~( 1 << SPI_CR2_TXEIE );
		pSPIHandle->pTxBuffer = NULL;
		pSPIHandle->TxState = SPI_READY;
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}


static void SPI_RXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
	if ( pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF) )
	{
		*((uint16_t*)pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2;
		(uint16_t*)pSPIHandle->pRxBuffer++;
	} else
	{
		*(pSPIHandle->pRxBuffer) = (uint8_t) pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if (! pSPIHandle->RxLen)
	{
		//Txlen is zero - close the spi transsmision and inform the application that tx is over
		pSPIHandle->pSPIx->CR2 &= ~( 1 << SPI_CR2_RXNEIE );
		pSPIHandle->pRxBuffer = NULL;
		pSPIHandle->RxState = SPI_READY;
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
	}
}


static void SPI_OVR_ERR_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
	//Clear the OVR flag
	//Inform the application

}




