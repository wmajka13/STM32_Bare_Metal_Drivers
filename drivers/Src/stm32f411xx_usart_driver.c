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

	// Implement the code to configure the baud rate
	// We will cover this in the lecture. No action required here 

}