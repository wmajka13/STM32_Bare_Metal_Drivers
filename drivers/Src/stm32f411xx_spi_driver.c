/*
 * stm32f411xx_spi_driver.c
 *
 *  Created on: Feb 7, 2026
 *      Author: Wiktor Majka
 */
#include "stm32f411xx_spi_driver.h"

/*****************************************************************************************************************/
/**************************************		Private function prototypes		**************************************/
/*****************************************************************************************************************/

static inline void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static inline void SPI_RXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static inline void SPI_OVR_ERR_Interrupt_Handle(SPI_Handle_t *pSPIHandle);



/*****************************************************************************************************************/
/**************************************		Global API implementations		**************************************/
/*****************************************************************************************************************/


/**************************************		Peripheral Clock setup		**************************************/

/**
 * Enables/Disables the peripheral clock for SPIx
 *
 * @param pSPIx 	SPI register structure.
 * @param EnOrDi    ENABLE or DISABLE.
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



/**************************************		Init and De_init		**************************************/

/**
 * Initializes the SPIx - sets different registers according to setting in handle, enables peripheral clock
 *
 * @param pSPIHandle Handle for SPIx.
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


/**
 * Deinitializes the SPIx using macros defined in MCU header file. Setting registers back to theirs original state,
 * done using RCC_AHB1RSTR (example)
 *
 * @param pSPIHandle Handle for SPIx
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



/**************************************		Data send and receive		**************************************/


/**
 * Sends data using SPIx in blocking mode (pooling)
 * @note When Length>1 then has to be used in loop in order to receive 1byte of data for 1byte of data transmitted
 *
 * @param pSPIx 		SPIx register structure.
 * @param pTxBuffer    	Pointer to data
 * @param Len    		Length of data
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
			(uint16_t*)pTxBuffer++;	//TODO: TEST IF 16BIT COMMUNICATION WORKS!
		} else
		{
			//8bit DFF
			pSPIx->DR = *pTxBuffer;
			Len--;
			pTxBuffer++;
		}
	}
}


/**
 * Receives data using SPIx in blocking mode (pooling)
 * @note When Length>1 then has to be used in loop in order to receive 1byte of data for 1byte of data transmitted
 *
 * @param pSPIx 		SPIx register structure.
 * @param pRxBuffer    	Pointer to buffer where received data will be written
 * @param Len    		Length of data
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
				*pRxBuffer = pSPIx->DR;
				Len--;
				pRxBuffer++;
			}
		}
	}
}



/**************************************		Data send and receive with interrrupts		**************************************/

/**
 * Starts sending data through SPIx using interrupts - function bascially just notes the information about transmission in tha Handle.
 * The sending of data itself is done in IRQ_handler
 * @note When Length>1 then has to be used in loop in order to receive 1byte of data for 1byte of data transmitted TODO: Repair this
 *
 * @param pSPIHandle 		SPIx handle structur
 * @param pRxBuffer  	  	Pointer to data to be transmitted
 * @param Len    			Length of data
 */
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


/**
 * Receives data using SPIx in interrupt mode - function notes the information about transmission in the handle.
 * Receving of data is done in IRQ_handler
 * @note When Length>1 then has to be used in loop in order to receive 1byte of data for 1byte of data transmitted TODO: Repair this
 *
 * @param pSPIx 		SPIx register structure.
 * @param pRxBuffer    	Pointer to buffer where received data will be written
 * @param Len    		Length of data
 */
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



/**************************************		IRQ Configuration and ISR handling		**************************************/

