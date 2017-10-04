
#include "state_machine.h"

// Define all of the commands that are valid
#define MOV (0x20)
#define WAIT (0x40)
#define LOOP (0x80)
#define END_LOOP (0xA0)
#define RECIPE_END (0x00)

// Define a "global" state value that is only accessible in one .c module (static makes it "private").
// Define the initial state as paused.
static enum servo_states current_servo_state = state_unknown ;
static enum servo_states saved_servo_state = state_unknown ;

	
// Code to start the move (adjust PWM) and start the timing delay based on the
// current position.
static void start_move( enum servo_states new_state ) {
	// TBD
}

void process_instruction( unsigned char op_code, unsigned char param ){
	
	switch(op_code){
		case MOV:
			
		case WAIT:
			
		case LOOP:
			
		case END_LOOP:
			
		case RECIPE_END:
			break ;
	}
	
	
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
