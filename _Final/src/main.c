#define stub (uint16_t)0x0000; // my stub. You should not end up with any of these!

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f0xx.h"


// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/*=====================================================================================
	Private Functions
=======================================================================================*/

/*
 * Initializes PC8 in Output Mode
 */
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
void timer_init(){							//Setup the Music Read time
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		//Connect TIM2 to the appropriate bus

	//Highest frequency of music is 4000Hz
	TIM2->PSC = 470;						//set to 1 Hz
	TIM2->ARR = 10;						//Double the frequency to get an interrupt ever 8000 cycles

	TIM2->DIER |= TIM_DIER_UIE;				//Enable UEV interrupt
	TIM2->CR1 |= TIM_CR1_CEN;				//Enable timer
	NVIC_EnableIRQ(TIM2_IRQn);				//Interrupt
}
void LED_BLUE_init(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 				// Enable peripheral clock to GPIOC
	GPIOC->MODER |= GPIO_MODER_MODER8_0; 			// Set PC8 to output
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8);			// Set to push-pull output type
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR8_0		// Set to low speed
		| GPIO_OSPEEDR_OSPEEDR8_1);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8_0 			// Set to no pull-up/down
		| GPIO_PUPDR_PUPDR8_1);
	GPIOC->ODR &= ~(GPIO_ODR_8);					// Shut off LED
}


/*
 * Initializes PC9 in Output Mode
 */
void LED_GREEN_init(){

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 				// Enable peripheral clock to GPIOC
	GPIOC->MODER |= GPIO_MODER_MODER9_0; 			// Set PC9 to output
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_9);			// Set to push-pull output type
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR9_0		// Set to low speed
		| GPIO_OSPEEDR_OSPEEDR9_1);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR9_0 			// Set to no pull-up/down
		| GPIO_PUPDR_PUPDR9_1);
	GPIOC->ODR &= ~(GPIO_ODR_9);					// Shut off LED
}


/*
 * Initializes PC8 in Alternate Function Mode
 *
 * Table 13 in the STM32F0-Discovery Datasheet
 * shows PC8 having one alternative function
 * attached to TIM3 Channel 3
 */
void LED_BLUE_AF_init(){

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 				// Enable peripheral clock to GPIOC
	GPIOC->MODER |= GPIO_MODER_MODER8_1; 			// Set PC8 to AF mode
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8);			// Set to push-pull output type
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR8_0 	// Set to low speed
		| GPIO_OSPEEDR_OSPEEDR8_1);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8_0 			// Set to no pull-up/down
		| GPIO_PUPDR_PUPDR8_1);
	GPIOC->AFR[1] &= ~(GPIO_AFRH_AFR8);				//alternate function 0 on pin 8
}


/*
 *  Initializes PC8 in Alternate Function Mode
 *
 * Table 13 in the STM32F0-Discovery Datasheet
 * shows PC9 having one alternative function
 * attached to TIM3 Channel 4
 */
void LED_GREEN_AF_init(){

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 				// Enable peripheral clock to GPIOC
	GPIOC->MODER |= GPIO_MODER_MODER9_1; 			// Set PC8 to AF mode
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_9);			// Set to push-pull output type
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR9_0 	// Set to low speed
		| GPIO_OSPEEDR_OSPEEDR9_1);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR9_0 			// Set to no pull-up/down
		| GPIO_PUPDR_PUPDR9_1);
	GPIOC->AFR[1] &= ~(GPIO_AFRH_AFR9);				//alternate function 0 on pin 9
}


/*
 * A basic general purpose timer that is capable
 * of throwing an interrupt every millisecond.
 *
 * We'll be using TIM2 for this timer. Note that
 * TIM2 has a 32 bit counter, so would not normally
 * be used for something as simple as this, but it
 * suits our purposes.
 */
//void millis_timer_init(){
//
///* TODO: Connect TIM2 to the appropriate bus */
//	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
//
///* TODO: Set ARR and PSC registers */
//	TIM2->PSC = 47;			//set to 1 MHz
//	TIM2->ARR = 1000000;	//1000000 cycles per interrrpt
//							//1 interrrupt per second
//
///* TODO: Enable UEV interrupt */
//	TIM2->DIER |= TIM_DIER_UIE;
//
///* TODO: Enable timer */
//	TIM2->CR1 |= TIM_CR1_CEN;
//
///* TODO: Finally, lets enable the TIM2 IRQ before we exit */
//	//NVIC_SetPriority(TIM2_IRQn, 1);
//	NVIC_EnableIRQ(TIM2_IRQn);
//}


/*
 * TODO: TIM2 ISR
 *
 * Be sure to clear the update interrupt flag.
 * It's located in the Status Register (SR)
 */
//void TIM2_IRQHandler(void){
//	TIM2->SR &= ~(TIM_SR_UIF);		//Clear interrupt bit
//	GPIOC->ODR ^= (GPIO_ODR_8);		// Toggle LED
//}