/**
 * Enables or disables the IRQs for a given SPIx
 *
 * @param IRQNumber 	Macro of an IRQNumber for a given SPIx - specified in MCU specific header file
 * @param EnorDI    	ENABLE or DISABLE.
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


/**
 * Enables or disables the IRQs for a given SPIx
 *
 * @param IRQNumber 		Macro of an IRQNumber for a given SPIx - specified in MCU specific header file
 * @param IRQPriority   	Macro of an IRQPriority - allows to set priorites of IRQs
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. finding out IPR reg
	uint32_t iprx = IRQNumber / 4;
	uint8_t iprx_byte = IRQNumber % 4;

	uint8_t shift_amount = ( 8 * iprx_byte ) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= ( IRQPriority << shift_amount );
}



/**
 * Function that overwrites ISR for a SPIx, it handles the interrupt
 * @note The function will be called only if interrupts are configured! Also works only for TxE, RxNE and OVR events.
 *
 * @param pSPIHandle 		Handle for a given SPIx
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
		SPI_TXE_Interrupt_Handle(pSPIHandle);
	}

	//check SR for RXNE
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_RXNEIE);

	if ( temp1 && temp2 )
	{
		//Handle RXE
		SPI_RXE_Interrupt_Handle(pSPIHandle);
	}

	//check for OVR flag - Will never work - ERRIE control bit is not set.
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_ERRIE);

	if ( temp1 && temp2 )
	{
		//Handle RXE
		SPI_OVR_ERR_Interrupt_Handle(pSPIHandle);
	}

}


/**
 * Function handles the interrupt form TxE bit - it writes data into the DR register.
 * @note	It is basically not functional due to transmitting-receiving, function doesnt recevie data
 *
 * @param pSPIHandle 		Handle for a given SPIx
 */
static inline void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
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
		//Txlen is zero - close the spi transsmision and inform the application that tx is over
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}


/**
 * Function handles the interrupt form RxNE bit - it reads data from the DR register.
 * @note	It is basically not functional due to transmitting-receiving, function doesnt trasnmit data
 *
 * @param pSPIHandle 		Handle for a given SPIx
 */
static inline void SPI_RXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
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
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
	}
}


/**
 * Function handles the interrupt form OVR error - clears the bit and calls application
 * @note	We dont set ERRIE bit anywehere
 *
 * @param pSPIHandle 		Handle for a given SPIx
 */
static inline void SPI_OVR_ERR_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
	//Clear the OVR flag
	//Inform the application
	uint8_t temp;
	//1. clear the ovr flag
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;
	//2. Inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);

}



/**************************************		Setting and clearing flags		**************************************/

/**
 * Enables the SPIx - have to be enabled!
 * @note 				For NSS settings (SSM=0, SSOE=1) works as NSS controller (SPE=1) pulls down NSS.
 *
 * @param pSPIx 		Handle for SPIx registers
 * @param EnOrDi	 	Enable or Disable
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


/**
 * Works when SSM=1 then value of SSI is set as NSS of device using SPIx - has to be set to 1 otherwise allows multimaster mode
 *
 * @param pSPIx 		Handle for SPIx registers
 * @param EnOrDi	 	Enable or Disable
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


/**
 * Sets the SSOE bit - SSOE=0 - multimaster mode, SSOE=1 - NSS is set according to SPE bit
 *
 * @param pSPIx 		Handle for SPIx registers
 * @param EnOrDi	 	Enable or Disable
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


/**
 * Clears the OVR flag by reading DR and SR
 *
 * @param pSPIx 		Handle to SPIx registers
 */
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}


/**
 * Allows to check the status of given flag
 *
 * @param pSPIx 		Handle for SPIx registers
 * @param FlagName	 	Name of the flag (defined as macros)
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}



/**************************************		Other peripheral control APIs		**************************************/

/**
 * Closes the transmission - clear the TxEIE bit, resets buffer and length, sets SPI_READY
 *
 * @param pSPIHandle 		Handle for SPIx
 */
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~( 1 << SPI_CR2_TXEIE );
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}


/**
 * Closes the reception - clear the RxNEIE bit, resets buffer and length, sets SPI_READY
 *
 * @param pSPIHandle 		Handle for SPIx
 */
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~( 1 << SPI_CR2_RXNEIE );
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}



/**************************************		Application callback		**************************************/

/**
 * Function is called after handeled interrupt - allows user to do something connected to the interrupt - for example resolve OVR error.
 * @note 	__weak = __attribute__((weak))
 *
 * @param pSPIHandle 		Handle for SPIx
 * @param AppEv 			Event in application (defined macro)
 */
__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	//weak implementation that can be overide by application
}




