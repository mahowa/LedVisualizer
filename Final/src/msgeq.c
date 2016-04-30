/*
 * msgeq.c
 *
 *  Created on: Apr 22, 2016
 *      Author: mahowa
 */

#include "stm32f0xx.h"


void MSGEQ_StrobeBtn_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER12_0 |GPIO_MODER_MODER11_0;
	//GPIOB->PUPDR |= 3<<23 | 3<<25;

	//Set both pins to Low
	GPIOB->ODR &= ~(GPIO_ODR_12);
	GPIOB->ODR &= ~(GPIO_ODR_11);

	//Set Reset to High
	volatile int i = 0;
	for(i;i<1000000; i++);
	GPIOB->ODR |= GPIO_ODR_11;

	//Set Reset to Low
	i = 0;
	for(i;i<1000000; i++);
	GPIOB->ODR &= ~(GPIO_ODR_11);

	//Set strobe to High
	//GPIOB->ODR |= GPIO_ODR_12;
}

void MSGEQ_Strobe(){
	GPIOB->ODR |= (GPIO_ODR_12);
	volatile int i = 0;
	for(i;i<10000;i++);

	GPIOB->ODR &= ~GPIO_ODR_12;
}

void MSGEQ_Reset(){
	GPIOB->ODR |= GPIO_ODR_11;
	volatile int i = 0;
	for(i;i<10000;i++);
	GPIOB->ODR &= ~(GPIO_ODR_11);
}

void MSGEQ_Clock_init(){

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER 	|= GPIO_MODER_MODER7_1;
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_7);
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR7_0 | GPIO_OSPEEDR_OSPEEDR7_1);
	GPIOA->PUPDR |= ~(GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR7_1);
	GPIOA->AFR[0] |= 1<<30;

	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

	TIM14->PSC = 23;
	TIM14->ARR = 12;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 6;
	TIM_OCInitStructure.TIM_OutputNState = (uint16_t) 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = (uint16_t) 0;
	TIM_OCInitStructure.TIM_OCIdleState = (uint16_t) 0;
	TIM_OCInitStructure.TIM_OCNIdleState = (uint16_t) 0;
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	TIM14->CR1 |= TIM_CR1_CEN;
}
