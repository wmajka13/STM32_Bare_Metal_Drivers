/*
 * stm32f411xx.h
 *
 *  Created on: Jan 7, 2026
 *      Author: Wiktor Majka
 */

#ifndef INC_STM32F411XX_H_
#define INC_STM32F411XX_H_

#include <stdint.h>									/* Included for uint32_t */
#define __vo volatile  								/* Shorter name for volatile  */


/************************************* Processor Specific Details **************************/
/*
 * ARM Cortex M4 PRocessor NVIC ISERx register addresses
 */
#define NVIC_ISER0				( (__vo uint32_t*)0xE000E100 ) //Zapisujemy od razu z typami, zeby potem łatwiej uzywać
#define NVIC_ISER1				( (__vo uint32_t*)0xE000E104 )
#define NVIC_ISER2				( (__vo uint32_t*)0xE000E108 )
#define NVIC_ISER3				( (__vo uint32_t*)0xE000E10C )

#define NVIC_ICER0				( (__vo uint32_t*)0xE000E180 )
#define NVIC_ICER1				( (__vo uint32_t*)0xE000E184 )
#define NVIC_ICER2				( (__vo uint32_t*)0xE000E188 )
#define NVIC_ICER3				( (__vo uint32_t*)0xE000E18C )


/*
 * ARM Cortex M4 Processor Priority Registers adresses
 */
#define NVIC_PR_BASE_ADDR		( (__vo uint32_t*)0xE000E400 )


/*
 *  ARM Cortex M4 Processor number of priority bits implemented in priority register
 */
#define NO_PR_BITS_IMPLEMENTED			4


/*
 * base addresses of flash and sram
 */

#define FLASH_BASEADDR			0x08000000U 		/* First address of flash memory */
#define SRAM1_BASEADDR			0x20000000U			/* First address of SRAM1 memory */
#define ROM						0x1FFF0000U			/* First address of ROM memory */
#define SRAM					SRAM1_BASEADDR		/* First address of SRAM memory */


/*
 * AHBx and APBx Bus Peripherial base addresses
 */

#define PERIPH_BASE				0x40000000U			/* Peripherials Bus Base Address */
#define APB1PERIPH_BASE			PERIPH_BASE
#define APB2PERIPH_BASE			0x40010000U
#define AHB1PERIPH_BASE			0x40020000U
#define AHB2PERIPH_BASE			0x50000000U

/*
 * Base addresses of peripherials on AHB1 Bus
 */

#define GPIOA_BASEADDR			(AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASEADDR			(AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASEADDR			(AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASEADDR			(AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASEADDR			(AHB1PERIPH_BASE + 0x1000)
#define GPIOH_BASEADDR			(AHB1PERIPH_BASE + 0x1C00)

#define RCC_BASEADDR			(AHB1PERIPH_BASE + 0x3800)

/*
 * 	Base addresses of peripherials on APB1 Bus (only I2C, SPI, USART, EXTI, SYSCFG)
 */

#define SPI2_BASEADDR			(APB1PERIPH_BASE + 0x3400)
#define SPI3_BASEADDR			(APB1PERIPH_BASE + 0x3C00)
#define USART2_BASEADDR			(APB1PERIPH_BASE + 0x4400)
#define I2C1_BASEADDR			(APB1PERIPH_BASE + 0x5400)
#define I2C2_BASEADDR			(APB1PERIPH_BASE + 0x5800)
#define I2C3_BASEADDR			(APB1PERIPH_BASE + 0x5C00)


/*
 * 	Base addresses of peripherials on APB2	 Bus (only I2C, SPI, USART, EXTI, SYSCFG)
 */

#define USART1_BASEADDR			(APB2PERIPH_BASE + 0x1000)
#define USART6_BASEADDR			(APB2PERIPH_BASE + 0x1400)
#define SPI1_BASEADDR			(APB2PERIPH_BASE + 0x3000)
#define SPI4_BASEADDR			(APB2PERIPH_BASE + 0x3400)
#define SYSCFG_BASEADDR			(APB2PERIPH_BASE + 0x3800)
#define EXTI_BASEADDR			(APB2PERIPH_BASE + 0x3C00)
#define SPI5_BASEADDR			(APB2PERIPH_BASE + 0x5000)


/************************************* Peripherial register definiton structure **************************/

