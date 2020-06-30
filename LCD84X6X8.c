
#include "LCD84X6X8.h"

volatile uint8_t currentView[84][6]={0};
volatile LCDObject* currentEditObject;

volatile uint32_t lcdCounter=0;

void LCDData(uint8_t data);
void LCDCommand(uint8_t);
void LCDSetXAddress(uint8_t addr);
void LCDSetYAddress(uint8_t addr);
void LCDPlotObjectProtocole();
void LCDRemoveObjectProtocole();

void LCDInit()
{
	SPI1_init();
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER=(GPIOA->MODER&(~(GPIO_MODER_MODER8|GPIO_MODER_MODER15)))|(GPIO_MODER_MODER8_0|GPIO_MODER_MODER15_0);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT_8|GPIO_OTYPER_OT_15);
	GPIOA->OSPEEDR|=(GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR15);
	GPIOA->PUPDR&=~(GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR15);
	GPIOA->BSRR=GPIO_BSRR_BR_15;
	delay(0xfffff);
	GPIOA->BSRR=GPIO_BSRR_BS_15;
	delay(0xfffff);
	GPIOA->BSRR=GPIO_BSRR_BR_15;
	delay(0xfffff);
	GPIOA->BSRR=GPIO_BSRR_BS_15;
	delay(0xfffff);
	LCDCommand(0x21); // LCD Extended Commands.
	LCDCommand(0xB9); // Set LCD Vop (Contrast).
	LCDCommand(0x04); // Set Temp coefficent.
	LCDCommand(0x14); // LCD bias mode 1:48.
	LCDCommand(0x20);  //LCD Basic Commands
	LCDNormalMode();
	delay(0xfffff);
	LCDSetXAddress(0x00);
	LCDSetYAddress(0x00);
	LCDClearScreen();
}

void LCDCommand(uint8_t command)
{
	while(!SPI1_TX_IS_EMPTY);
	delay(8);
	GPIOA->BSRR=GPIO_BSRR_BR_8;
	delay(1);
	SPI1_DATA_REGISTER=(uint16_t)command;
}

void LCDDisplayBlank()
{
	LCDCommand(0b1000);
}

void LCDNormalMode()
{
	LCDCommand(0b1100);
}

void LCDDisplayAll()
{
	LCDCommand(0b1001);
}

void LCDInverseMode()
{
	LCDCommand(0b1101);
}

void LCDSetXAddress(uint8_t addr)
{
	LCDCommand(0x80|addr);//0->83
}

void LCDSetYAddress(uint8_t addr)
{
	LCDCommand(0x40|addr);//0->5
}

void LCDData(uint8_t data)
{
	while(!SPI1_TX_IS_EMPTY);
	delay(8);
	GPIOA->BSRR=GPIO_BSRR_BS_8;
	delay(1);
	SPI1_DATA_REGISTER=(uint16_t)data;
}

void LCDPlotObject(LCDObject* target)
{
	while(1)
	{
		SPI1_TXBEmptyIntDisable();
		if((currentEditObject->mode!=busy)&&(target->mode!=busy))
		{
			currentEditObject=target;
			break;
		}
		SPI1_TXBEmptyIntResume();
		delay(400);//100us 35 byte
	}
	target->mode=busy;
	LCDSetXAddress(target->xLocation);
	LCDSetYAddress(target->yLocation/8);
	currentView[target->xLocation][target->yLocation/8]|=(target->shape[0][0]<<(target->yLocation%8));
	LCDData(currentView[target->xLocation][target->yLocation/8]);
	if(target->xSize!=1)
	{
		SPI1_TXBEmptyIntEnable(LCDPlotObjectProtocole);
	}
	else
	{
		target->mode=idle;
	}
}

void LCDPlotObjectProtocole()
{
	static uint8_t startFlag=1;
	static uint8_t xMax;
	static uint8_t yMax;
	static uint8_t xCounter;
	static uint8_t yCounter;
	static uint8_t yShift;
	uint8_t temp;
	if(startFlag)
	{
		startFlag=0;
		if((currentEditObject->xSize+currentEditObject->xLocation)>84)
		{
			xMax=84-currentEditObject->xLocation;
		}
		else
		{
			xMax=currentEditObject->xSize;
		}
		if((currentEditObject->ySize+currentEditObject->yLocation)>48)
		{
			temp=48-currentEditObject->yLocation;
			yMax=(temp+(currentEditObject->yLocation%8))/8;
		}
		else
		{
			yMax=(currentEditObject->yLocation%8)+currentEditObject->ySize;
			if(yMax%8)
			{
				yMax=(yMax/8)+1;
			}
			else
			{
				yMax/=8;
			}
		}
		xCounter=1;
		yCounter=0;
		yShift=currentEditObject->yLocation%8;
	}
	if(yCounter>0)
	{
		currentView[currentEditObject->xLocation+xCounter][(currentEditObject->yLocation/8)+yCounter]|=((currentEditObject->shape[xCounter][yCounter]<<yShift)|(currentEditObject->shape[xCounter][yCounter-1]>>(8-yShift)));
	}
	else
	{
		currentView[currentEditObject->xLocation+xCounter][(currentEditObject->yLocation/8)+yCounter]|=(currentEditObject->shape[xCounter][yCounter]<<yShift);
	}
	SPI1_DATA_REGISTER=(uint16_t)currentView[currentEditObject->xLocation+xCounter][(currentEditObject->yLocation/8)+yCounter];
	xCounter++;
	if(xCounter==xMax)
	{
		xCounter=0;
		yCounter++;
		if(yCounter==yMax)
		{
			startFlag=1;
			currentEditObject->mode=idle;
			SPI1_TXBEmptyIntDisable();
			return;
		}
		LCDSetXAddress(currentEditObject->xLocation);
		LCDSetYAddress((currentEditObject->yLocation/8)+yCounter);
		currentView[currentEditObject->xLocation][(currentEditObject->yLocation/8)+yCounter]|=((currentEditObject->shape[0][yCounter]<<yShift)|(currentEditObject->shape[0][yCounter-1]>>(8-yShift)));
		LCDData(currentView[currentEditObject->xLocation][(currentEditObject->yLocation/8)+yCounter]);
	}
}

