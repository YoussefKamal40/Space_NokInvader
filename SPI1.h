/*
 * SPI1.h
 *
 *  Created on: Aug 15, 2019
 *
 */

#ifndef SPI1_H_
#define SPI1_H_

#include "stm32f4xx.h"

#define SPI1_IS_BUSY ((uint8_t)(SPI1->SR&SPI_SR_BSY))
#define SPI1_TX_IS_EMPTY ((uint8_t)(SPI1->SR&SPI_SR_TXE))
#define SPI1_DATA_REGISTER (SPI1->DR)

extern void SPI1_init();
extern void SPI1_TXBEmptyIntEnable();
extern void SPI1_TXBEmptyIntDisable();
extern void SPI1_TXBEmptyIntResume(void);

#endif /* SPI1_H_ */
