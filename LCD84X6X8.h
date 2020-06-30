/*
 * LCD84X6X8.h
 *
 *  Created on: Aug 15, 2019
 *      Author: 6r6Eng
 */

#ifndef LCD84X6X8_H_
#define LCD84X6X8_H_

#include "SPI1.h"

typedef enum {unplot,idle,busy}LCDObjectMode;

typedef struct {
	uint8_t xSize;//in bits
	uint8_t ySize;//in bits
	uint8_t shape[84][6];//x,y
	uint8_t xLocation;
	uint8_t yLocation;
	LCDObjectMode mode;
}LCDObject;

extern void LCDInit();
extern void LCDDisplayBlank();
extern void LCDNormalMode();
extern void LCDDisplayAll();
extern void LCDInverseMode();
extern void LCDPlotObject(LCDObject* target);
extern void LCDRemoveObject(LCDObject* target);
extern void LCDObjectInit(LCDObject* target,uint8_t sizeOfX,uint8_t sizeOfY,uint8_t locationOfX,uint8_t locationOfY);
extern void LCDWaitForObjectNBusy(LCDObject* target);
extern void LCDClearScreen();

#endif /* LCD84X6X8_H_ */