//GPIO REGISTERS
typedef struct
{
	__vo uint32_t MODER;					/* GPIO port mode register */
	__vo uint32_t OTYPER;					/* GPIO port output type register */
	__vo uint32_t OSPEEDR;					/* GPIO port output speed register */
	__vo uint32_t PUPDR;					/* GPIO port pull-up/pull-down register */
	__vo uint32_t IDR;						/* GPIO port input data register */
	__vo uint32_t ODR;						/* GPIO port output data register */
	__vo uint32_t BSRR;						/* GPIO port bit set/reset register */
	__vo uint32_t LCKR;						/* GPIO port configuration lock register */
	__vo uint32_t AFR[2];					/* AFR[0]: GPIO alternate function low register AFR[1]: GPIO alternate function high register */

}GPIO_RegDef_t;

//RCC REGISTERS
typedef struct
{
	__vo uint32_t CR;							/*  RCC clock control register   										Address offset: 0x00 */
	__vo uint32_t PLLCFGR;						/* RCC PLL configuration register   									Address offset: 0x04*/
	__vo uint32_t CFGR;							/* RCC clock configuration register    									Address offset: 0x08*/
	__vo uint32_t CIR;							/* RCC clock interrupt register    										Address offset: 0x0C*/
	__vo uint32_t AHB1RSTR;						/* RCC AHB1 peripheral reset register    								Address offset: 0x10*/
	__vo uint32_t AHB2RSTR;						/* RCC AHB2 peripheral reset register    								Address offset: 0x14*/
	__vo uint32_t Reserved0[2];					/*			RESERVED					0x18-0x1C	*/
	__vo uint32_t APB1RSTR;						/* RCC APB2 peripheral reset register    								Address offset: 0x20*/
	__vo uint32_t APB2RSTR;						/* RCC APB2 peripheral reset register					    			Address offset: 0x24*/
	__vo uint32_t Reserved1[2];					/*			RESERVED		 			0x28-0x2C	*/
	__vo uint32_t AHB1ENR;						/* RCC AHB1 peripheral clock enable register    						Address offset: 0x30*/
	__vo uint32_t AHB2ENR;						/* RCC AHB2 peripheral clock enable register    						Address offset: 0x34*/
	__vo uint32_t Reserved2[2];					/*			RESERVED					0x38-0x3C	*/
	__vo uint32_t APB1ENR;						/* RCC APB1 peripheral clock enable register    						Address offset: 0x40*/
	__vo uint32_t APB2ENR;						/* RCC APB2 peripheral clock enable register     						Address offset: 0x44*/
	__vo uint32_t Reserved3[2];					/*			RESERVED					0x48-0x4C	*/
	__vo uint32_t AHB1LPENR;					/* RCC AHB1 peripheral clock enable in low power mode register    		Address offset: 0x50*/
	__vo uint32_t AHB2LPENR;					/* RCC AHB2 peripheral clock enable in low power mode register    		Address offset: 0x54*/
	__vo uint32_t Reserved4[2];					/*			RESERVED					0x58-0x5C	*/
	__vo uint32_t APB1LPENR;					/* RCC APB1 peripheral clock enable in low power mode register    		Address offset: 0x60*/
	__vo uint32_t APB2LPENR;					/* RCC APB2 peripheral clock enabled in low power mode register    		Address offset: 0x64*/
	__vo uint32_t Reserved5[2];					/*			RESERVED					0x68-0x6C	*/
	__vo uint32_t BDCR;							/* RCC Backup domain control register    								Address offset: 0x70*/
	__vo uint32_t CSR;							/* RCC spread spectrum clock generation register    					Address offset: 0x74*/
	__vo uint32_t Reserved6[2];					/*			RESERVED					0x78-0x7C	*/
	__vo uint32_t SSCGR;						/* RCC spread spectrum clock generation register     					Address offset: 0x80*/
	__vo uint32_t PLLI2SCFGR;					/* RCC PLLI2S configuration register    								Address offset: 0x84*/
	__vo uint32_t Reserved7;					/*			RESERVED					0x88	*/
	__vo uint32_t DCKCFGR;						/* RCC Dedicated Clocks Configuration Register    						Address offset: 0x8C*/

}RCC_RegDef_t;


//EXTI REGISTERS
typedef struct
{
	__vo uint32_t IMR;							/* EXTI */
	__vo uint32_t EMR;							/* EXTI */
	__vo uint32_t RTSR;							/* EXTI */
	__vo uint32_t FTSR;							/* EXTI */
	__vo uint32_t SWIER;						/* EXTI */
	__vo uint32_t PR;							/* EXTI */

}EXTI_RegDef_t;


