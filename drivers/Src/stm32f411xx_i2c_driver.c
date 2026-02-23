/*
 * stm32f411xx_i2c_driver.c
 *
 *  Created on: Feb 18, 2026
 *      Author: Wiktor Majka
 */


#include "stm32f411_i2c_driver.h"

#define I2C_READ			1
#define I2C_WRITE			0

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint8_t APB1_PreScaler[4] = {2, 4, 8, 16};



static inline void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static inline void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr, uint8_t ReadorWrite);
static inline void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static inline void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);


static inline void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr, uint8_t ReadorWrite)
{
	SlaveAddr = (SlaveAddr << 1);
	if (ReadorWrite == I2C_WRITE)
	{
		SlaveAddr &= ~(1);		//write - last bit=0
	} else
	{
		SlaveAddr |= 1;  	//read - last bit=1
	}

	pI2Cx->DR = SlaveAddr;
}

static inline void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummyread;
	//check for device mode
	if (pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL ))
	{
		//master
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if (pI2CHandle->RxSize == 1)
			{
				//first disable the ACK
				I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

				//clear the addr flag (read sr1, sr2)
				dummyread = pI2CHandle->pI2Cx->SR1;
				dummyread = pI2CHandle->pI2Cx->SR2;
				(void)dummyread;
			}
		} else
		{
			dummyread = pI2CHandle->pI2Cx->SR1;
			dummyread = pI2CHandle->pI2Cx->SR2;
			(void)dummyread;
		}

	} else
	{
		//slave
		dummyread = pI2CHandle->pI2Cx->SR1;
		dummyread = pI2CHandle->pI2Cx->SR2;
		(void)dummyread;

	}
}

static inline void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx) { pI2Cx->CR1 |= ( 1 << I2C_CR1_START ); }
static inline void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx) { pI2Cx->CR1 |= ( 1 << I2C_CR1_STOP ); }


/*
 *  Peripheral Clock setup
 */


void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDI)
{
	if (EnorDI == ENABLE)
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		} else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		} else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	} else
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		} else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		} else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}


/*
 * 	Init and De_init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t tempreg = 0;
	//1. Configure the Mode (standard or fast )

	//2.1 Configure the speed of the serial clock (SCL), configuring the freq in cr2
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value() / 1000000U;
	pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F); //masking because we only want to se first 5bits

	//2.2 Configure the CCR
	uint16_t ccr_value = 0;
	tempreg = 0;

	//setting the F/S bit and CCRbits in CCR
	if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{

		ccr_value = RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		tempreg |= (ccr_value & 0xFFF);
	} else
	{
		tempreg |= (1 << I2C_CCR_FS);
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY);
		if (pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
		{
			ccr_value = RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		} else
		{
			ccr_value = RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}
		tempreg |= (ccr_value & 0xFFF);
	}

	pI2CHandle->pI2Cx->CCR = tempreg;


	//3. Configure the device address (Applicable when device is slave)
	tempreg = 0;
	tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
	tempreg |= ( 1 << 14 ); //manual says that this bit should always be kept high by software
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	//4. Enable the Acking
	tempreg = 0;
	tempreg |= pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	//5. Configure the rise time for I2C pins (TRISE register)
	tempreg = 0;
	if (pI2CHandle->I2C_Config.I2C_FMDutyCycle <= I2C_SCL_SPEED_SM)
	{
		tempreg = (I2C_TRISE_MAX_SM * RCC_GetPCLK1Value() * 1000) + 1;  //mult by 1000 because PCLK in MHz, TRISE
	} else
	{
		tempreg = (I2C_TRISE_MAX_FM * RCC_GetPCLK1Value() * 1000) + 1;
	}
	pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);

}


void I2C_DeInit(I2C_RegDef_t *pI2Cx) /* Setting registers back to theirs original state, done using RCC_AHB1RSTR (example)*/
{
	if(pI2Cx == I2C1)
	{
		I2C1_REG_RESET();
	} else if(pI2Cx == I2C2)
	{
		I2C2_REG_RESET();
	} else if(pI2Cx == I2C3)
	{
		I2C3_REG_RESET();
	}
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr)
{
	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the SB flag in the SR1
	// Note: Until SB is cleared SCL will be stretched (pulled to LOW)

	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB) ); //we chceck and for SB and read the SR1 simultaneously

	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits )
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, SlaveAddr, I2C_WRITE);

	//4. Confirm that address phase is completed by checking the ADDR flag in the SR1
	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR) );

	//5. clear the ADDR flag according to its software sequence
	// Note: Until ADDR is cleared SCL will be stretched (pulled to LOW)
	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

	//6. send the data until Len becomes 0
	while (Len > 0)
	{
		while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TxE) ) //waits until TxE=1
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	//7. when Len becomes zero wait for TXE=1 and BTF=1 before generating the STOP condition
	// Note: TXE=1 , BTF=1 , means that both SR and DR are empty and next transmission should begin
	// when BTF=1 SCL will be stretched (pulled to LOW)
	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TxE) );
	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF) );

	//8. Generate STOP condition and master need not to wait for the completion of stop condition.
	// Note: generating STOP, automatically clears the BTF
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}


