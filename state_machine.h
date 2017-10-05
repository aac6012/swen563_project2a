// This is a good way to define the status of the display.
enum status {
	status_running,
	status_paused,
	status_command_error,
	status_nested_error 
} ;

// This is a good way to define the state of a servo motor.
enum servo_states {
	state_position_0,
	state_position_1,
	state_position_2,
	state_position_3,
	state_position_4,
	state_position_5,
	state_unknown,
	state_moving,
	state_recipe_paused,
	state_recipe_ended
} ;

// This is a good way to define the event transitions between states.
// More events are needed.
enum events {
	user_entered_left,
	user_entered_right,
	user_entered_pause,
	user_entered_continue,
	user_entered_restart,
	user_enetered_no_op,
	recipe_ended
} ;


void process_event( enum events one_event ) ;
int process_instruction( unsigned char op_code, unsigned char param, int recipe_index ) ;