//SYSCFG REGISTERS
typedef struct
{
	__vo uint32_t MEMRMP;						/* SYSCFG memory remap register 						Address offset: 0x00 */
	__vo uint32_t PMC;							/* SYSCFG peripheral mode configuration register 		Address offset: 0x04 */
	__vo uint32_t EXTICR[4];					/* SYSCFG external interrupt configuration registers 	Address offset: 0x08-0x14 */
	uint32_t RESERVED[2];						/*														Address offset: 0x18-0x1C */
	__vo uint32_t CMPCR;						/* Compensation cell control register (SYSCFG_CMPCR) 	Address offset: 0x20 */

}SYSCFG_RegDef_t;

/*
 * Peripheral definitions (Peripheral base addresses typecasted to xxx_RegDef_t)
 */

#define GPIOA			((GPIO_RegDef_t*) GPIOA_BASEADDR)
#define GPIOB			((GPIO_RegDef_t*) GPIOB_BASEADDR)
#define GPIOC			((GPIO_RegDef_t*) GPIOC_BASEADDR)
#define GPIOD			((GPIO_RegDef_t*) GPIOD_BASEADDR)
#define GPIOE			((GPIO_RegDef_t*) GPIOE_BASEADDR)
#define GPIOH			((GPIO_RegDef_t*) GPIOH_BASEADDR)

#define RCC 			((RCC_RegDef_t*) RCC_BASEADDR)
#define EXTI 			((EXTI_RegDef_t*) EXTI_BASEADDR)
#define SYSCFG			((SYSCFG_RegDef_t*) SYSCFG_BASEADDR)

/*************************** PCLOCK ENABLE ***************************/


/*
 * 	Clock Enable Macros for GPIOx peripherals
 */
// GPIOA_PERI_CLOCK_ENABLE
#define GPIOA_PCLK_EN()		( RCC->AHB1ENR |= ( 1 << 0 ) )
#define GPIOB_PCLK_EN()		( RCC->AHB1ENR |= ( 1 << 1 ) )
#define GPIOC_PCLK_EN()		( RCC->AHB1ENR |= ( 1 << 2 ) )
#define GPIOD_PCLK_EN()		( RCC->AHB1ENR |= ( 1 << 3 ) )
#define GPIOE_PCLK_EN()		( RCC->AHB1ENR |= ( 1 << 4 ) )
#define GPIOH_PCLK_EN()		( RCC->AHB1ENR |= ( 1 << 7 ) )

/*
 * 	Clock Enable Macros for I2Cx peripherals
 */
#define I2C1_PCLK_EN()		( RCC->APB1ENR |= ( 1 << 21 ) )
#define I2C2_PCLK_EN()		( RCC->APB1ENR |= ( 1 << 22 ) )
#define I2C3_PCLK_EN()		( RCC->APB1ENR |= ( 1 << 23 ) )

/*
 * 	Clock Enable Macros for SPIx peripherals
 */
#define SPI1_PCLK_EN()		( RCC->APB2ENR |= ( 1 << 12 ) )
#define SPI2_PCLK_EN()		( RCC->APB1ENR |= ( 1 << 14 ) )
#define SPI3_PCLK_EN()		( RCC->APB1ENR |= ( 1 << 15 ) )
#define SPI4_PCLK_EN()		( RCC->APB2ENR |= ( 1 << 13 ) )
#define SPI5_PCLK_EN()		( RCC->APB2ENR |= ( 1 << 20 ) )

/*
 * 	Clock Enable Macros for USARTx peripherals
 */
#define USART1_PCLK_EN()		( RCC->APB2ENR |= ( 1 << 4 ) )
#define USART6_PCLK_EN()		( RCC->APB2ENR |= ( 1 << 5 ) )

/*
 * 	Clock Enable Macros for SYSCFG peripherals
 */
#define SYSCFG_PCLK_EN()		( RCC->APB2ENR |= ( 1 << 14 ) )


/*************************** PCLOCK DISABLE ***************************/

/*
 * 	Clock Disable Macros for GPIOx peripherals
 */
#define GPIOA_PCLK_DI()		( RCC->AHB1ENR &= ~( 1 << 0 ) )
#define GPIOB_PCLK_DI()		( RCC->AHB1ENR &= ~( 1 << 1 ) )
#define GPIOC_PCLK_DI()		( RCC->AHB1ENR &= ~( 1 << 2 ) )
#define GPIOD_PCLK_DI()		( RCC->AHB1ENR &= ~( 1 << 3 ) )
#define GPIOE_PCLK_DI()		( RCC->AHB1ENR &= ~( 1 << 4 ) )
#define GPIOH_PCLK_DI()		( RCC->AHB1ENR &= ~( 1 << 7 ) )