void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr)
{
    //1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    //2. confirm that start generation is completed by checking the SB flag in the SR1
    //   Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB) );


    //3. Send the address of the slave with r/nw bit set to R(1) (total 8 bits )
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, SlaveAddr, I2C_READ);

    //4. wait until address phase is completed by checking the ADDR flag in the SR1
	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR) );

    //procedure to read only 1 byte from slave
    if(Len == 1)
    {
        //Disable Acking
    	I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

    	//generate STOP condition
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

        //clear the ADDR flag
    	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

        //wait until  RXNE becomes 1
    	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RxNE) );

        //read data in to buffer
    	*pRxBuffer = pI2CHandle->pI2Cx->DR;
        return;
    }

    //procedure to read data from slave when Len > 1
    if(Len > 1)
    {
        //clear the ADDR flag
    	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

        //read the data until Len becomes zero
        for ( uint32_t i = Len ; i > 0 ; i--)
        {
            //wait until RXNE becomes 1
        	while ( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RxNE) );

            if(i == 2) //if last 2 bytes are remaining
            {
                //clear the ack bit
            	I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
                //generate STOP condition
            	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
            }

            //read the data from data register in to buffer
            *pRxBuffer = pI2CHandle->pI2Cx->DR;
            //increment the buffer address
            pRxBuffer++;
        }
    }

    if (pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
    {
        I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
    }
}



/*
 * 	IRQ Configuration and ISR handling
 */

