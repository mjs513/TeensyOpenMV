//rcarduino shared variables
// holds the update flags defined above
volatile uint8_t bUpdateFlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint16_t unThrottleInShared;
volatile uint16_t unSteeringInShared;
volatile uint16_t unRCInShared;

// These are used to record the rising edge of a pulse in the calcInput functions
// They do not need to be volatile as they are only used in the ISR. If we wanted
// to refer to these in loop and the ISR then they would need to be declared volatile
uint32_t ulThrottleStart;
uint32_t ulSteeringStart;
uint32_t ulRCStart;

//uint8_t throttleLeft;
//uint8_t throttleRight;
int tDeadZoneRange, sDeadZoneRange;

/* ================================================ */
//rcarduino constants
#define RC_NEUTRAL_STEERING 1506  //1504 was 1506
#define RC_NEUTRAL_THROTTLE 1504  //1500 was 1511

#define RC_MAX_STEERING 1990
#define RC_MAX_THROTTLE 1994

#define RC_MIN_STEERING 1029  //1012 was 1016
#define RC_MIN_THROTTLE 1004  //1012 was 1014

#define RC_DEADBAND 30

#define RC_MODE_TOGGLE 1510

uint16_t unSteeringMin = RC_MIN_THROTTLE;
uint16_t unSteeringMax = RC_MAX_STEERING;
uint16_t unSteeringCenter = RC_NEUTRAL_STEERING;

uint16_t unThrottleMin = RC_MIN_THROTTLE;
uint16_t unThrottleMax = RC_MAX_THROTTLE;
uint16_t unThrottleCenter = RC_NEUTRAL_THROTTLE;

uint16_t ThrottleDeadBandMax = RC_NEUTRAL_THROTTLE + RC_DEADBAND;
uint16_t ThrottleDeadBandMin = RC_NEUTRAL_THROTTLE - RC_DEADBAND;
uint16_t SteeringDeadBandMax = RC_NEUTRAL_STEERING + RC_DEADBAND;
uint16_t SteeringDeadBandMin = RC_NEUTRAL_STEERING - RC_DEADBAND;

#define PWM_MIN 0
#define PWM_MAX 255

#define GEAR_NONE 0
#define GEAR_IDLE 1
#define GEAR_FULL 2
#define GEAR_NEUTRAL 3

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define THROTTLE_FLAG 1
#define STEERING_FLAG 2
#define RC_FLAG 3

uint8_t gThrottle = 0;
uint8_t gGear = GEAR_NONE;
uint8_t gOldGear = GEAR_NONE;

#define IDLE_MAX 40

#define MODE_RUN 1
#define MODE_PROGRAM 0

uint8_t gMode = MODE_RUN;
uint32_t ulProgramModeExitTime = 0; 




