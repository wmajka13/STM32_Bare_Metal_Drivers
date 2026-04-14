	/*
 * stm32f411xx_usart_driver.c
 *
 *  Created on: Feb 25, 2026
 *      Author: Wiktor Majka
 */

#include "stm32f411xx_usart_driver.h"


 /*
 * Peripheral Clock setup
 */

 void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
 {
    if (EnorDi == ENABLE)
    {
        if (pUSARTx == USART1)
        {
            USART1_PCLK_EN();
        } else if (pUSARTx == USART2)
        {
            USART2_PCLK_EN();
        } else if (pUSARTx == USART6)
        {
            USART6_PCLK_EN();
        }
    } else 
    {
        if (pUSARTx == USART1)
        {
            USART1_PCLK_DI();
        } else if (pUSARTx == USART2)
        {
            USART2_PCLK_DI();
        } else if (pUSARTx == USART6)
        {
            USART6_PCLK_DI();
        }
    }
 }


/*
 * Other Peripheral Control APIs
 */
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi)
{
    if (EnOrDi == ENABLE)
    {
        pUSARTx->CR1 |= (1 <<  USART_CR1_UE);
    } else 
    {
        pUSARTx->CR1 &= ~(1 <<  USART_CR1_UE);
    }
}

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx , uint32_t FlagName)
{
	if(pUSARTx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName)
{
//TODO:
}

/*
 * IRQ Configuration and ISR handling
 */
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
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

void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. finding out IPR reg
	uint32_t iprx = IRQNumber / 4;
	uint8_t iprx_byte = IRQNumber % 4;

	uint8_t shift_amount = ( 8 * iprx_byte ) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= ( IRQPriority << shift_amount );
}
	


void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{
	uint32_t temp1, temp2, temp3;

/************************* Check for TC (Transmission Complete) flag ********************/

	// 1. Check the state of TC bit in the SR and TCIE bit in CR1
	temp1 = (pUSARTHandle->pUSARTx->CR1 >> USART_CR1_TCIE) & 0x1;
	temp2 = (pUSARTHandle->pUSARTx->SR >> USART_SR_TC) & 0x1;
	// 2. If both are set, it means the interrupt is because of TC
	if (temp1 && temp2)
	{
	// 3. If TxBusyState is USART_BUSY_IN_TX and TxLen is 0:
		if (pUSARTHandle->TxState == USART_BUSY_IN_TX && pUSARTHandle->TxLen == 0)
		{
	//    - Clear the TC flag
			pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_TC);
	//    - Clear the TCIE control bit
			pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TCIE);
	//    - Reset the application state (TxBusyState) to USART_READY
			pUSARTHandle->TxState = USART_READY;
	//    - Reset Buffer address to NULL
			pUSARTHandle->pTxBuffer = NULL;
	//    - Reset the length to zero
			pUSARTHandle->TxLen = 0;
	//    - Call the application callback with USART_EVENT_TX_CMPLT
			USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_TX_CMPLT);
		}
	}
	
/************************* Check for TXE (Transmit Data Register Empty) flag ************/

	// 1. Check the state of TXE bit in the SR and TXEIE bit in CR1
	temp1 = (pUSARTHandle->pUSARTx->CR1 >> USART_CR1_TXEIE) & 0x1;
	temp2 = (pUSARTHandle->pUSARTx->SR >> USART_SR_TXE) & 0x1;
	uint16_t* pdata;
	// 2. If both are set, it means the interrupt is because of TXE
	if (temp1 && temp2)
	{
	// 3. If TxBusyState is USART_BUSY_IN_TX and TxLen > 0:
		if (pUSARTHandle->pTxBuffer == USART_BUSY_IN_TX && pUSARTHandle->TxLen > 0)
		{
	//    - Check the USART_WordLength (9BIT or 8BIT)
			if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_9BITS)
			{
	//    - Load the DR register with data from pTxBuffer (mask it properly)
				pdata = (uint16_t*)(pUSARTHandle->pTxBuffer);
				pUSARTHandle->pUSARTx->DR = *(pdata) & 0x1FF;
	//    - Handle USART_ParityControl to know whether to increment pTxBuffer by 1 or 2
				if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
				{
					pUSARTHandle->pTxBuffer += 2;
				} else
				{
					pUSARTHandle->pTxBuffer++;
				}
			} else if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_8BITS)
			{
				pUSARTHandle->pUSARTx->DR = *(pUSARTHandle->pTxBuffer) & 0xFF;
				pUSARTHandle->pTxBuffer++;
			}
	//    - Decrement the length (TxLen)
			pUSARTHandle->TxLen-=2;
		}

	// 4. If TxLen reaches 0:
		if (pUSARTHandle->TxLen == 0)
		{
	//    - Clear the TXEIE bit (disable interrupt for TXE flag)
			pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TXEIE);
		}
	}

