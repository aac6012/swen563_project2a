// L. Kiser Feb. 16, 2017
// Some ideas for writing the SWEN 563 recipe related code.

#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "state_machine.h"
#include "globals.h"

#include <stdio.h>
#include <stdlib.h>

// Examples of simple recipes
// Note that, because the bottom 5 bits are zeros adding or bitwise or'ing
// in the values for the bottom 5 bits are equivalent. However, using a bitwise
// or is better at communicating your purpose.
//unsigned char recipe1[] = { MOV + 3, MOV | 5, RECIPE_END } ;
unsigned char recipe1[] = { LOOP | 5, MOV + 5, MOV + 0, END_LOOP, RECIPE_END } ;
unsigned char recipe2[] = { MOV | 5, MOV | 2, RECIPE_END } ;

// If you set up an array like this then you can easily switch recipes
// using an additional user input command.
unsigned char *recipes[] = { recipe1, recipe2, NULL } ;

void timer_init() {
	
	// Enable GPIO A clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;
	
	
	/**
	* GPIO PA0 / TIM2 setup
	*/
	// Set GPIO PA0 mode to Alternate Function
	GPIOA->MODER &= ~(0x03) ;
	GPIOA->MODER |= 0x02 ;
	
	// Set GPIO A pin0 alternate function to AF1
	GPIOA->AFR[0] |= 0x01;
	
	// Enable Timer 2 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	// Max count
	TIM2->ARR = DUTY_CYCLE ;
	
	// Set the timer prescalar value
	// This value will set the PWM to 20ms periods
	TIM2->PSC =  200; //Clock frequencies in MHz
	
	// Load new prescalar value by forcing update event.
	TIM2->EGR |= TIM_EGR_UG;
	
	// Set the input mode of the Timer (Input, CC1 is mapped to timer input 1)
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 ;
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE ;
	
	TIM2->CR1 |= TIM_CR1_ARPE ;
	
	// Enable input capture
	TIM2->CCER |= TIM_CCER_CC1E ;
	
	// Clear update flag
	TIM2->SR &= ~TIM_SR_UIF ;
	
	TIM2->DIER |= TIM_DIER_UIE ;
	TIM2->CR1 |= TIM_CR1_CEN ;
	
	//Initialize with 2% duty cycle
	TIM2->CCR1 = DUTY_CYCLE * (0.02) ;
	
	
	/**
	* GPIO PA1 / TIM5 setup
	*/
	// Set GPIO PA1 mode to Alternate Function
	GPIOA->MODER &= ~(0x03 << 2) ;
	GPIOA->MODER |= (0x02 << 2) ;
	
	// Set GPIO A pin1 to alternate function to AF2
	GPIOA->AFR[1] |= 0x02 ;
	
	// Enable Timer 5 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	
	// Max count
	TIM5->ARR = DUTY_CYCLE ;
	
	// Set the timer prescalar value
	// This value will set the PWM to 20ms periods
	TIM5->PSC =  200; //Clock frequencies in MHz
	
	// Load new prescalar value by forcing update event.
	TIM5->EGR |= TIM_EGR_UG;
	
	// Set the input mode of the Timer (Input, CC1 is mapped to timer input 1)
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 ;
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE ;
	
	TIM5->CR1 |= TIM_CR1_ARPE ;
	
	// Enable input capture
	TIM5->CCER |= TIM_CCER_CC1E ;
	
	// Clear update flag
	TIM5->SR &= ~TIM_SR_UIF ;
	
	TIM5->DIER |= TIM_DIER_UIE ;
	TIM5->CR1 |= TIM_CR1_CEN ;
	
	//Initialize with 2% duty cycle
	TIM5->CCR1 = DUTY_CYCLE * (0.02) ;
	
}



// A simple main that just prints out the hex value of the first entry in each recipe.
int main() {
	
	int index = 0 ;
	
	timer_init() ;
	Servo *servo1, *servo2 ;
	
	
	
	LED_Init( );
	UART2_Init( );
	
	while( recipe1[index] != RECIPE_END ) {
		
		// Extract op-code and parameter from instruction
		unsigned char opcode = recipe1[index] & OPCODE_MASK ;
		unsigned char param = recipe1[index] & PARAM_MASK ;
		
		index = process_instruction( opcode, param, index ) ;
		
	}
	
	
}
