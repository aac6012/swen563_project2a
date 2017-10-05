
#include <stdlib.h>

#include "state_machine.h"
#include "stm32l476xx.h"

// Define all of the commands that are valid
#define MOV (0x20)
#define WAIT (0x40)
#define LOOP (0x80)
#define END_LOOP (0xA0)
#define RECIPE_END (0x00)

#define DUTY_CYCLE 414
#define NOT_IN_LOOP 32

// Define a "global" state value that is only accessible in one .c module (static makes it "private").
// Define the initial state as paused.
static enum servo_states current_servo_state = state_unknown ;
static enum servo_states saved_servo_state = state_unknown ;

static unsigned char loop_flag = NOT_IN_LOOP ;
static int loop_index = 0 ;

// Code to start the move (adjust PWM) and start the timing delay based on the
// current position.
static void start_move( enum servo_states new_state ) {
	// TBD
}

int process_instruction( unsigned char op_code, unsigned char param, int recipe_index ){
	
	switch(op_code){
		case MOV:
			if (param > 5){
				//error
			} else{
				// Set duty cycle to match specified position
				TIM2->CCR1 = DUTY_CYCLE * (0.02 * (1 + param) ) ;
				recipe_index ++ ;
			}
			break ;
		case WAIT:
			if(param > 31){
				//error
			} else{
				// Wait for given time (param * 1/10) seconds
				unsigned char flag_count = 0 ;
				TIM2->SR &= ~TIM_SR_UIF ;
				while(flag_count < (5 * (param + 1) ) ){
					if( (TIM2->SR && TIM_SR_UIF) == TIM_SR_UIF ){
						flag_count++ ;
						TIM2->SR &= ~TIM_SR_UIF ;
					}
				}
				recipe_index ++ ;
			}
			break ;
		case LOOP:
			if(loop_flag != NOT_IN_LOOP ){
				// nested loop error
			} else if (param > 31){
				// param error
			} else{
				loop_flag = param ;
				recipe_index++ ;
				loop_index = recipe_index ;
			}
			break ;
		case END_LOOP:
			if( loop_flag == NOT_IN_LOOP ){
				// end_loop without loop
			} else if(loop_flag != 0){
				recipe_index = loop_index ;
				loop_flag -- ;
			} else{
				// Done with loop
				recipe_index ++ ;
				loop_flag = NOT_IN_LOOP ;
			}
			break ;
		case RECIPE_END:
			break ;
	}
	
	return recipe_index ;
	
}


void process_event( enum events one_event ) {
	
	// state-independent events
	if(one_event == user_entered_pause ){
		
	} else {	// state-dependent events
		
		switch ( current_servo_state ) {
			case state_position_0 :		// right-most position
				if ( one_event == user_entered_left ) {
					start_move( state_position_1 ) ;
					current_servo_state = state_moving ;		// when the move ends (enough time has elapsed) new state will be state_position_1
				}
				break ;
			case state_position_1 :
				if ( one_event == user_entered_left ) {
					start_move( state_position_2 ) ;
					current_servo_state = state_moving ;
				} else if ( one_event == user_entered_right ) {
					start_move( state_position_0 ) ;
					current_servo_state = state_moving ;
				}
				break;
			case state_position_2 :
				if ( one_event == user_entered_left ) {
					start_move( state_position_3 ) ;
					current_servo_state = state_moving ;
				} else if ( one_event == user_entered_right ) {
					start_move( state_position_1 ) ;
					current_servo_state = state_moving ;
				}
				break ;
			case state_position_3 :
				if ( one_event == user_entered_left ) {
					start_move( state_position_4 ) ;
					current_servo_state = state_moving ;
				} else if ( one_event == user_entered_right ) {
					start_move( state_position_2 ) ;
					current_servo_state = state_moving ;
				}
				break ;
			case state_position_4 :
				if ( one_event == user_entered_left ) {
					start_move( state_position_5 ) ;
					current_servo_state = state_moving ;
				} else if ( one_event == user_entered_right ) {
					start_move( state_position_3 ) ;
					current_servo_state = state_moving ;
				}
				break ;
			case state_position_5 : // left-most position
				if ( one_event == user_entered_right ) {
					start_move( state_position_4 ) ;
					current_servo_state = state_moving ;
				}
				break ;
			case state_moving :			
				break ;
			case state_recipe_paused:
				if ( one_event == user_entered_continue ){
					
				}
			case state_unknown :
				break ;
			case state_recipe_ended :
				break ;
		}
		
	}
	
}
