/*
 * Lights.c
 *
 *  Created on: Apr 19, 2016
 *      Author: mahowa
 */
#include "stm32f0xx.h"
#include "Lights.h"




volatile int read_count;
const int numReadings = 30;     // Averaging window size
int readings[30];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;
int maxx = 0;                   // Find minn/maxx to normalize input
int minn = 0;
int gain = 100000;
double normal = 0;
const int NORMAL = 55;           //Gain for the normal paramater
int OPT = 0;


void INITI(){
	for(int i = 0; i<numReadings; i++){
		readings[i] = 0;
	}

	LIGHTS_AF_init();
	ADC_init();
	timer_init();
}


void TIM2_IRQHandler(){
	  // subtract the last reading:
	  total = total - readings[readIndex];
	  // read from the sensor:
	  readings[readIndex] = ADC1->DR;;
	  // add the reading to the total:

	  if (maxx < readings[readIndex]){
		  maxx = readings[readIndex];
	  }
	  if (minn > readings[readIndex]){
		  minn = 0;
	  }


	  total = total + readings[readIndex];
	  // advance to the next position in the array:
	  readIndex = readIndex + 1;

	  // if we're at the end of the array...
	  if (readIndex >= numReadings) {
		  // ...wrap around to the beginning:
		  readIndex = 0;
	  }

	  // calculate the average and applies gain
	  average = (total) / numReadings;

	normal = (double)average/((double)maxx );
	OPT = normal*gain;
	volatile int dump = (int) OPT;




	TIM3_PWM_mode(17000-dump); //17000 = OFF
	TIM2->SR &= ~TIM_SR_UIF;
}



void ADC_init() {							//Setup the ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		//Enable ADC
	GPIOA->MODER |= GPIO_MODER_MODER1_1;	//Place pin A1 in analog mode

	ADC1->CFGR1 |= ADC_CFGR1_RES_1;			//set to 8 bit or bits:10
	ADC1->CFGR1 |= ADC_CFGR1_CONT;			//Continuous
	ADC1->CFGR2 |= ADC_CFGR2_CKMODE;		//Asyncronous

	ADC1->CR |= ADC_CR_ADCAL;				// Start an ADC self-calibration cycle
	while((ADC1->CR & ADC_CR_ADCAL) != 0)	//wait until it's complete
	{
		//Wait
	}

	/// TODO: Enable the ADC and wait until it's ready to run


	ADC1->CHSELR |= ADC_CHSELR_CHSEL1;		//Set the ADC channel

	ADC1->CR |= ADC_CR_ADEN;				//Enable
	while((ADC1->ISR & ADC_ISR_ADRDY) != 0)//Wait unit ready
	{
		//Wait
	}

	ADC1->CR |= ADC_CR_ADSTART;				//Signal/Trigger the start of the continuous conversion
}

void TIM3_PWM_mode(int pulse){
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 47;			//set to 1 MHz
	TIM3->ARR = 1000000;	//1000000 cycles per interrrpt
							//1 interrrupt per second

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OutputNState = (uint16_t) 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = (uint16_t) 0;
	TIM_OCInitStructure.TIM_OCIdleState = (uint16_t) 0;
	TIM_OCInitStructure.TIM_OCNIdleState = (uint16_t) 0;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM3->CR1 |= TIM_CR1_CEN;
}


void timer_init(){							//Setup the Music Read time
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		//Connect TIM2 to the appropriate bus

	//Highest frequency of music is 4000Hz
	TIM2->PSC = 470;						//set to 1 Hz
	TIM2->ARR = 10;						//Double the frequency to get an interrupt ever 8000 cycles

	TIM2->DIER |= TIM_DIER_UIE;				//Enable UEV interrupt
	TIM2->CR1 |= TIM_CR1_CEN;				//Enable timer
	NVIC_EnableIRQ(TIM2_IRQn);				//Interrupt
}


void LIGHTS_AF_init(){

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 				// Enable peripheral clock to GPIOC
	GPIOC->MODER |= GPIO_MODER_MODER9_1; 			// Set PC8 to AF mode
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_9);			// Set to push-pull output type
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR9_0 	// Set to low speed
		| GPIO_OSPEEDR_OSPEEDR9_1);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR9_0 			// Set to no pull-up/down
		| GPIO_PUPDR_PUPDR9_1);
	GPIOC->AFR[1] &= ~(GPIO_AFRH_AFR9);				//alternate function 0 on pin 9
}



