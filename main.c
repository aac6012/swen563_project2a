
#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "state_machine.h"
#include "globals.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>

//Definition of recipes
//unsigned char recipe1[] = { MOV + 3, MOV | 5, RECIPE_END } ;
unsigned char recipe1[] = { LOOP | 2, MOV | 5, WAIT | 30, MOV | 1, END_LOOP | 0, RECIPE_END } ;
unsigned char recipe2[] = { LOOP | 2, MOV | 5, WAIT | 20, MOV | 3, WAIT | 20, MOV | 1, WAIT | 20, END_LOOP, RECIPE_END } ;

// Servo structs
volatile Servo servo1_obj = { TIM2, 0, 0, 0, NOT_IN_LOOP, state_position_2, status_paused, recipe1 } ;
volatile Servo *servo1 = &servo1_obj ;
volatile Servo servo2_obj = { TIM5, 0, 0, 0, NOT_IN_LOOP, state_position_2, status_paused, recipe2 } ;
volatile Servo *servo2 = &servo2_obj ;

// UART output messages
char prompt[] = "Enter Command >" ;

// UART output buffer
uint8_t buffer[BufferSize];

// Input string from user through UART (two characters max)
char inputString[2] ;

void servo_timers_init() {
	
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
	TIM2->PSC =  4000;
	
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
	TIM2->CCR1 = DUTY_CYCLE * (0.06) ;
	
	/**
	* GPIO PA1 / TIM5 setup
	*/
	// Set GPIO PA1 mode to Alternate Function
	GPIOA->MODER &= ~(0x03 << 2) ;
	GPIOA->MODER |= (0x02 << 2) ;
	
	// Set GPIO A pin1 to alternate function to AF2
	//GPIOA->AFR[0] &= ~(0x0F << 4) ;
	GPIOA->AFR[0] |= (0x02 << 4) ;
	
	// Enable Timer 5 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	
	// Max count
	TIM5->ARR = DUTY_CYCLE ;
	
	// Set the timer prescalar value
	// This value will set the PWM to 20ms periods
	TIM5->PSC =  4000; 
	
	// Load new prescalar value by forcing update event.
	TIM5->EGR |= TIM_EGR_UG;
	
	// Set the mode of the Timer
	TIM5->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 ;
	TIM5->CCMR1 |= TIM_CCMR1_OC2PE ;
	
	TIM5->CR1 |= TIM_CR1_ARPE ;
	
	// Enable input capture
	TIM5->CCER |= TIM_CCER_CC2E ;
	
	// Clear update flag
	TIM5->SR &= ~TIM_SR_UIF ;
	
	TIM5->DIER |= TIM_DIER_UIE ;
	TIM5->CR1 |= TIM_CR1_CEN ;
	
	//Initialize with 2% duty cycle
	TIM5->CCR2 = DUTY_CYCLE * (0.06) ;
	
}

void init_master_timer(){
	// Master timer controls timing of recipe execution (every 100ms)
	
	// Enable Timer 3 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	
	// Set prescalar to ?
	//TIM3->PSC = 20000 ;
	TIM3->PSC = 20000 ;
	
	// Load new prescalar value by forcing update event.
	TIM3->EGR |= TIM_EGR_UG;
	
	// Set auto reload value
	//TIM3->ARR = 100 ;
	TIM3->ARR = 200 ;
	
	// Enable timer interrupts
	TIM3->DIER = TIM_DIER_UIE ;
	
	// Enable timer
	TIM3->CR1 = TIM_CR1_CEN ;
	
	// Enable interrupt in NVIC
	NVIC_EnableIRQ(TIM3_IRQn) ;
	
}

// Timer 3 interrupt handler
void TIM3_IRQHandler(void) {
	unsigned char opcode, param ;
	enum recipe_status prev_status ; // used to tell if state changes in interrupt.
	
	// Check if interrupt flag is set
	if(TIM3->SR & TIM_SR_UIF) { 
		// Clear interrupt flag
		TIM3->SR &= ~TIM_SR_UIF ;
		
		prev_status = servo1->status ;
		
		// Handle user input
		if( inputString[0] != 0x00 ){
			process_user_event( servo1, input_char_to_event(inputString[0]) ) ;
			inputString[0] = 0x00 ;
		}
		if( inputString[1] != 0x00 ){
			process_user_event( servo2, input_char_to_event(inputString[1]) ) ;
			inputString[1] = 0x00 ;
		}
		
		
		// Decrement delay counters, if necessary
		if( servo1->delay_counter > 0 ){
			servo1->delay_counter-- ;
		}
		if( servo2->delay_counter > 0 ){
			servo2->delay_counter-- ;
		}
		
		
		// Process next instruction for each servo, if ready
		if( servo1->status != status_paused && servo1->delay_counter == 0 && servo1->status != status_ended){
			opcode = servo1->recipe[servo1->recipe_index] & OPCODE_MASK ;
			param = servo1->recipe[servo1->recipe_index] & PARAM_MASK ;
			process_instruction( servo1, opcode, param ) ;
		}
		if( servo2->status != status_paused && servo2->delay_counter == 0 ){
			opcode = servo2->recipe[servo2->recipe_index] & OPCODE_MASK ;
			param = servo2->recipe[servo2->recipe_index] & PARAM_MASK ;
			process_instruction( servo2, opcode, param ) ;
		}
		
		
		//Only change LEDs if state has changed.
		if(prev_status != servo1->status){
			// Handle LED colors for servo1 status
			switch( servo1->status ){
				case status_running:
					Red_LED_Off() ;
					Green_LED_On() ;
					break ;
				case status_paused:
					Red_LED_Off() ;
					Green_LED_Off() ;
					break ;
				case status_ended:
					Red_LED_Off() ;
					Green_LED_Off() ;
					break ;
				case status_command_error:
					Red_LED_On() ;
					Green_LED_Off() ;
					break ;
				case status_nested_error:
					Red_LED_On() ;
					Green_LED_On() ;
					break ;
			}		
		}
		
	}
}



// A simple main that just prints out the hex value of the first entry in each recipe.
int main() {
	
	char rxByte ;
	char tempInputBuffer[100] ;
	int inputBuffIndex ;
	
	
	//Initialize system clock to 80MHz
	System_Clock_Init() ;
	
	/*
	//Initialize servo structs
	init_servo(servo1) ;
	init_servo(servo2) ;
	*/
	
	//Initialize servo timers
	servo_timers_init() ;
	
	LED_Init( );
	UART2_Init( );
	
	//Initialize master timer (100ms period)
	init_master_timer() ;
	
	while(1){
		
		//Write prompt for user input to terminal
		USART_Write( USART2, (uint8_t *)prompt, strlen( prompt ) ) ;
		
		rxByte = 0x00 ;
		inputBuffIndex = 0 ;
		while(rxByte != 0x0D) { // Wait until user presses enter
			rxByte = USART_Read( USART2 ) ;
			USART_Write(USART2, (uint8_t *)&rxByte, 1 ) ;
			tempInputBuffer[inputBuffIndex] = rxByte ;
			inputBuffIndex++ ;
		}
		
		// Set global input string to first 2 chars of temp buffer
		for(inputBuffIndex = 0; inputBuffIndex < 2; inputBuffIndex++){
			inputString[inputBuffIndex] = tempInputBuffer[inputBuffIndex] ;
			tempInputBuffer[inputBuffIndex] = 0x00 ; // reset to null char
		}
		
		// Write new line to terminal
		USART_Write( USART2, (uint8_t *)"\r\n", 2 );
		
		// Reset temp char
		rxByte = 0x00 ;
		
	}
	
	
}
