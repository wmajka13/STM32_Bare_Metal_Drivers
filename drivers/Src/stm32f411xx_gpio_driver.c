/*
 * stm32f411xx_gpio_driver.c
 *
 *  Created on: Jan 9, 2026
 *      Author: Wiktor Majka
 */

#include "stm32f411xx_gpio_driver.h"

/*
 *  Peripheral Clock setup
 */

/***********************************************************
 * @fn			- GPIO_PeriClockControl
 *
 * @brief 		- Enables or disables clock on GPIO's bus
 *
 * @param[pGPIOx]		- Pointer to structure defining GPIO's registers (it is also a base address of GPIO)
 * @param[EnorDi]		- ENABLE or DISABLE macros
 *
 * @return		- None
 *
 * @note		- None
 *
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{

		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		} else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();
		} else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		} else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		} else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		} else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();
		}

	} else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();
		} else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();
		} else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();
		} else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();
		} else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();
		} else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_DI();
		}
	}
}

/*
 * 	Init and De_init
 */

/***********************************************************
 * @fn			- GPIO_Init
 *
 * @brief 		- Initialazing GPIO
 *
 * @param[pGPIOHandle]		- Pointer to structure holding GPIO settings and base address
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0; //Temporary register
	//1. Configure the mode of GPIO pin
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		// Non-interrupt part
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) ); //clearing
		pGPIOHandle->pGPIOx->MODER |= temp; //setting
	}else
	{
		if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			//1. Configure the FTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//Clear the corresponding RTSR bit
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			//1. Configure the RTSR
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//Clear the corresponding RTSR bit
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			//1. Configure the RFTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		//2. Configure the GPIO port Selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4; // obliczamy w którym z rejestrów EXTICR będzie nasz pin
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4; // Obliczamy w której częsci rejestru mamy zacząć pisać
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx); // przelicza port na kod który się zapisuje w EXTICR A = 0001
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] = portcode << (temp2 * 4);

		//3. Enable the EXTI interrrupt delivery using IMR (Interrupt Mask Register)
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}

	temp = 0;
	//2. Configure the speed of GPIO pin
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) ); //clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;
	//3. Configure the PUPD settings
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) ); //clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;
	//4. Configure the OTYPE
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	temp = 0;
	//5. Configure the ALT functionality
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		//If mode is set to ALTFN then set this registers.
		uint8_t temp1, temp2;
		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;		//This temp1 shows which AFRx register to use - lower temp1=0, higher temp1=1
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;		//this temp2 calculates the number of bit in register, for ex. 15pin and 7pin should be wrriten at pos=28

		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2) );
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2) );
	}
}

/***********************************************************
 * @fn			- GPIO_DeInit
 *
 * @brief 		- Deinitializing GPIO,
 *
 * @param[pGPIOx]		- Pointer to structure holding GPIO settings and base address
 *
 * @return		- none
 *
 * @note		- Setting registers back to theirs original state, done using RCC_AHB1RSTR (example)
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();
	} else if(pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();
	} else if(pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();
	} else if(pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();
	} else if(pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();
	} else if(pGPIOx == GPIOH)
	{
		GPIOH_REG_RESET();
	}
}


/*
 * 	Data read and write
 */

/***********************************************************
 * @fn			- GPIO_ReadFromInputPin
 *
 * @brief 		- Reading input from a pin
 *
 * @param[pGPIOx]		- Pointer to GPIO base address
 * @param[PinNumber]		- Number of pin to read from
 *
 * @return		- 0 or 1
 *
 * @note		- none
 *
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t pinValue;

	pinValue = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001 );

	return pinValue;
}

/***********************************************************
 * @fn			- GPIO_ReadFromInputPort
 *
 * @brief 		- Reading pin values of whole port
 *
 * @param[pGPIOx]		- Pointer to GPIO base address
 *
 * @return		- Value of pins in port - 0 or 1
 *
 * @note		- none
 *
 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t portValue;

	portValue = (uint16_t)(pGPIOx->IDR);

	return portValue;
}

/***********************************************************
 * @fn			- GPIO_WriteToOutputPin
 *
 * @brief 		- Writing value to a pin
 *
 * @param[pGPIOx]		- Pointer to base address
 * @param[PinNumber]		- Number of pin
 * @param[Value]		- Value to be written in a pin - macors GPIO_PIN_SET/GPIO_PIN_RESET
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		pGPIOx->ODR |= (0x1 << PinNumber); 			 //setting
	}else
	{
		pGPIOx->ODR &= ~(0x1 << PinNumber);		 	 //clearing
	}
}

/***********************************************************
 * @fn			- GPIO_WriteToOutputPorta
 *
 * @brief 		- Writing value to whole port
 *
 * @param[pGPIOx]		- Pointer to base address
 * @param[Value]		- Value to be written in a port
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void GPIO_WriteToOutputPorta(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx->ODR = Value;
}

/***********************************************************
 * @fn			- GPIO_ToggleOutputPin
 *
 * @brief 		- Toggles value of a pin
 *
 * @param[pGPIOx]		- Pointer to base address
 * @param[PinNumber]		- Number of pin
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= ( 1 << PinNumber );
}

/*
 * 	IRQ Configuration and ISR handling
 */

/***********************************************************
 * @fn			- GPIO_IRQInterruptConfig
 *
 * @brief 		- Enables/Disables ability to use Interrupt from GPIO
 *
 * @param[IRQNumber]		- Number of interrupt
 * @param[IRQPriority]		- Priority of interrupt
 * @param[EnorDI]		- Enable or Disable macro
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI)
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
			*NVIC_ISER2 |= ( 1 << (IRQNumber % 62) );
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
			*NVIC_ICER2 |= ( 1 << (IRQNumber % 62) );
		}
	}
}


/***********************************************************
 * @fn			- GPIO_IRQPriorityConfig
 *
 * @brief 		- , allows to set a interrupt priority
 *
 * @param[IRQNumber]		- Number of interrupt
 * @param[IRQPriority]		- Priority of interrupt
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
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
 * @param[PinNumber]		- number of pin
 *
 * @return		- none
 *
 * @note		- none
 *
 */
void GPIO_IRQHandling(uint8_t PinNumber)
{
	//Clear the exti PR register corresponding to the pin number
	if(EXTI->PR & ( 1 << PinNumber ))
	{
		//clear, here it is writing 1 to the register.
		EXTI->PR |= ( 1 << PinNumber );
	}
}


