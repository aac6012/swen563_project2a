// Define all of the commands that are valid
#define MOV (0x20)
#define WAIT (0x40)
#define LOOP (0x80)
#define END_LOOP (0xA0)
#define RECIPE_END (0x00)

// Define useful masks
#define OPCODE_MASK (0xE0)
#define PARAM_MASK (0x1F)

#define DUTY_CYCLE 414

#define NOT_IN_LOOP 32