void LCDRemoveObject(LCDObject* target)
{
	while(1)
	{
		SPI1_TXBEmptyIntDisable();
		if((currentEditObject->mode!=busy)&&(target->mode!=busy))
		{
			currentEditObject=target;
			break;
		}
		SPI1_TXBEmptyIntResume();
		delay(400);//100us 35 byte
	}
	target->mode=busy;
	LCDSetXAddress(target->xLocation);
	LCDSetYAddress(target->yLocation/8);
	currentView[target->xLocation][target->yLocation/8]&=(~(target->shape[0][0]<<(target->yLocation%8)));
	LCDData(currentView[target->xLocation][target->yLocation/8]);
	if(target->xSize!=1)
	{
		SPI1_TXBEmptyIntEnable(LCDRemoveObjectProtocole);
	}
	else
	{
		target->mode=unplot;
	}
}

void LCDRemoveObjectProtocole()
{
	static uint8_t startFlag=1;
	static uint8_t xMax;
	static uint8_t yMax;
	static uint8_t xCounter;
	static uint8_t yCounter;
	static uint8_t yShift;
	uint8_t temp;
	if(startFlag)
	{
		startFlag=0;
		if((currentEditObject->xSize+currentEditObject->xLocation)>84)
		{
			xMax=84-currentEditObject->xLocation;
		}
		else
		{
			xMax=currentEditObject->xSize;
		}
		if((currentEditObject->ySize+currentEditObject->yLocation)>48)
		{
			temp=48-currentEditObject->yLocation;
			yMax=(temp+(currentEditObject->yLocation%8))/8;
		}
		else
		{
			yMax=(currentEditObject->yLocation%8)+currentEditObject->ySize;
			if(yMax%8)
			{
				yMax=(yMax/8)+1;
			}
			else
			{
				yMax/=8;
			}
		}
		xCounter=1;
		yCounter=0;
		yShift=currentEditObject->yLocation%8;
	}
	if(yCounter>0)
	{
		currentView[currentEditObject->xLocation+xCounter][(currentEditObject->yLocation/8)+yCounter]&=(~((currentEditObject->shape[xCounter][yCounter]<<yShift)|(currentEditObject->shape[xCounter][yCounter-1]>>(8-yShift))));
	}
	else
	{
		currentView[currentEditObject->xLocation+xCounter][(currentEditObject->yLocation/8)+yCounter]&=(~(currentEditObject->shape[xCounter][yCounter]<<yShift));
	}
	SPI1_DATA_REGISTER=(uint16_t)currentView[currentEditObject->xLocation+xCounter][(currentEditObject->yLocation/8)+yCounter];
	xCounter++;
	if(xCounter==xMax)
	{
		xCounter=0;
		yCounter++;
		if(yCounter==yMax)
		{
			startFlag=1;
			currentEditObject->mode=unplot;
			SPI1_TXBEmptyIntDisable();
			return;
		}
		LCDSetXAddress(currentEditObject->xLocation);
		LCDSetYAddress((currentEditObject->yLocation/8)+yCounter);
		currentView[currentEditObject->xLocation][(currentEditObject->yLocation/8)+yCounter]&=(~((currentEditObject->shape[0][yCounter]<<yShift)|(currentEditObject->shape[0][yCounter-1]>>(8-yShift))));
		LCDData(currentView[currentEditObject->xLocation][(currentEditObject->yLocation/8)+yCounter]);
	}
}

void LCDObjectInit(LCDObject* target,uint8_t sizeOfX,uint8_t sizeOfY,uint8_t locationOfX,uint8_t locationOfY)
{
	//size of x must not be less than 2
	uint8_t i,j;
	target->mode=unplot;
	target->xSize=sizeOfX;
	target->ySize=sizeOfY;
	target->xLocation=locationOfX;
	target->yLocation=locationOfY;
	for(i=0;i<84;i++)
		for(j=0;j<6;j++)
			target->shape[i][j]=0;
}

void LCDWaitForObjectNBusy(LCDObject* target)
{
	while(1)
	{
		SPI1_TXBEmptyIntDisable();
		if(target->mode!=busy)
		{
			if(currentEditObject->mode==busy)
			{
				SPI1_TXBEmptyIntResume();
			}
			break;
		}
		SPI1_TXBEmptyIntResume();
		delay(400);//100us 35 byte
	}
}


void LCDClearScreen()
{
	uint32_t i;
	for(i=0;i<4032;i++)
		{
			LCDData(0x00);
		}
}