/***********************************************************
 * @fn			- I2C_IRQInterruptConfig
 *
 * @brief 		- Enables/Disables ability to use Interrupt from I2C
 *
 * @param[IRQNumber]		- Number of interrupt
 * @param[EnorDI]			- Enable or Disable macro
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI)
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. finding out IPR reg
	uint32_t iprx = IRQNumber / 4;
	uint8_t iprx_byte = IRQNumber % 4;

	uint8_t shift_amount = ( 8 * iprx_byte ) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= ( IRQPriority << shift_amount );
}


/*
 * 	Other peripheral control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	} else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}

}

uint32_t RCC_GetPullOutputClock(void)
{
	return 0;
}


//returns frequency of PLCK in Mhz
uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t pclk1, SystemClk;
	uint8_t clksrc, temp, ahbp, apb1p;

	clksrc = ( (RCC->CFGR >> 2) & 0x3); //moves bit 3 and 2 at the beggings and masks

	if (clksrc == 0)
	{
		//sysclk = HSI
		SystemClk = 16e6; //16Mhz

	} else if (clksrc == 1)
	{
		//HSE
		SystemClk = 8e6; //8Mhz
	} else if (clksrc == 2)
	{
		//PLL - another func to calculate this needed - for now not implementend TODO
		SystemClk = RCC_GetPullOutputClock();
	}

	temp = ( (RCC->CFGR >> 4) & 0xF); //value of ahb prescaler in code
	if (temp < 8)
	{
		ahbp = 1;
	} else
	{
		ahbp = AHB_PreScaler[temp-8]; //We have an array that holds possible prescalers: for prescaler=2 code is 8, 4->9, 8->10
	}

	temp = ( (RCC->CFGR >> 10) & 0x7); //value of ahb prescaler in code
	if (temp < 4)
	{
		apb1p = 1;
	} else
	{
		apb1p = APB1_PreScaler[temp-4]; //We have an array that holds possible prescalers: for prescaler=2 code is 8, 4->9, 8->10
	}

	pclk1 = (SystemClk / ahbp) / apb1p;

	return pclk1;
}


uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if (EnOrDi == I2C_ACK_ENABLE)
	{
		pI2Cx->CR1 |= ( 1 << I2C_CR1_ACK );
	} else
	{
		pI2Cx->CR1 &= ~( 1 << I2C_CR1_ACK );
	}
}


uint8_t  I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;

}


uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);
	}

	return busystate;
}


//TODO
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//Interrupt handling for both master and slave mode of a device

	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITEVTEN ); //checking if ITEVENT is set
	temp1 = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITBUFEN );



	//1. Handle For interrupt generated by SB event
	//	Note : SB flag is only applicable in Master mode
	temp3 = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_SB );
	if (temp1 && temp3)
	{
		//SB flag is set - interrupt generated becasue of SB event
		//this block will not be executed in slave mode because slave sb is always zero
		//here the next step should be executed = addres phase.
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, I2C_WRITE);
		} else if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, I2C_READ);
		}
	}

	//2. Handle For interrupt generated by ADDR event
	//Note : When master mode : Address is sent
	//		 When Slave mode   : Address matched with own address
	temp3 = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_ADDR );
	if (temp1 && temp3)
	{
		//ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle->pI2Cx);
	}

	//3. Handle For interrupt generated by BTF(Byte Transfer Finished) event
	temp3 = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_BTF );
	if (temp1 && temp3)
	{
		//BTF flag is set - finishing the communication
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			//make sure that the txe is also set
			if (pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_TxE ))
			{
				// 	TxE=1, BTF=1
				if (pI2CHandle->TxLen == 0)
				{

					// Generate stop cond
					if (pI2CHandle->Sr == I2C_DISABLE_SR) //Reapted start disabled
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

					// Reset all the member elements of structure
					I2C_CloseSendData(pI2CHandle); //TODO

					// Notify the application about transmission
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT); //TODO
				}
			}
		} else if (pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_RxNE ))
		{
			; //nothing to do when it is busy in RxNE
		}
	}

	//4. Handle For interrupt generated by STOPF event
	// Note : Stop detection flag is applicable only slave mode . For master this flag will never be set
	temp3 = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_STOPF );
	if (temp1 && temp3)
	{
		//STOPF flag is set
		//Clear the STOPF - read SR1, write to CR1
		pI2CHandle->pI2Cx->CR1 |= 0x0000;
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP); //TODO
	}

	//5. Handle For interrupt generated by TXE event
	temp3 = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_TxE );
	if (temp1 && temp2 && temp3)
	{
		if (pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL )) // checking whether the device is a master!
		{
			//TxE flag is set
			// Here should be a transmission, we transmit when the i2c is busy in tx
			if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				if (pI2CHandle->TxLen > 0)
				{
					pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);
					pI2CHandle->TxLen--;
					pI2CHandle->pTxBuffer++;

				}
			}
		}
	}

	//6. Handle For interrupt generated by RXNE event
	temp3 = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_RxNE );
	if (temp1 && temp2 && temp3)
	{
		//RxNE flag is set
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			//data reception
			if (pI2CHandle->RxSize == 1)
			{
				*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
				pI2CHandle->RxLen--;
			}

			if (pI2CHandle->RxSize > 1)
			{
				if (pI2CHandle->RxLen == 2)
				{
					//clear the acl
					I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
				}

				//Read the DR
				*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
				pI2CHandle->RxLen--;
				pI2CHandle->pRxBuffer++;
			}

		}	if (pI2CHandle->RxLen == 0) {
			//close the i2c data reception and notify the app
			if (pI2CHandle->Sr == I2C_DISABLE_SR) //Reapted start disabled
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

			//TODO
			I2C_CloseReceiveData(pI2CHandle); //TODO

		}
	}
}




void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

}