/************************* Check for RXNE (Read Data Register Not Empty) flag ***********/

	// 1. Check the state of RXNE bit in the SR and RXNEIE bit in CR1
	temp1 = (pUSARTHandle->pUSARTx->CR1 >> USART_CR1_RXNEIE) & 0x1;
	temp2 = (pUSARTHandle->pUSARTx->SR >> USART_SR_RXNE) & 0x1;
	// 2. If both are set, it means the interrupt is because of RXNE
	if (temp1 && temp2)
	{
	// 3. If RxBusyState is USART_BUSY_IN_RX and RxLen > 0:
		if (pUSARTHandle->RxState == USART_BUSY_IN_RX && pUSARTHandle->RxLen > 0)
		{
	//    - Check the USART_WordLength (9BIT or 8BIT)
	//    - Read data from DR register to pRxBuffer (mask it properly based on Parity)
	//    - Handle USART_ParityControl to know whether to increment pRxBuffer by 1 or 2
	//    - Decrement the length (RxLen)
		
			if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_9BITS)
			{
				if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
				{
					*((uint16_t*)(pUSARTHandle->pRxBuffer)) = (pUSARTHandle->pUSARTx->DR & 0x01FF);
					pUSARTHandle->pRxBuffer += 2;
				} else
				{
					(pUSARTHandle->pRxBuffer)++;
				}

			} else if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_8BITS)
			{
				if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
				{
					*(pUSARTHandle->pRxBuffer) = pUSARTHandle->pUSARTx->DR;
					(pUSARTHandle->pRxBuffer)++;
				} else
				{
					*(pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->DR & 0x7F);
					(pUSARTHandle->pRxBuffer)++;
				}
			}
			pUSARTHandle->RxLen--;
		}
	}
	// 4. If RxLen reaches 0:
	//    - Disable the RXNEIE bit
	//    - Reset RxBusyState to USART_READY
	//    - Call the application callback with USART_EVENT_RX_CMPLT
	if (pUSARTHandle->RxLen == 0)
	{
		pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_RXNEIE);
		pUSARTHandle->RxState == USART_READY;
		USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_RX_CMPLT);
	}

/************************* Check for CTS (Clear To Send) flag ***************************/
	// Note: CTS feature is not applicable for UART4 and UART5
	temp1 = ( pUSARTHandle->pUSARTx->SR >> USART_SR_CTS ) & 0x1;
	temp2 = ( pUSARTHandle->pUSARTx->CR3 >> USART_CR3_CTSE ) & 0x1;
	temp3 = ( pUSARTHandle->pUSARTx->CR3 >> USART_CR3_CTSIE ) & 0x1;
	// 1. Check the status of CTS bit in the SR
	// 2. Check the state of CTSE bit in CR3
	// 3. Check the state of CTSIE bit in CR3

	// 4. If all conditions are met:
	//    - Clear the CTS flag in SR
	//    - Call the application callback with USART_EVENT_CTS
	if (temp1 && temp2 && temp3)
	{
		pUSARTHandle->pUSARTx->SR |= (1 << USART_SR_CTS);
		USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_CTS);
	}

/************************* Check for IDLE detection flag ********************************/

	// 1. Check the status of IDLE flag bit in the SR and IDLEIE bit in CR1
	// 2. If both are set:
	//    - Clear the IDLE flag (Refer to the RM to understand the clear sequence!)
	//    - Call the application callback with USART_EVENT_IDLE
	// TODO

/************************* Check for Overrun (ORE) detection flag ***********************/

	// 1. Check the status of ORE flag in the SR and RXNEIE bit in CR1
	// 2. If both are set:
	//    - Need not clear the ORE flag here. Provide an API for the application to clear it.
	//    - Call the application callback with USART_EVENT_ORE
	// TODO

/************************* Check for Error Flags (FE, NE, ORE) **************************/
	// Note: Relevant for multibuffer communication.

	// 1. Check the state of EIE bit in CR3
	// 2. If set, check SR for FE (Framing Error), NE (Noise Error), and ORE flags
	// 3. For each flag that is set, call the callback with:
	//    - USART_ERREVENT_FE
	//    - USART_ERREVENT_NE
	//    - USART_ERREVENT_ORE
	// Note: Refer to RM for software sequence to clear these flags (read SR, then read DR).
	// TODO

}
/**************************************		Init and De_init		**************************************/


