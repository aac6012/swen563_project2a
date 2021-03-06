#include "stm32l476xx.h"

// This is a good way to define the status of the display.
enum recipe_status {
	status_running,
	status_paused,
	status_ended,
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
	state_unknown
} ;

// This is a good way to define the event transitions between states.
// More events are needed.
enum events {
	user_entered_left,
	user_entered_right,
	user_entered_pause,
	user_entered_continue,
	user_entered_restart,
	user_entered_no_op,
	user_entered_invalid
} ;


typedef struct {
	TIM_TypeDef *timer ;		// The PWM timer in charge of controlling servo output signal
	uint32_t delay_counter ;			// Delay counter used for WAIT and between servo movements
	uint32_t recipe_index ; 			// Current index of the executing recipe
	uint32_t loop_index ;			// Index of the start of a loop
	uint32_t loop_count ; 	// Number of iterations left in a loop; also indicates not in a loop when equal to NOT_IN_LOOP.
	enum servo_states position ;// Current position of the servo
	enum recipe_status status ; // Current status of the recipe being executed on the servo
	unsigned char *recipe ;		// Recipe being executed by servo
} Servo ;


void process_user_event( volatile Servo* servo, enum events one_event ) ;
void process_instruction( volatile Servo* servo, unsigned char op_code, unsigned char param ) ;
void start_move( volatile Servo* servo, enum servo_states target_position ) ;
enum events input_char_to_event( char inputChar ) ;
int servo_state_to_int( enum servo_states state ) ;
enum servo_states servo_int_to_state( int servo_int );