/*
 * Enable TIM3 in PWM mode. Timer 3 has 4 usable Channels.
 * If we look at the STM32F0-Discovery Datasheet
 * at Table 13 we'll see that Channel 3 is connected to PC8
 * and Channel 4 is connected to PC9. You're free to connect
 * whatever LED you'd like to this PWM signal, but note that
 * the bits you manipulate here will need to reflect the channel
 * you are using.
 *
 * PC8: TIM3 CH3
 * PC9: TIM3 CH4
 */
void TIM3_PWM_mode(int pulse){

/* 	TODO: Connect TIM3 to the appropriate bus */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

/*	TODO: Set the appropriate time base registers
		  like you did for TIM2
*/
	TIM3->PSC = 47;			//set to 1 MHz
	TIM3->ARR = 1000000;	//1000000 cycles per interrrpt
							//1 interrrupt per second


/* Create a new OCInitTypeDef object.

	The TIM_OCInitTypeDef struct is used
	as a parameter to the TIM_OCxInit()
	functions. Every member needs a value
	before it can be sent through the function. */
	TIM_OCInitTypeDef  TIM_OCInitStructure;


/*	TODO: Fill in the values of the members in the
	 	  TIM_OCINitTypeDef struct. Looking at the
	   	  comments for this struct in the stm32f0xx_tim.h
		  file will be very helpful. I've filled in several
		  of the less useful members for you.

		  By defining these members we are able to instantiate
		  PWM on multiple channels with minimal effort. If you
		  look at the TIM_OCxInit() functions in the
		  stm32f0xx_tim.c file, you'll notice that the bits for
		  some of the registers are shifted in reference to
		  the channel you'd like to configure. This is a very
		  useful feature.
 */

/*	The type of output compare mode. This member is analagous
	to Bits [6:4] in the CCMR2. We'd like to use
	PWM Mode 1 here. */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	/* TODO */

/*	Bits for CCER. The Output Compare signal is output on the
 	corresponding output pin, but is shifted for you in the
 	initiation function. Check out the function if you need
 	a better idea. */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* TODO */

/*	Maps to CCRx. This is the value that will control your
	duty cycle in relation to ARR and PSC */
	TIM_OCInitStructure.TIM_Pulse = pulse;	/* TODO */


/*	Bit 2 in CCER. Reserved, always read as zero. */
	TIM_OCInitStructure.TIM_OutputNState = (uint16_t) 0;

/* 	Output polarity- We want an active low polarity.
	You don't need to do anything here. The value will be
	shifted left accordingly when you call the initiation function */
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

/* 	This bit is only used in TIM1 - just keep it zero */
	TIM_OCInitStructure.TIM_OCNPolarity = (uint16_t) 0;

/*	This bit is only used in TIM1 - just keep it zero */
	TIM_OCInitStructure.TIM_OCIdleState = (uint16_t) 0;

/* 	This bit is only used in TIM1 - just keep it zero */
	TIM_OCInitStructure.TIM_OCNIdleState = (uint16_t) 0;

/* 	TODO: Call the corresponding TIM_OCxInit() function.
   	   	  Remember to include all parameters and to
   	   	  dereference your struct pointer parameter. */
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);


/*	TODO: Finally let's enable the TIM3 counter */
	TIM3->CR1 |= TIM_CR1_CEN;


}

volatile int read_count;
const int numReadings = 30;     // Averaging window size
int readings[30];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;
int maxx = 0;                   // Find minn/maxx to normalize input
int minn = 0;
int gain = 1000;
double normal = 0;
const int NORMAL = 55;           //Gain for the normal paramater
int OPT = 0;


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




	TIM3_PWM_mode(15990-(dump)); //15990 = OFF
	TIM2->SR &= ~TIM_SR_UIF;
}

/*=====================================================================================
	Main Entry Point to Program
=======================================================================================*/
int main(void){
	for(int i = 0; i<numReadings; i++){
		readings[i] = 0;
	}

	LED_BLUE_init();
	LED_GREEN_AF_init();
	ADC_init();
	timer_init();
	/* TODO: For the first part of lab we'll blink an LED at 1Hz. You can choose either LED.
	 * Set up Timer 2 to be a millisecond timer. The outline to the function is
	 * described above. You might need another variable floating around your program
	 * to keep track of elapsed time.
	 */
	//millis_timer_init();


	/* TODO: The next part of the lab asks you to instantiate a PWM signal and use it to control
	 * the brightness of the other LED. You'll have to fill in some functions above. Because the LED
	 * is now attached to Timer 3, we'll have to configure the pin in Alternate Function mode
	 */

	//TIM3_PWM_mode(599);

	/* TODO: For the final part of lab we'll demonstrate the PWM signal by pulsing the LED.
	 * You can do it anyway you want, just have it pulse on and off indefinitely.
	 */

	int count =0;
	int dir = 1;
	volatile int delay;
	while(1){
		//for(delay=0;delay<500;delay++);
//		if(dir ==1){
//			if(count<15990)
//				count++;
//			else
//				dir =2;
//
//		}
//		else{
//			if(count>0)
//				count--;
//			else{
//				dir =1;
//			}
//		}
//		//TIM3_PWM_mode(count);

	} // loop indefinitely
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
