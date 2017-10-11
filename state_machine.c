
#include <stdlib.h>

#include "state_machine.h"
#include "stm32l476xx.h"
#include "globals.h"

void start_move( Servo* servo, enum servo_states target_position ){
	
	// Calculate the number of positions being moved.
	int offset = abs( servo_state_to_int(servo->position ) - servo_state_to_int(target_position) ) ;
	
	// The recipe timer runs at 100ms, 1 position takes about 200ms.
	// So, 2*offset will give the necessary wait time for the length being travelled.
	servo->delay_counter = 2*offset ;
	
	
}

void process_instruction( Servo* servo, unsigned char op_code, unsigned char param ){
	
	// Don't process instructions in these states.
	if( servo->status == status_paused || servo->status == status_command_error || servo->status == status_nested_error ) {
		return ;
	}
	
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
				
				// master recipe timer is at 100ms, so 1 wait command = 1 timer cycle
				servo->delay_counter = 1 + param ;
				
				servo->recipe_index ++ ;
			}
			break ;
		case LOOP:
			if(servo->loop_count != NOT_IN_LOOP ){
				// nested loop error
			} else if (param > 31){
				// param error
			} else{
				servo->loop_count = param ;
				servo->recipe_index++ ;
				servo->loop_index = servo->recipe_index ;
			}
			break ;
		case END_LOOP:
			if( servo->loop_count == NOT_IN_LOOP ){
				// end_loop without loop
			} else if( servo->loop_count != 0 ){
				servo->recipe_index = servo->loop_index ;
				servo->loop_count -- ;
			} else{
				// Done with loop
				servo->recipe_index ++ ;
				servo->loop_count = NOT_IN_LOOP ;
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
		
		switch ( servo->position ) {
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
