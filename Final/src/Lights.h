/*
 * Lights.h
 *
 *  Created on: Apr 19, 2016
 *      Author: mahowa
 */

#ifndef LIGHTS_H_
#define LIGHTS_H_

volatile int strobeIndex;

void TIM3_PWM_RED(int pulse);
void TIM3_PWM_GREEN(int pulse);
void TIM3_PWM_BLUE(int pulse);

void ADC_init();
void LIGHTS_AF_init();
void INITI();
void btn_init();

void  RGB(int R, int G, int B);
int Map(int value);

#endif /* LIGHTS_H_ */
