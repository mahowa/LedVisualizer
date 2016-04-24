

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f0xx.h"
#include "msgeq.h"
#include "Lights.h"
#include "delay.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define numReadings 25
#define GAIN_VALUE  8000
#define isSingle 1
#define START_INDEX 2
void single();
void spectrum();

volatile int readings[numReadings][7];      // the readings from the analog input
volatile int min[7];
volatile int max[7];
int readIndex = 0;              // the index of the current reading
volatile int total[7];                  // the running total
volatile int average[7];

int gain = GAIN_VALUE;
volatile double normal[7];
volatile int OPT = 0;
volatile int outputs[7];

int main(void){
	//Initiate lights
	INITI();
	delay_init();

	//Cool startup sequence
	RGB(255,255,255);
	delay_ms(500);

	RGB(255,0,0);		//RED
	delay_ms(500);

	RGB(0,255,0);		//GREEN
	delay_ms(500);

	RGB(0,0,255);		//BLUE
	delay_ms(500);




	for(int i = 0; i<numReadings; i++){
		for(int j=0; j<7; j++){
			readings[i][j] = 0;
		}
	}

	for(int j=0; j<7; j++){
		min[j] = 0;
		max[j] = 0;
		total[j] = 0;
		average[j] = 0;
		outputs[j] = 0;
		normal[j] = 0;
	}

///////////////START
	RGB(0,50,0);	//Set base color


	//Set starting frequency
	for(int i=0; i<START_INDEX; i++){
			GPIOB->ODR &= ~(GPIO_ODR_12);
			delay_ms(1);
			GPIOB->ODR |= GPIO_ODR_12;
			delay_ms(1);
			}

	while(1){
		if(isSingle)
			single();
		else
			spectrum();
	}
}

void single(){
	int OPT;

	// subtract the last reading:
	total[0] = total[0] - readings[readIndex][0];

	readings[readIndex][0] = ADC1->DR;


	if (max[0] < readings[readIndex][0]){
		max[0] = readings[readIndex][0];
	}
//			if (min[i] > readings[readIndex][i]){
//			  min[i] = 0;
//			}
	total[0] += readings[readIndex][0];
	average[0] = total[0]/numReadings;
	normal[0] = ((double)average[0])/((double)max[0]);
	OPT = normal[0]*gain;

	int final = (18000 - OPT);
	if(final >18000)
		final = 18000;
	if(final<0)
		final = 0;

	outputs[0] = final;
	delay_ms(1);


	TIM3_PWM_RED(outputs[0]);




	readIndex = readIndex + 1;
	if (readIndex >= numReadings) {
	  readIndex = 0;
	}


}

void spectrum(){
	int OPT;
	for(int i =0; i<7; i++){
				// subtract the last reading:
				total[i] = total[i] - readings[readIndex][i];

				GPIOB->ODR &= ~(GPIO_ODR_12);
				delay_ms(1);
				readings[readIndex][i] = ADC1->DR;
				GPIOB->ODR |= GPIO_ODR_12;

				if (max[i] < readings[readIndex][i]){
					max[i] = readings[readIndex][i];
				}
	//			if (min[i] > readings[readIndex][i]){
	//			  min[i] = 0;
	//			}
				total[i] += readings[readIndex][i];
				average[i] = total[i]/numReadings;
				normal[i] = ((double)average[i])/((double)max[i]);
				OPT = normal[i]*gain;

				int final = (18000 - OPT);
				if(final >18000)
					final = 18000;
				if(final<0)
					final = 0;

				outputs[i] = final;
				delay_ms(1);

				if(i==0){
					TIM3_PWM_RED(outputs[0]);
					break;
				}
			}

			readIndex = readIndex + 1;
			if (readIndex >= numReadings) {
			  readIndex = 0;
			}

	//		TIM3_PWM_RED(outputs[0]);
			//TIM3_PWM_GREEN(outputs[3]);
			//TIM3_PWM_BLUE(outputs[6]);


}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
