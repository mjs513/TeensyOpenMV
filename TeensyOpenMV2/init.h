// Uncomment this line to use long range mode. This
// increases the sensitivity of the sensor and extends its
// potential range, but increases the likelihood of getting
// an inaccurate reading because of reflections from objects
// other than the intended target. It works best in dark
// conditions.

//#define LONG_RANGE


// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

//#define HIGH_SPEED
//#define HIGH_ACCURACY

//#define  SingleShot
//=======================================================//

// Initial mode flags
uint8_t roam = 0;
uint8_t stop_flag = 0;
uint8_t rc_mode_toggle = 0;
uint8_t rc_sw_on = 0;
uint8_t odo_mode_toggle = 0;
uint8_t manual = 0;
uint8_t manual_toggle = 0;

uint8_t motor_on = 0;

int panZero = 80;
int tiltZero = 100;

// Motor control
#define DIRECTION_STOP 0
#define DIRECTION_FORWARD 1
#define DIRECTION_REVERSE 2
#define DIRECTION_ROTATE_RIGHT 3
#define DIRECTION_ROTATE_LEFT 4
#define DIRECTION_PIVOT_RIGHT 5
#define DIRECTION_PIVOT_LEFT 6

uint8_t gThrottleDirection = DIRECTION_STOP;
uint8_t gDirection = DIRECTION_STOP;
uint8_t gOldDirection = DIRECTION_STOP;

String str1, str2;
int gapAngle[10];
float gapDist[10];
int i, j, k;

//Globals
float roll, pitch;
float fXg = 0;
float fYg = 0;
float fZg = 0;
float gyroz, accelx, accely;

// global for heading from compass
float yar_heading, new_heading;
float rebound_angle;

// Compass navigation
float targetHeading,              // where we want to go to reach current waypoint
      currentHeading,             // where we are actually facing now
      headingError,               // signed (+/-) difference between targetHeading and currentHeading
      init_heading;
    
float pos_x,
      pos_y,
      ENCODER_SCALE_FACTOR;

//encoder variables
int ticksRR,
     ticksLR;
volatile int kcount_revs_l,
             kcount_revs_r;


