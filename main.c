/* Includes */
#include "stm32f4xx.h"
#include "LCD84X6X8.h"
#include "ADC.h"

/* Private macro */
#define PLAYER_HEIGHT       12
#define PLAYER_WIDTH        15
#define PLAYER_SCORE		5
#define ENIMY1_HEIGHT       10
#define ENIMY1_WIDTH        8
#define ENIMY1_SCORE		5
#define ENIMY2_HEIGHT       10
#define ENIMY2_WIDTH        12
#define ENIMY2_SCORE		20
#define ENIMY3_HEIGHT 	    6
#define ENIMY3_WIDTH   	    8
#define ENIMY3_SCORE		1
#define ENIMY3_NUMBER	 	10
#define ENIMY3_INTERSPACE_HORIZONTAL	5
#define ENIMY3_INTERSPACE_VERTICAL	    3
#define MAX_ENIMY_HIGHT     10
#define MAX_ENIMY_WIDTH  	12
#define FIRE_WIDTH 			8
#define FIRE_HEIGHT 		8
#define SCREEN_WIDTH     	84
#define SCREEN_HEIGHT    	48
#define MAX_INTERSPACE_HORIZONTAL 		5
#define MAX_INTERSPACE_VERTICAL			3

#define SHOOT_STATE (!(GPIOA->IDR&GPIO_IDR_IDR_3))
#define ROCKET_HEIGHT 4
#define PLAYER_ROCKET_WAIT 1
#define ENIMY_ROCKET_WAIT 3
/* Private variables */
volatile uint32_t mailBox[100];
volatile uint8_t mailBoxNOM=0;
/* Private function prototypes */
uint8_t rand(uint8_t lim);
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
	uint32_t buffer[100];
	uint8_t bufferNOB=0;
	uint8_t i;
	char tempChar;
	uint8_t temp8Bit;;
	LCDObject player;
	LCDObject enimy3[ENIMY3_NUMBER];
	LCDObject rocket;
	LCDObject fire;

	SystemInit();
	SystemCoreClockUpdate();
	NVIC_SetPriorityGrouping(4);

	LCDObjectInit(&fire,FIRE_WIDTH,FIRE_HEIGHT,0,0);
	fire.shape[0][0]=0b00101100;
	fire.shape[1][0]=0b01011010;
	fire.shape[2][0]=0b10110101;
	fire.shape[3][0]=0b11101010;
	fire.shape[4][0]=0b01010111;
	fire.shape[5][0]=0b10101101;
	fire.shape[6][0]=0b01011010;
	fire.shape[7][0]=0b00110100;

	LCDObjectInit(&rocket,1,ROCKET_HEIGHT,0,0);
	rocket.shape[0][0]=0b00001111;

	LCDObjectInit(&player,PLAYER_WIDTH,PLAYER_HEIGHT,(SCREEN_WIDTH/2)-(PLAYER_WIDTH/2),MAX_ENIMY_HIGHT*2+MAX_INTERSPACE_VERTICAL);
	player.shape[0 ][0]=0b11000000;
	player.shape[1 ][0]=0b01000100;
	player.shape[2 ][0]=0b10101000;
	player.shape[3 ][0]=0b11110000;
	player.shape[4 ][0]=0b11100000;
	player.shape[5 ][0]=0b11110000;
	player.shape[6 ][0]=0b11111100;
	player.shape[7 ][0]=0b11111111;
	player.shape[8 ][0]=0b11111100;
	player.shape[9 ][0]=0b11110000;
	player.shape[10][0]=0b11100000;
	player.shape[11][0]=0b11110000;
	player.shape[12][0]=0b10101000;
	player.shape[13][0]=0b01000100;
	player.shape[14][0]=0b11000000;
	player.shape[0 ][1]=0b00001111;
	player.shape[1 ][1]=0b00000101;
	player.shape[2 ][1]=0b00000010;
	player.shape[3 ][1]=0b00000011;
	player.shape[4 ][1]=0b00001001;
	player.shape[5 ][1]=0b00000111;
	player.shape[6 ][1]=0b00001000;
	player.shape[7 ][1]=0b00000011;
	player.shape[8 ][1]=0b00001000;
	player.shape[9 ][1]=0b00000111;
	player.shape[10][1]=0b00001001;
	player.shape[11][1]=0b00000011;
	player.shape[12][1]=0b00000010;
	player.shape[13][1]=0b00000101;
	player.shape[14][1]=0b00001111;

	for(i=0;i<ENIMY3_NUMBER/2;i++)
	{
		LCDObjectInit(enimy3+i,ENIMY3_WIDTH,ENIMY3_HEIGHT,i*(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL),0);
		enimy3[i].shape[0][0]=0b011000;
		enimy3[i].shape[1][0]=0b011100;
		enimy3[i].shape[2][0]=0b110110;
		enimy3[i].shape[3][0]=0b011111;
		enimy3[i].shape[4][0]=0b011111;
		enimy3[i].shape[5][0]=0b110110;
		enimy3[i].shape[6][0]=0b011100;
		enimy3[i].shape[7][0]=0b011000;
	}

	for(i=0;i<ENIMY3_NUMBER/2;i++)
	{
		LCDObjectInit(enimy3+i+(ENIMY3_NUMBER/2),ENIMY3_WIDTH,ENIMY3_HEIGHT,i*(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL),ENIMY3_HEIGHT+ENIMY3_INTERSPACE_VERTICAL);
		enimy3[i+(ENIMY3_NUMBER/2)].shape[0][0]=0b011000;
		enimy3[i+(ENIMY3_NUMBER/2)].shape[1][0]=0b011100;
		enimy3[i+(ENIMY3_NUMBER/2)].shape[2][0]=0b110110;
		enimy3[i+(ENIMY3_NUMBER/2)].shape[3][0]=0b011111;
		enimy3[i+(ENIMY3_NUMBER/2)].shape[4][0]=0b011111;
		enimy3[i+(ENIMY3_NUMBER/2)].shape[5][0]=0b110110;
		enimy3[i+(ENIMY3_NUMBER/2)].shape[6][0]=0b011100;
		enimy3[i+(ENIMY3_NUMBER/2)].shape[7][0]=0b011000;
	}

	LCDInit();
	ADCInit();

	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;//timer power on
	TIM2->CR1|=TIM_CR1_ARPE;//auto-reload
	TIM2->CR1&=(~TIM_CR1_CMS);
	TIM2->CR1|=TIM_CR1_DIR;// count down
	TIM2->CR1|=TIM_CR1_URS;//only overflow or underflow generate an interrupt
	TIM2->CNT=800000;//60Hz
	TIM2->ARR=800000;
	TIM2->CR1|=TIM_CR1_CEN;//enable counter
	TIM2->SR&=(~TIM_SR_UIF);
	TIM2->DIER|=TIM_DIER_UIE;//enable interrupt
	NVIC_SetPriority(TIM2_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,0));
	NVIC_EnableIRQ(TIM2_IRQn);
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER&=(~GPIO_MODER_MODER3);
	GPIOA->PUPDR=(GPIOA->PUPDR&(~GPIO_PUPDR_PUPDR3))|(GPIO_PUPDR_PUPDR3_0);//a3	`

	LCDPlotObject(&player);
	for(i=0;i<ENIMY3_NUMBER;i++)
	{
		LCDPlotObject(enimy3+i);
	}

	while (1)
	{
		TIM2->DIER&=(~TIM_DIER_UIE);//disable interrupt
		if(mailBoxNOM)
		{
			for(bufferNOB=mailBoxNOM;mailBoxNOM>0;mailBoxNOM--)
			{
				buffer[mailBoxNOM-1]=mailBox[mailBoxNOM-1];
			}
		}
		TIM2->DIER|=TIM_DIER_UIE;//enable interrupt

		if(bufferNOB==0)
		{
			delay(100);
		}

		for(i=0;i<bufferNOB;i++)
		{
			if((((buffer[i]>>23)&((uint32_t)0b11))==0b01)||(((buffer[i]>>23)&((uint32_t)0b11))==0b10)||(((buffer[i]>>23)&((uint32_t)0b11))==0b11))//remove or move or move abs
			{
				switch((buffer[i]>>(16+4))&((uint32_t)0b111))
				{
				case 0b000://player
					LCDWaitForObjectNBusy(&player);
					LCDRemoveObject(&player);
					break;
				case 0b100://rocket
					LCDWaitForObjectNBusy(&rocket);
					rocket.xLocation=((uint8_t)((buffer[i]>>8)&0xff));
					rocket.yLocation=((uint8_t)(buffer[i]&0xff));
					LCDRemoveObject(&rocket);
					break;
				case 0b011://enimy3
					LCDWaitForObjectNBusy(enimy3+((buffer[i]>>16)&0b1111));
					LCDRemoveObject(enimy3+((buffer[i]>>16)&0b1111));
					break;
				case 0b101://fire
					LCDWaitForObjectNBusy(&fire);
					fire.xLocation=((uint8_t)((buffer[i]>>8)&0xff));
					fire.yLocation=((uint8_t)(buffer[i]&0xff));
					LCDRemoveObject(&fire);
					break;
				}
			}
		}

		for(i=0;i<bufferNOB;i++)
		{
			if((((buffer[i]>>23)&((uint32_t)0b11))==0)||(((buffer[i]>>23)&((uint32_t)0b11))==0b11))//plot or move abs
			{
				switch((buffer[i]>>(20))&((uint32_t)0b111))
				{
				case 0b000://player
					LCDWaitForObjectNBusy(&player);
					player.xLocation=((uint8_t)((buffer[i]>>8)&0xff));
					player.yLocation=((uint8_t)(buffer[i]&0xff));
					LCDPlotObject(&player);
					break;
				case 0b100://rocket
					LCDWaitForObjectNBusy(&rocket);
					rocket.xLocation=((uint8_t)((buffer[i]>>8)&0xff));
					rocket.yLocation=((uint8_t)(buffer[i]&0xff));
					LCDPlotObject(&rocket);
					break;
				case 0b011://enimy 3
					LCDWaitForObjectNBusy(enimy3+((buffer[i]>>16)&0b1111));
					enimy3[(buffer[i]>>16)&0b1111].xLocation=((uint8_t)((buffer[i]>>8)&0xff));
					enimy3[(buffer[i]>>16)&0b1111].yLocation=((uint8_t)(buffer[i]&0xff));
					LCDPlotObject(enimy3+((buffer[i]>>16)&0b1111));
					break;
				case 0b101://fire
					LCDWaitForObjectNBusy(&fire);
					fire.xLocation=((uint8_t)((buffer[i]>>8)&0xff));
					fire.yLocation=((uint8_t)(buffer[i]&0xff));
					LCDPlotObject(&fire);
					break;
				}
			}
		}

		for(i=0;i<bufferNOB;i++)
		{
			if(((buffer[i]>>(23))&((uint32_t)0b11))==0b10)//move relative
				switch((buffer[i]>>(20))&((uint32_t)0b111))
				{
				case 0b011://enimy 3
					LCDWaitForObjectNBusy(enimy3+((buffer[i]>>16)&0b1111));
					temp8Bit=(uint8_t)((buffer[i]>>8)&0xff);
					tempChar=*((char*)(&temp8Bit));
					tempChar+=enimy3[(buffer[i]>>16)&0b1111].xLocation;
					enimy3[(buffer[i]>>16)&0b1111].xLocation=(uint8_t)tempChar;
					temp8Bit=(uint8_t)(buffer[i]&0xff);
					tempChar=*((char*)(&temp8Bit));
					tempChar+=enimy3[(buffer[i]>>16)&0b1111].yLocation;
					enimy3[(buffer[i]>>16)&0b1111].yLocation=(uint8_t)tempChar;
					LCDPlotObject(enimy3+((buffer[i]>>16)&0b1111));
					break;
				}
		}

		bufferNOB=0;
	}
	return 0;
}