/*
 * 	Clock Disable Macros for I2Cx peripherals
 */
#define I2C1_PCLK_DI()		( RCC->APB1ENR &= ~( 1 << 21 ) )
#define I2C2_PCLK_DI()		( RCC->APB1ENR &= ~( 1 << 22 ) )
#define I2C3_PCLK_DI()		( RCC->APB1ENR &= ~( 1 << 23 ) )

/*
 * 	Clock Disable Macros for SPIx peripherals
 */
#define SPI1_PCLK_DI()		( RCC->APB2ENR &= ~( 1 << 12 ) )
#define SPI2_PCLK_DI()		( RCC->APB1ENR &= ~( 1 << 14 ) )
#define SPI3_PCLK_DI()		( RCC->APB1ENR &= ~( 1 << 15 ) )
#define SPI4_PCLK_DI()		( RCC->APB2ENR &= ~( 1 << 13 ) )
#define SPI5_PCLK_DI()		( RCC->APB2ENR &= ~( 1 << 20 ) )

/*
 * 	Clock Disable Macros for USARTx peripherals
 */
#define USART1_PCLK_DI()		( RCC->APB2ENR &= ~( 1 << 4 ) )
#define USART6_PCLK_DI()		( RCC->APB2ENR &= ~( 1 << 5 ) )

/*
 * 	Clock Disable Macros for SYSCFG peripherals
 */
#define SYSCFG_PCLK_DI()		( RCC->APB2ENR &= ~( 1 << 14 ) )


/*  Bit in APB1RSTR should be set and reseted
 * 	Macros to reset GPIOx peripherals
 */
#define GPIOA_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 0 )); (RCC->APB1RSTR &= ~(1 << 0 )); }while(0)
#define GPIOB_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 1 )); (RCC->APB1RSTR &= ~(1 << 1 )); }while(0)
#define GPIOC_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 2 )); (RCC->APB1RSTR &= ~(1 << 2 )); }while(0)
#define GPIOD_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 3 )); (RCC->APB1RSTR &= ~(1 << 3 )); }while(0)
#define GPIOE_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 4 )); (RCC->APB1RSTR &= ~(1 << 4 )); }while(0)
#define GPIOH_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 7 )); (RCC->APB1RSTR &= ~(1 << 7 )); }while(0)


/*
 * Returns port code for given GPIOx base address
 */
#define GPIO_BASEADDR_TO_CODE(x) 	( (x == GPIOA) ? 0 :\
									  (x == GPIOB) ? 1 :\
									  (x == GPIOC) ? 2 :\
									  (x == GPIOD) ? 3 :\
									  (x == GPIOE) ? 4 :\
									  (x == GPIOH) ? 7 :0xFF )

/*
 * IRQ (Interrupt Request) Numbers of STM32F411RE MCU
 */
#define IRQ_NO_EXTI0			6
#define IRQ_NO_EXTI1			7
#define IRQ_NO_EXTI2			8
#define IRQ_NO_EXTI3			9
#define IRQ_NO_EXTI4			10
#define IRQ_NO_EXTI9_5			23
#define IRQ_NO_EXTI15_10		40


/*
 * IRQ (Interrupt Request) Priorities
 */
#define NVIC_IRQ_PRI0			0
#define NVIC_IRQ_PRI1			1
#define NVIC_IRQ_PRI2			2
#define NVIC_IRQ_PRI3			3
#define NVIC_IRQ_PRI4			4
#define NVIC_IRQ_PRI5			5
#define NVIC_IRQ_PRI6			6
#define NVIC_IRQ_PRI7			7
#define NVIC_IRQ_PRI8			8
#define NVIC_IRQ_PRI9			9
#define NVIC_IRQ_PRI10			10
#define NVIC_IRQ_PRI11			11
#define NVIC_IRQ_PRI12			12
#define NVIC_IRQ_PRI13			13
#define NVIC_IRQ_PRI14			14
#define NVIC_IRQ_PRI15			15




/*
 *  Generic macros
 */
#define ENABLE 				1
#define DISABLE 			0
#define SET 				ENABLE
#define RESET 				DISABLE
#define GPIO_PIN_SET    	SET
#define GPIO_PIN_RESET  	RESET


/*
 * 	Includes
 */
#include "stm32f411xx_gpio_driver.h"

#endif /* INC_STM32F411XX_H_ */