void USART_Init(USART_Handle_t *pUSARTHandle)
{
	uint32_t tempreg = 0;

/******************************** Configuration of CR1 ******************************************/

	// 1. Enable the Clock for given USART peripheral
	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

	// 2. Enable USART Tx and/or Rx engines according to the USART_Mode configuration item
	if (pUSARTHandle->USARTConfig.USART_Mode == USART_MODE_ONLY_TX)
	{
		tempreg |= (1 << USART_CR1_TE);
	} else if (pUSARTHandle->USARTConfig.USART_Mode == USART_MODE_ONLY_RX)
	{
		tempreg |= (1 << USART_CR1_RE);
	} else if (pUSARTHandle->USARTConfig.USART_Mode == USART_MODE_TXRX)
	{
		tempreg |= (3 << USART_CR1_RE);
	}

	// 3. Configure the Word length
	tempreg |= (pUSARTHandle->USARTConfig.USART_WordLength << USART_CR1_M);

	// 4. Configure Parity control (None, Even, or Odd)
	if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		tempreg |= (1 << USART_CR1_PCE);
		tempreg &= ~(1 << USART_CR1_PS);
	} else if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_EN_ODD)
	{
		tempreg |= (1 << USART_CR1_PCE);
		tempreg |= (1 << USART_CR1_PS);
	}

	// 5. Program the CR1 register
	pUSARTHandle->pUSARTx->CR1 = tempreg;

/******************************** Configuration of CR2 ******************************************/
	
	tempreg = 0;

	// 1. Configure the number of stop bits inserted during USART frame transmission
	tempreg |= (pUSARTHandle->USARTConfig.USART_NoOfStopBits << USART_CR2_STOP);

	// 2. Program the CR2 register
	pUSARTHandle->pUSARTx->CR2 = tempreg;

/******************************** Configuration of CR3 ******************************************/
	
	tempreg = 0;

	// 1. Configure USART hardware flow control (None, CTS, RTS, or CTS+RTS)
	if (pUSARTHandle->USARTConfig.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
    {
        tempreg |= (1 << USART_CR3_CTSE);
    } 
    else if (pUSARTHandle->USARTConfig.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
    {
        tempreg |= (1 << USART_CR3_RTSE);
    } 
    else if (pUSARTHandle->USARTConfig.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
    {
        tempreg |= (1 << USART_CR3_CTSE) | (1 << USART_CR3_RTSE);
    }

	// 2. Program the CR3 register
	pUSARTHandle->pUSARTx->CR3 = tempreg;

/******************************** Configuration of BRR (Baudrate register) **********************/

	USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USARTConfig.USART_Baud);
}


/**************************************		Data send and receive		**************************************/

void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint16_t *pdata;

	// 1. Loop over until "Len" number of bytes are transferred
	while (Len > 0)
	{
		// 2. Wait until TXE (Transmit Data Register Empty) flag is set in the SR
		while (! USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_TXE_FLAG)) {};

		// 3. Check the USART_WordLength item for 9BIT or 8BIT in a frame
		if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_9BITS)
		{
			// 3a. If 9BIT:
			// - Load the DR with 2 bytes (mask the bits other than first 9 bits)
			// - Check for USART_ParityControl (Disable vs Enable)
			// - If no parity: increment pTxBuffer twice
			// - If parity enabled: increment pTxBuffer once (9th bit is handled by HW)
			pdata = (uint16_t*)pTxBuffer;
			pUSARTHandle->pUSARTx->DR = *(pdata) & 0x1FF;
			if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
			{
				pTxBuffer += 2;
			} else
			{
				//9th bit is parity bit and it is added by hardware, so we send 8bit of data
				//Still takes 2 byte but ignores the second byte - overwrites only the 9th bit with parity of 
				//first byte
				pTxBuffer++;
			}
		} else if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_8BITS)
		{
			// 3b. If 8BIT:
			// - Load the DR with 1 byte (mask to 8 bits)
			// - Increment the buffer address
			pUSARTHandle->pUSARTx->DR = *(pTxBuffer) & 0xFF;
			pTxBuffer++;
		}
		Len--;

	}
	// 4. Wait till TC (Transmission Complete) flag is set in the SR
	while (! USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_TC_FLAG)) {};
}