void TIM2_IRQHandler()
{
	static uint8_t currentPlayerX=0;
	static uint8_t currentPlayerY=MAX_ENIMY_HIGHT;
	static struct playerRocket
	{
		uint8_t workingFlag;
		uint8_t x;
		uint8_t y;
		uint8_t wait;
	}playerRocket={0,0,0,0};

	static struct enimyRocket
	{
		uint8_t workingFlag;
		uint8_t x;
		uint8_t y;
		uint8_t wait;
	}enimyRocket={0,0,0,0};

	static struct enimy3Data
	{
		uint8_t score[ENIMY3_NUMBER];
		uint8_t index;
		uint8_t wait;
		uint8_t waitRef;
		uint8_t halfMaxIndex;
		uint8_t x[ENIMY3_NUMBER];
		uint8_t y[ENIMY3_NUMBER];
		uint8_t fireFlag[ENIMY3_NUMBER];
	}enimy3Data={
			{ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE,ENIMY3_SCORE},
			0,
			20,
			20,
			(SCREEN_WIDTH-(ENIMY3_WIDTH*(ENIMY3_NUMBER/2))-(ENIMY3_INTERSPACE_HORIZONTAL*((ENIMY3_NUMBER/2)-1))),
			{
					0,
					ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL,
					(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL)*2,
					(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL)*3,
					(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL)*4,
					0,
					ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL,
					(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL)*2,
					(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL)*3,
					(ENIMY3_WIDTH+ENIMY3_INTERSPACE_HORIZONTAL)*4,
			},
			{
					0,0,0,0,0,
					ENIMY3_HEIGHT+ENIMY3_INTERSPACE_VERTICAL,
					ENIMY3_HEIGHT+ENIMY3_INTERSPACE_VERTICAL,
					ENIMY3_HEIGHT+ENIMY3_INTERSPACE_VERTICAL,
					ENIMY3_HEIGHT+ENIMY3_INTERSPACE_VERTICAL,
					ENIMY3_HEIGHT+ENIMY3_INTERSPACE_VERTICAL,
			},
			{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}
	};

	uint8_t newPlayerX,newPlayerY,xPot,yPot,temp8Bit;
	uint32_t temp32Bit=0;
	uint8_t i;
	char tempChar;
	static uint8_t playerScore=PLAYER_SCORE;

	TIM2->SR&=(~TIM_SR_UIF);
	if(playerScore==0)
	{
		mailBox[mailBoxNOM]=((uint32_t)(0b01<<23));
		mailBox[mailBoxNOM+1]=((uint32_t)(0b101<<20))|((uint32_t)(((uint32_t)currentPlayerX)<<8))|((uint32_t)(currentPlayerY));
		mailBoxNOM+=2;
		TIM2->CR1&=~TIM_CR1_CEN;
		TIM2->DIER&=~TIM_DIER_UIE;
	}
	xPot=ADCCh1();
	yPot=ADCCh2();
	if(xPot>235)
	{
		xPot=235;
	}
	else if(xPot<20)
	{
		xPot=20;
	}
	xPot-=20;
	if(yPot>235)
	{
		yPot=235;
	}
	else if(yPot<20)
	{
		yPot=20;
	}
	yPot-=20;
	newPlayerX=(uint8_t)((((float)xPot)*((float)(SCREEN_WIDTH-PLAYER_WIDTH)))/((float)215.0));
	newPlayerY=(uint8_t)(((float)yPot*(float)(SCREEN_HEIGHT-4.0))/(float)(215.0));
	if(newPlayerX!=currentPlayerX||newPlayerY!=currentPlayerY)
	{
		temp32Bit|=(((uint32_t)0b11)<<23);//move abs
		temp32Bit&=(~(((uint32_t)0b111)<<20));//player
		temp32Bit|=(((uint32_t)newPlayerX)<<8);
		temp32Bit|=((uint32_t)newPlayerY);
		mailBox[mailBoxNOM]=temp32Bit;
		mailBoxNOM++;
		currentPlayerX=newPlayerX;
		currentPlayerY=newPlayerY;
	}
	if(!enimyRocket.workingFlag)
	{
		temp8Bit=rand(9);
		while(enimy3Data.score[temp8Bit]==0)
		{
			temp8Bit=rand(9);
		}
		enimyRocket.workingFlag=0xff;
		enimyRocket.x=enimy3Data.x[temp8Bit]+((ENIMY3_WIDTH/2)-rand(1));
		enimyRocket.y=enimy3Data.y[temp8Bit]+ENIMY3_HEIGHT;
		mailBox[mailBoxNOM]=(((uint32_t)0b100)<<20)|(((uint32_t)enimyRocket.x)<<8)|((uint32_t)enimyRocket.y);
		mailBoxNOM++;
		enimyRocket.wait=ENIMY_ROCKET_WAIT;
	}
	if(enimyRocket.workingFlag)
	{
		if(enimyRocket.wait==0)
		{
			mailBox[mailBoxNOM]=(((uint32_t)0b01)<<23)|(((uint32_t)0b100)<<20)|(((uint32_t)enimyRocket.x)<<8)|((uint32_t)enimyRocket.y);
			mailBoxNOM++;
			if(enimyRocket.y!=(SCREEN_HEIGHT-1))
			{
				if((enimyRocket.x>=currentPlayerX)&&(enimyRocket.x<(currentPlayerX+PLAYER_WIDTH))&&((enimyRocket.y+ROCKET_HEIGHT-1)>=currentPlayerY)&&((enimyRocket.y+ROCKET_HEIGHT-1)<(currentPlayerY+PLAYER_HEIGHT)))
				{
					playerScore--;
					enimyRocket.workingFlag=0x00;
				}
				if(enimyRocket.workingFlag)
				{
					enimyRocket.y++;
					mailBox[mailBoxNOM]=(((uint32_t)0b100)<<20)|(((uint32_t)enimyRocket.x)<<8)|((uint32_t)enimyRocket.y);
					mailBoxNOM++;
					enimyRocket.wait=ENIMY_ROCKET_WAIT;
				}
			}
			else
			{
				enimyRocket.workingFlag=0x00;
			}
		}
		else
		{
			enimyRocket.wait--;
		}
	}
	if(playerRocket.workingFlag)
	{
		if(playerRocket.wait==0)
		{
			mailBox[mailBoxNOM]=(((uint32_t)0b01)<<23)|(((uint32_t)0b100)<<20)|(((uint32_t)playerRocket.x)<<8)|((uint32_t)playerRocket.y);
			mailBoxNOM++;
			if(playerRocket.y!=0)
			{
				if(playerRocket.y<((ENIMY3_HEIGHT*2)+ENIMY3_INTERSPACE_VERTICAL))
				{
					if(playerRocket.y<ENIMY3_HEIGHT)
					{
						for(i=0;i<ENIMY3_NUMBER/2;i++)
						{
							if((playerRocket.x>=enimy3Data.x[i])&&(playerRocket.x<(enimy3Data.x[i]+ENIMY3_WIDTH)))
							{
								if(enimy3Data.score[i]!=0)
								{
									enimy3Data.score[i]--;
									playerRocket.workingFlag=0x00;
									enimy3Data.waitRef-=2;
								}
							}
						}
					}
					else if(playerRocket.y>=(ENIMY3_HEIGHT+ENIMY3_INTERSPACE_VERTICAL))
					{
						for(i=ENIMY3_NUMBER/2;i<ENIMY3_NUMBER;i++)
						{
							if((playerRocket.x>=enimy3Data.x[i])&&(playerRocket.x<(enimy3Data.x[i]+ENIMY3_WIDTH)))
							{
								if(enimy3Data.score[i]!=0)
								{
									enimy3Data.score[i]--;
									playerRocket.workingFlag=0x00;
									enimy3Data.waitRef-=2;
								}
							}
						}
					}
				}
				if(playerRocket.workingFlag)
				{
					playerRocket.y--;
					mailBox[mailBoxNOM]=(((uint32_t)0b100)<<20)|(((uint32_t)playerRocket.x)<<8)|((uint32_t)playerRocket.y);
					mailBoxNOM++;
					playerRocket.wait=PLAYER_ROCKET_WAIT;
				}
			}
			else
			{
				playerRocket.workingFlag=0x00;
			}
		}
		else
		{
			playerRocket.wait--;
		}
	}
	if(SHOOT_STATE&&(currentPlayerY!=0)&&(!(playerRocket.workingFlag)))
	{
		playerRocket.workingFlag=0xff;
		playerRocket.x=currentPlayerX+(PLAYER_WIDTH/2);
		playerRocket.y=currentPlayerY;
		mailBox[mailBoxNOM]=(((uint32_t)0b100)<<20)|(((uint32_t)playerRocket.x)<<8)|((uint32_t)playerRocket.y);
		mailBoxNOM++;
		playerRocket.wait=PLAYER_ROCKET_WAIT;
	}
	if(enimy3Data.wait==0)
	{
		enimy3Data.wait=enimy3Data.waitRef;
		if(enimy3Data.index<enimy3Data.halfMaxIndex)
		{
			enimy3Data.index++;
			for(i=0;i<ENIMY3_NUMBER;i++)
			{
				if(enimy3Data.score[i]==0)
				{
					if(enimy3Data.fireFlag[i]==0xff)
					{
						mailBox[mailBoxNOM]=((uint32_t)(0b01<<23))|((uint32_t)(0b011<<20))|((uint32_t)(i<<16));
						mailBox[mailBoxNOM+1]=((uint32_t)(0b101<<20))|((uint32_t)(enimy3Data.x[i]<<8))|((uint32_t)enimy3Data.y[i]);
						mailBoxNOM+=2;
						enimy3Data.fireFlag[i]=0x55;
					}
					else if(enimy3Data.fireFlag[i]==0x55)
					{
						mailBox[mailBoxNOM]=((uint32_t)(0b01<<23))|((uint32_t)(0b101<<20))|((uint32_t)(enimy3Data.x[i]<<8))|((uint32_t)enimy3Data.y[i]);
						mailBoxNOM++;
						enimy3Data.fireFlag[i]=0x00;
					}
				}
				else
				{
					mailBox[mailBoxNOM]=((uint32_t)(0b10<<23))|((uint32_t)(0b011<<20))|((uint32_t)(i<<16))|((uint32_t)(1<<8));
					mailBoxNOM++;
					enimy3Data.x[i]++;
				}
			}
		}
		else if(enimy3Data.index<(2*enimy3Data.halfMaxIndex))
		{
			enimy3Data.index++;
			for(i=0;i<ENIMY3_NUMBER;i++)
			{
				if(enimy3Data.score[i]==0)
				{
					if(enimy3Data.fireFlag[i]==0xff)
					{
						mailBox[mailBoxNOM]=((uint32_t)(0b01<<23))|((uint32_t)(0b011<<20))|((uint32_t)(i<<16));
						mailBox[mailBoxNOM+1]=((uint32_t)(0b101<<20))|((uint32_t)(enimy3Data.x[i]<<8))|((uint32_t)enimy3Data.y[i]);
						mailBoxNOM+=2;
						enimy3Data.fireFlag[i]=0x55;
					}
					else if(enimy3Data.fireFlag[i]==0x55)
					{
						mailBox[mailBoxNOM]=((uint32_t)(0b01<<23))|((uint32_t)(0b101<<20))|((uint32_t)(enimy3Data.x[i]<<8))|((uint32_t)enimy3Data.y[i]);
						mailBoxNOM++;
						enimy3Data.fireFlag[i]=0x00;
					}
				}
				else
				{
					tempChar=-1;
					temp8Bit=*((uint8_t*)(&tempChar));
					temp32Bit=(uint32_t)temp8Bit;
					mailBox[mailBoxNOM]=((uint32_t)(0b10<<23))|((uint32_t)(0b011<<20))|((uint32_t)(i<<16))|(temp32Bit<<8);
					mailBoxNOM++;
					enimy3Data.x[i]--;
				}
			}
		}
		if(enimy3Data.index==(2*enimy3Data.halfMaxIndex))
		{
			enimy3Data.index=0;
		}
	}
	else
	{
		enimy3Data.wait--;
	}
}

uint8_t rand(uint8_t lim)
{
        static uint32_t a = 100001;
        a = (a * 125) % 2796203;
        return ((uint8_t)(a % lim));
}


