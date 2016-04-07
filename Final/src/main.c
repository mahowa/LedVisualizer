//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f0xx.h"
#include "main.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//Setup the ADC
void ADC_init() {
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

//Setup the Music Read time
void timer_init(){
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		//Connect TIM2 to the appropriate bus

	//Highest frequency of music is 4000Hz
	TIM2->PSC = 470;						//set to 1 Hz
	TIM2->ARR = 8000;						//Double the frequency to get an interrupt ever 8000 cycles

	TIM2->DIER |= TIM_DIER_UIE;				//Enable UEV interrupt
	TIM2->CR1 |= TIM_CR1_CEN;				//Enable timer
	NVIC_EnableIRQ(TIM2_IRQn);				//Interrupt
}

volatile int read_count;
volatile int music[10];
volatile int output;
void TIM2_IRQHandler(){
	if(read_count<10){
		music[read_count]= ADC1->DR;	//Add the adc value to
		read_count++;
	}
	else{
		read_count = 0;					//Reset the counter
		int sum = 0;
		for(int i=0; i<10;i++){
			sum += music[i];
		}
		output = (sum/10);
	}
}
void pwm_out(){
	//TODO Output
}

int main(int argc, char* argv[]){
	read_count = 0;
	ADC_init();
	timer_init();


  // Infinite loop
  while (1)
    {
       // Add your code here.
    }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
