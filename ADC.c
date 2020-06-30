//PA1->IN1 PA2->IN2 ADC1-->APB2

#include "ADC.h"

static volatile uint8_t ch1Data=255;
static volatile uint8_t ch2Data=255;

void ADCJConversionCompleteClearFlag();
void ADCJConversionCompleteFlagIntEnable();
void ADCJConversionCompleteFlagIntDisable();
void ADCJStartConversion();

void ADCInit()
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;//GPIO Power On
	RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;//ADC power ON
	RCC->APB1ENR|=RCC_APB1ENR_TIM5EN;//timer power on
	GPIOA->MODER|=(GPIO_MODER_MODER1|GPIO_MODER_MODER2);//a1 a2
	GPIOA->PUPDR&=(~(GPIO_PUPDR_PUPDR1|GPIO_PUPDR_PUPDR2));
	ADC1->CR2|=ADC_CR2_ADON;//ADC on
	ADC->CCR=(ADC->CCR&(~ADC_CCR_ADCPRE))|ADC_CCR_ADCPRE_0;//prescaler APB2/4
	ADC1->CR1=(ADC1->CR1&(~ADC_CR1_RES))|ADC_CR1_RES_1; //8 bit resolution
	ADC1->CR1|=ADC_CR1_JDISCEN;//enable injected group sequence
	ADC1->CR1|=ADC_CR1_SCAN;
	ADC1->CR2=(ADC1->CR2&(~ADC_CR2_JEXTSEL))|ADC_CR2_JEXTSEL_0|ADC_CR2_JEXTSEL_1|ADC_CR2_JEXTSEL_3;//Timer 5 TRGO event is trigger to injected channel external trig
	ADC1->CR2&=(~ADC_CR2_ALIGN);//right alignment
	ADC1->SMPR2&=(~(ADC_SMPR2_SMP1|ADC_SMPR2_SMP2));// sample time 3 cycles
	ADC1->JSQR=(ADC1->JSQR&(~ADC_JSQR_JL))|ADC_JSQR_JL_0;//2 conversions in injected sequence
	ADC1->JSQR=(ADC1->JSQR&(~(ADC_JSQR_JSQ3|ADC_JSQR_JSQ4)))|(ADC_JSQR_JSQ3_0|ADC_JSQR_JSQ4_1);// ch1 then ch2 in injected sequence
	NVIC_SetPriority(ADC_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,0));
	NVIC_EnableIRQ(ADC_IRQn);
	ADCJTimerTrigEnable();
	ADCJConversionCompleteFlagIntEnable();
	//init timer
	TIM5->CR1|=TIM_CR1_ARPE;//auto-reload
	TIM5->CR1&=(~TIM_CR1_CMS);
	TIM5->CR1|=TIM_CR1_DIR;// count down
	TIM5->CR1|=TIM_CR1_URS;//only overflow or underflow generate an interrupt
	TIM5->CR2=(TIM5->CR2&(~TIM_CR2_MMS))|TIM_CR2_MMS_1;//trgo at update event
	TIM5->CNT=200000;//240Hz out; 120Hz adc
	TIM5->ARR=200000;
	TIM5->CR1|=TIM_CR1_CEN;//enable counter
}

void ADCJConversionCompleteClearFlag()
{
	ADC1->SR&=(~((uint32_t)ADC_SR_JEOC));
}

void ADCJConversionCompleteFlagIntEnable()
{
	ADC1->CR1|=ADC_CR1_JEOCIE;
}

void ADCJConversionCompleteFlagIntDisable()
{
	ADC1->CR1&=(~ADC_CR1_JEOCIE);
}

void ADCJStartConversion()
{
	ADC1->CR2|=ADC_CR2_JSWSTART;
}

void ADCJTimerTrigEnable()
{
	ADC1->CR2=(ADC1->CR2&(~ADC_CR2_JEXTEN))|ADC_CR2_JEXTEN_0;
}

void ADCJTimerTrigDisable()
{
	ADC1->CR2&=(~ADC_CR2_JEXTEN);
}

void ADC_IRQHandler()
{
	ch1Data=(uint8_t)(((uint32_t)0x000000ff)&ADC1->JDR1);
	ch2Data=(uint8_t)(((uint32_t)0x000000ff)&ADC1->JDR2);
	ADCJConversionCompleteClearFlag();
}

uint8_t ADCCh1()
{
	uint8_t data;
	ADCJConversionCompleteFlagIntDisable();
	data=ch1Data;
	ADCJConversionCompleteFlagIntEnable();
	return data;
}

uint8_t ADCCh2()
{
	uint8_t data;
	ADCJConversionCompleteFlagIntDisable();
	data=ch2Data;
	ADCJConversionCompleteFlagIntEnable();
	return data;
}