void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	// 1. Loop over until "Len" number of bytes are transferred
	while (Len > 0)
	{
		// 2. Wait until RXNE (Read Data Register Not Empty) flag is set in the SR
		while (! USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_RXNE_FLAG)) {};

		// 3. Check the USART_WordLength to decide whether we are receiving 9bit or 8bit
		if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_9BITS)
		{
			// 3a. If 9BIT:
			// - Check if USART_ParityControl is used or not
			// - If no parity: read 9 bits (mask with 0x01FF), increment pRxBuffer twice
			// - If parity enabled: read 8 bits (mask with 0xFF), increment pRxBuffer once
			if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
			{
				*((uint16_t*)pRxBuffer) = (pUSARTHandle->pUSARTx->DR & 0x01FF);
				pRxBuffer += 2;
			} else
			{
				pRxBuffer++;
			}

		} else if (pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_8BITS)
		{
			// 3b. If 8BIT:
			// - Check if USART_ParityControl is used or not
			// - If no parity: read 8 bits, increment pRxBuffer
			// - If parity enabled: read 7 bits (mask with 0x7F), increment pRxBuffer
			if (pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
			{
				*(pRxBuffer) = pUSARTHandle->pUSARTx->DR;
				pRxBuffer++;
			} else
			{
				*(pRxBuffer) = (pUSARTHandle->pUSARTx->DR & 0x7F);
				pRxBuffer++;
			}
		}
		Len--;
	}
}

/**************************************		Data send and receive with interrupts		**************************************/

uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t txstate;

	// 1. Get current Tx state from the handle
	txstate = pUSARTHandle->TxState;

	// 2. If txstate is NOT busy in TX:
	// - Save length, buffer address, and set state to BUSY_IN_TX in the handle
	// - Enable interrupt for TXE (Transmit Data Register Empty)
	// - Enable interrupt for TC (Transmission Complete)
	if (txstate != USART_BUSY_IN_TX)
	{
		pUSARTHandle->TxLen = Len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxState = USART_BUSY_IN_TX;
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TXEIE);
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TCIE);
	}

	// 3. Return current txstate
	txstate = pUSARTHandle->TxState;

    return txstate;
}

uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t rxstate;

	// 1. Get current Rx state from the handle
	rxstate = pUSARTHandle->RxState;

	// 2. If rxstate is NOT busy in RX:
	// - Save length, buffer address, and set state to BUSY_IN_RX in the handle
	// - Enable interrupt for RXNE (Read Data Register Not Empty)
	if (rxstate != USART_BUSY_IN_RX)
	{
		pUSARTHandle->RxLen = Len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->TxState = USART_BUSY_IN_RX;
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE);
	}

	// 3. Return current rxstate
	rxstate = pUSARTHandle->RxState;

    return rxstate; 
}



void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{
	uint32_t PCLKx = 0;
	uint32_t usartdiv;
	uint32_t M_part, F_part;
	uint32_t tempreg = 0;
	uint8_t OVER = 0;
	// 1. Get the value of the correct APB bus clock into the variable PCLKx
	// Note: Check which USART/UART peripheral hangs on APB1 and which on APB2
	if (pUSARTx == USART2)
	{
		PCLKx = RCC_GetPCLK1Value();
	} else 
	{
		PCLKx = RCC_GetPCLK2Value();
	}

	// 2. Check for OVER8 configuration bit in CR1
	// Calculate the raw USARTDIV value based on OVER8 (oversampling by 8) or OVER16 (oversampling by 16)
	if (((pUSARTx->CR1 >> USART_CR1_OVER8) & 0x1) == 0 )
	{
		OVER = 16;
	} else
	{
		OVER = 8;
	}

	usartdiv = (25 * PCLKx) / (OVER / 4 * BaudRate);

	// 3. Calculate the Mantissa part (M_part) from usartdiv
	M_part = usartdiv / 100;

	// 4. Place the Mantissa part in the appropriate bit position in tempreg
	tempreg |= M_part << USART_BRR_DIV_MANT;

	// 5. Extract the fraction part (F_part) from usartdiv
	F_part = ( (usartdiv - (M_part * 100)) *  OVER + 50 ) / 100;

	// 6. Calculate the final fractional value depending on OVER8 or OVER16
	// Note: Remember to implement correct rounding logic and mask the result (3 bits for OVER8, 4 bits for OVER16)
	// 7. Place the final fractional part in the appropriate bit position in tempreg
	if (((pUSARTx->CR1 >> USART_CR1_OVER8) & 0x1) == 0 )
	{
		tempreg |= (F_part & 0xF) << USART_BRR_DIV_FRAC;
	} else
	{
		tempreg |= (F_part & 0x7) << USART_BRR_DIV_FRAC;
	}
	// 8. Program the BRR (Baud Rate Register) with tempreg
	pUSARTx->BRR = tempreg;
}
