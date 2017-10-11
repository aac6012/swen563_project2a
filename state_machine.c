
#include <stdlib.h>

#include "state_machine.h"
#include "stm32l476xx.h"
#include "globals.h"

static unsigned char loop_flag = NOT_IN_LOOP ;
static int loop_index = 0 ;

void start_move( Servo* servo, enum servo_states target_position ){
	
	
	
}

void process_instruction( Servo* servo, unsigned char op_code, unsigned char param ){
	
	switch(op_code){
		case MOV:
			if (param > 5){
				//error
			} else{
				// Set duty cycle to match specified position
				servo->timer->CCR1 = DUTY_CYCLE * (0.02 * (1 + param) ) ;
				servo->recipe_index ++ ;
			}
			break ;
		case WAIT:
			if(param > 31){
				//error
			} else{
				// Wait for given time (param * 1/10) seconds
				unsigned char flag_count = 0 ;
				servo->timer->SR &= ~TIM_SR_UIF ;
				while(flag_count < (5 * (param + 1) ) ){
					if( (servo->timer->SR && TIM_SR_UIF) == TIM_SR_UIF ){
						flag_count++ ;
						servo->timer->SR &= ~TIM_SR_UIF ;
					}
				}
				servo->recipe_index ++ ;
			}
			break ;
		case LOOP:
			if(loop_flag != NOT_IN_LOOP ){
				// nested loop error
			} else if (param > 31){
				// param error
			} else{
				loop_flag = param ;
				servo->recipe_index++ ;
				loop_index = servo->recipe_index ;
			}
			break ;
		case END_LOOP:
			if( loop_flag == NOT_IN_LOOP ){
				// end_loop without loop
			} else if(loop_flag != 0){
				servo->recipe_index = loop_index ;
				loop_flag -- ;
			} else{
				// Done with loop
				servo->recipe_index ++ ;
				loop_flag = NOT_IN_LOOP ;
			}
			break ;
		case RECIPE_END:
			break ;
	}
	
}


void process_user_event( Servo* servo, enum events one_event ) {
	
	// state-independent events
	if(one_event == user_entered_pause ){
		
	} else {	// state-dependent events
		
		switch ( servo->state ) {
			case state_position_0 :		// right-most position
				if ( one_event == user_entered_left ) {
					start_move( servo, state_position_1 ) ;
				}
				break ;
			case state_position_1 :
				if ( one_event == user_entered_left ) {
					start_move( servo, state_position_2 ) ;
				} else if ( one_event == user_entered_right ) {
					start_move( servo, state_position_0 ) ;
				}
				break;
			case state_position_2 :
				if ( one_event == user_entered_left ) {
					start_move( servo, state_position_3 ) ;
				} else if ( one_event == user_entered_right ) {
					start_move( servo, state_position_1 ) ;
				}
				break ;
			case state_position_3 :
				if ( one_event == user_entered_left ) {
					start_move( servo, state_position_4 ) ;
				} else if ( one_event == user_entered_right ) {
					start_move( servo, state_position_2 ) ;
				}
				break ;
			case state_position_4 :
				if ( one_event == user_entered_left ) {
					start_move( servo, state_position_5 ) ;
				} else if ( one_event == user_entered_right ) {
					start_move( servo, state_position_3 ) ;
				}
				break ;
			case state_position_5 : // left-most position
				if ( one_event == user_entered_right ) {
					start_move( servo, state_position_4 ) ;
				}
				break ;
			case state_unknown :
				break ;
		}
		
	}
	
}



int servo_state_to_int( enum servo_states state ){
	int servo_int ;
	switch(state){
		case state_position_0:
			servo_int = 0 ;
		case state_position_1:
			servo_int = 1 ;
		case state_position_2:
			servo_int = 2 ;
		case state_position_3:
			servo_int = 3 ;
		case state_position_4:
			servo_int = 4 ;
		case state_position_5:
			servo_int = 5 ;
		case state_unknown:
		default:
			servo_int = 0 ;
			break ;
	}
	
	return servo_int ;
	
}
