#include "stm32f411xx_rcc_driver.h"

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint8_t APB_PreScalers[4] = {2, 4, 8, 16};


/**************************************		Calculating clock value		**************************************/

/**
 * Calculates PLLCK speed TODO
 */
uint32_t RCC_GetPullOutputClock(void)
{
	return 0;
}
//returns frequency of PLCK in Mhz

/**
 * Calculates the PCLK1 value
 */
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
		apb1p = APB_PreScalers[temp-4]; //We have an array that holds possible prescalers: for prescaler=2 code is 8, 4->9, 8->10
	}

	pclk1 = (SystemClk / ahbp) / apb1p;

	return pclk1;
}

/**
 * Calculates the PCLK1 value
 */
uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t pclk2, SystemClk;
	uint8_t clksrc, temp, ahbp, apb2p;

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

	temp = ( (RCC->CFGR >> 13) & 0x7); //value of ahb prescaler in code
	if (temp < 4)
	{
		apb2p = 1;
	} else
	{
		apb2p = APB_PreScalers[temp-4]; //We have an array that holds possible prescalers: for prescaler=2 code is 8, 4->9, 8->10
	}

	pclk2 = (SystemClk / ahbp) / apb2p;

	return pclk2;
}



