/*
 * Lights.h
 *
 *  Created on: Apr 19, 2016
 *      Author: mahowa
 */

#ifndef LIGHTS_H_
#define LIGHTS_H_

void TIM2_IRQHandler();
void ADC_init();
void TIM3_PWM_P9(int pulse);
void TIM3_PWM_P8(int pulse);
void TIM3_PWM_P7(int pulse);
void timer_init();
void LIGHTS_AF_init();
void INITI();
#endif /* LIGHTS_H_ */
