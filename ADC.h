
#ifndef ADC_H_
#define ADC_H_

#include "stm32f4xx.h"

#define ADCJConversionIsComplete()  ADC1->SR&ADC_SR_JEOC
extern void ADCInit();
extern void ADCJTimerTrigEnable();
extern void ADCJTimerTrigDisable();
extern uint8_t ADCCh1();
extern uint8_t ADCCh2();

#endif /* ADC_H_ */
