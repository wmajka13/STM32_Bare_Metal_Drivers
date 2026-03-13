/*
 * stm32f411_i2c_driver.h
 *
 *  Created on: Feb 18, 2026
 *      Author: Wiktor Majka
 */

#ifndef INC_STM32F411XX_I2C_DRIVER_H_
#define INC_STM32F411XX_I2C_DRIVER_H_

#include "stm32f411xx.h"

/*
 * 	Configuration structure for I2C
 */

typedef struct
{
	uint32_t I2C_SCLSpeed;
	uint8_t I2C_DeviceAddress;
	uint8_t I2C_ACKControl;
	uint16_t I2C_FMDutyCycle;

}I2C_Config_t;


/*
 * 	Handle structure for I2Cx peripheral
 */
typedef struct
{
	I2C_RegDef_t 		*pI2Cx;
	I2C_Config_t 		I2C_Config;

	uint8_t 			*pTxBuffer;				/* 		Storing Tx buffer address			*/
	uint8_t 			*pRxBuffer;				/* 		Storing Rx buffer address			*/
	uint32_t 			TxLen;					/* 		Storing Tx length					*/
	uint32_t 			RxLen;					/* 		Storing Tx length					*/
	uint8_t 			TxRxState;				/* 		Storing the communication state		*/
	uint8_t				DevAddr;				/* 		Storing address of device/slave		*/
	uint32_t 			RxSize;					/* 		Storing Rx size	- starting RxLen	*/
	uint8_t				Sr;						/* 		Storing reapted start value			*/

}I2C_Handle_t;


/*
 * 	@I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_SM			100000
#define I2C_SCL_SPEED_FM4K			400000
#define I2C_SCL_SPEED_FM2K			200000

/*
 * 	@I2C_ACKControl
 */
#define I2C_ACK_ENABLE				1
#define I2C_ACK_DISABLE				0

/*
 * 	@I2C_FMDutyCycle
 */
#define I2C_FM_DUTY_2				0
#define I2C_FM_DUTY_16_9			1

/*
 *  I2C_sr
 */
#define I2C_DISABLE_SR				DISABLE
#define I2C_ENABLE_SR				ENABLE

/*
 * I2C SR Flags
 */
#define I2C_FLAG_SB					(1 << I2C_SR1_SB)
#define I2C_FLAG_ADDR				(1 << I2C_SR1_ADDR)
#define I2C_FLAG_BTF				(1 << I2C_SR1_BTF)
#define I2C_FLAG_RxNE				(1 << I2C_SR1_RxNE)
#define I2C_FLAG_TxE				(1 << I2C_SR1_TxE)
#define I2C_FLAG_STOPF				(1 << I2C_SR1_STOPF)
#define I2C_FLAG_BEER				(1 << I2C_SR1_BERR)
#define I2C_FLAG_ARLO				(1 << I2C_SR1_ARLO)
#define I2C_FLAG_AF					(1 << I2C_SR1_AF)
#define I2C_FLAG_OVR				(1 << I2C_SR1_OVR)
#define I2C_FLAG_TIMEOUT			(1 << I2C_SR1_TIMEOUT)


/*
 *  TRISE time
 */
#define I2C_TRISE_MAX_SM			1000 		//ns
#define I2C_TRISE_MAX_FM			300 		//ns


/*
 *	I2C Application states
 */
#define I2C_READY					0
#define I2C_BUSY_IN_RX				1
#define I2C_BUSY_IN_TX				2

/*
 * 	I2C application events macros
 */
#define I2C_EV_TX_CMPLT				0
#define I2C_EV_RX_CMPLT				1
#define I2C_EV_STOP					2
#define I2C_ERROR_BERR  			3
#define I2C_ERROR_ARLO  			4
#define I2C_ERROR_AF    			5
#define I2C_ERROR_OVR  				6
#define I2C_ERROR_TIMEOUT 			7
#define I2C_EV_DATA_REQ				8
#define I2C_EV_DATA_RCV				9

/*
 * 	I2C Reapted start macros
 */
#define I2C_RS_DISABLED				RESET
#define I2C_RS_ENABLED				SET


/*****************************************************************************************
 * 								APIs supported by this driver
 * 			For more information about APIs check the function definitions
 *****************************************************************************************/
/*
 *  Peripheral Clock setup
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDI); 		/* Enable or Disable clock for given I2C address*/


/*
 * 	Init and De_init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);								/* Setting registers back to theirs original state, done using RCC_AHB1RSTR (example)*/

/*
 * Send and Receive data
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr);

uint8_t  I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr);

void I2C_SlaveSendData(I2C_RegDef_t *pI2C, uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C);

/*
 * 	IRQ Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDI);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);

/*
 * 	Other peripheral control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);

uint32_t RCC_GetPCLK1Value(void);
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);

void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/*
 * 	Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);


#endif /* INC_STM32F411XX_I2C_DRIVER_H_ */
