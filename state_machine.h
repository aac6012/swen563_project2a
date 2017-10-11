#include "stm32l476xx.h"

// This is a good way to define the status of the display.
enum status {
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
	user_entered_no_op
} ;


typedef struct {
	TIM_TypeDef *timer ;		// The PWM timer in charge of controlling servo output signal
	int delay_counter ;			// Delay counter used for WAIT and between servo movements
	int recipe_index ; 			// Current index of the executing recipe
	enum servo_states state ;	// Current state of the servo
	unsigned char *recipe ;		// Recipe being executed by servo
	int is_paused ;				// If the recipe is paused
} Servo ;


void process_user_event( Servo* servo, enum events one_event ) ;
void process_instruction( Servo* servo, unsigned char op_code, unsigned char param ) ;
void start_move( Servo* servo, enum servo_states target_position ) ;
int servo_state_to_int( enum servo_states state ) ;
