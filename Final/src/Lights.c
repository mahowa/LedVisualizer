/*
 * Lights.c
 *
 *  Created on: Apr 19, 2016
 *      Author: mahowa
 */
//TODO NOTES


	//Look at timer_init() and clock
	//PWM on PC8, PC7 (PC9 Works...)


#include "stm32f0xx.h"
#include "Lights.h"
#include "msgeq.h"

//0-255
//Maps Red, Green, Blue 0-255 to the pwm output necissary to
//Generate the collor
void  RGB(int R, int G, int B){
	int red = Map(R);
	int green = Map(G);
	int blue = Map(B);

	TIM3_PWM_RED(red);
	TIM3_PWM_GREEN(green);
	TIM3_PWM_BLUE(blue);
}

//Returns 0-255 mapped to 65535
int Map(int value){
	double percentage = (double)value/255.0;
	int mVal = 17000*percentage;
	return 17000 - mVal;
}


void INITI(){
	LIGHTS_AF_init();
	ADC_init();
	MSGEQ_Clock_init();
	MSGEQ_StrobeBtn_init();
	btn_init();
}

void EXTI0_1_IRQHandler(void){
	MSGEQ_Strobe();
	EXTI->PR |= 1;
}
void EXTI4_15_IRQHandler(){
	MSGEQ_Reset();
	EXTI->PR |=(1<<8);
}

void btn_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	//Make buttons pull down
	GPIOB->PUPDR |= 1<<1 |1<<13;

	EXTI->RTSR |=(1 | 1<<8);		//Rising Clock edge
	EXTI->IMR |= (1 | 1<<8);		//Input register

	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_SetPriority(EXTI0_1_IRQn,1);
	NVIC_EnableIRQ(EXTI4_15_IRQn);		//Interrupt handler set
	NVIC_SetPriority(EXTI4_15_IRQn,1);	//Set priority of the interrupt
}




void ADC_init() {							//Setup the ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		//Enable ADC
	GPIOA->MODER |= GPIO_MODER_MODER1_1;	//Place pin A1 in analog mode

	ADC1->CFGR1 |= ADC_CFGR1_RES_1;			//set to 8 bit or bits:10
	ADC1->CFGR1 |= ADC_CFGR1_CONT;			//Continuous
	ADC1->CFGR2 |= ADC_CFGR2_CKMODE;		//Asyncronous

	ADC1->CR |= ADC_CR_ADCAL;				// Start an ADC self-calibration cycle
	while((ADC1->CR & ADC_CR_ADCAL) != 0){}	//wait until it's complete



	ADC1->CHSELR |= ADC_CHSELR_CHSEL1;		//Set the ADC channel

	ADC1->CR |= ADC_CR_ADEN;				//Enable
	while((ADC1->ISR & ADC_ISR_ADRDY) != 0){}//Wait unit ready


	ADC1->CR |= ADC_CR_ADSTART;				//Signal/Trigger the start of the continuous conversion
}




void LIGHTS_AF_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 				// Enable peripheral clock to GPIOC

	GPIOC->MODER 	|= GPIO_MODER_MODER9_1
					| GPIO_MODER_MODER8_1
					| GPIO_MODER_MODER7_1; 			// Set PC8 to AF mode

	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_9|GPIO_OTYPER_OT_8|GPIO_OTYPER_OT_7);			// Set to push-pull output type

	GPIOC->OSPEEDR &= ~((GPIO_OSPEEDR_OSPEEDR9_0 	// Set to low speed
							| GPIO_OSPEEDR_OSPEEDR9_1)
						|(GPIO_OSPEEDR_OSPEEDR8_0
							| GPIO_OSPEEDR_OSPEEDR8_1)
						|(GPIO_OSPEEDR_OSPEEDR7_0
							| GPIO_OSPEEDR_OSPEEDR7_1));

	GPIOC->PUPDR &= ~((GPIO_PUPDR_PUPDR9_0 			// Set to no pull-up/down
						| GPIO_PUPDR_PUPDR9_1)
					|(GPIO_PUPDR_PUPDR8_0
						| GPIO_PUPDR_PUPDR8_1)
					|(GPIO_PUPDR_PUPDR7_0
						| GPIO_PUPDR_PUPDR7_1));

	GPIOC->AFR[1] &= ~(	GPIO_AFRH_AFR9
						|GPIO_AFRH_AFR8
						|GPIO_AFRL_AFR7);				//alternate function 0 on pin 9
}

void TIM3_PWM_RED(int pulse){
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 47;				//set to 1 MHz
	TIM3->ARR = 1000000;		//1000000 cycles per interrrpt
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


void TIM3_PWM_GREEN(int pulse){
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 47;				//set to 1 MHz
	TIM3->ARR = 1000000;		//1000000 cycles per interrrpt
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
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_PWM_BLUE(int pulse){
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 47;				//set to 1 MHz
	TIM3->ARR = 1000000;		//1000000 cycles per interrrpt
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
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM3->CR1 |= TIM_CR1_CEN;
}

