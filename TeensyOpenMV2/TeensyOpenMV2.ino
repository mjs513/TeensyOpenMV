#include <Wire.h>
#include <EEPROM.h>
#include <StopWatch.h>
#include <elapsedMillis.h>
#include <Streaming.h>
#include <StopWatch.h>
#include <Servo.h>

#undef min
#undef max
#include <vector>
#include "Constants.h"
#include "IOpins.h"

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <VL53L0X.h>

//Create VL53L0X instance
VL53L0X sensor;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *lMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rMotor = AFMS.getMotor(2);

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

//#define	SingleShot
//=======================================================//

int roam = 0;
int stop_flag = 0;
int rc_mode_toggle = 0;
int rc_sw_on = 0;
int odo_mode_toggle = 0;

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

uint8_t motor_on = 0;

// Set elapsed time constants
elapsedMillis motorFwd;
elapsedMillis motorFwdRunTime;
elapsedMillis motorTurnTime;
elapsedMillis motorRevTime;
elapsedMillis turn_timer;
elapsedMillis odo_timer;

StopWatch etm_millis;

Servo panServo;
Servo tiltServo;

int panZero = 85;
int tiltZero = 100;

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

/* Set the delay between fresh samples for BNO055*/
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup() {
	telem.begin(57600);           // set up Serial library at 9600 bps
  Wire.begin();
  telem2.begin(115200);

  sensor.init();
  sensor.setTimeout(500);
  //sensor.startContinuous();
  telem << "VL53L0X Initialized" << endl;

  BNO055_Init();
  
	AFMS.begin();  // create with the default frequency 1.6KHz
  telem.println("Adafruit Motorshield v2 - Initialized!");
  
	// Set the speed to start, from 0 (off) to 255 (max speed)
	lMotor->setSpeed(150);
	rMotor->setSpeed(150);

	attachInterrupt(THROTTLE_IN_PIN, calcThrottle, CHANGE);
	attachInterrupt(STEERING_IN_PIN, calcSteering, CHANGE);
	attachInterrupt(RCMODE_IN_PIN, togRCMode, CHANGE);
	throttleLeft = throttleRight = speed;

    panServo.attach(23);
    panServo.write(panZero);
    tiltServo.attach(38);
    tiltServo.write(tiltZero);
  
    //Enable pull ups on encoders
    pinMode(l_encoder, INPUT_PULLUP);
    pinMode(r_encoder, INPUT_PULLUP);
    //Attach interrupts for encoders
    init_ticks_counter();
    attach_encoders();

    telem.println("I'm Ready to receive telem Commands![f, b, r, l, s, t]"); // Tell us I"m ready

}

void loop()
{
/*	#if defined SingleShot
		Serial.print(sensor.readRangeSingleMillimeters());
		if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
	#else 
		Serial.print(sensor.readRangeContinuousMillimeters());
		if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  #endif */
  
	//Serial.println();

  if (telem.available() > 0)
  {
    int val = telem.read();	//read telem input commands

    turn_time_mult = telem.parseInt();
    if(turn_time_mult == 0)
                turn_time_mult = 4;          
    
    switch(val)
    {
  
    case 'f' : 
      motorFwdRunTime = 0;
      motorFwd = 0;
      set_speed(speed);
      gDirection = DIRECTION_FORWARD;        
      telem.println("Rolling Forward!");
      mForward();
      etm_millis.start();   
      //  Read encoders
      init_ticks_counter(); 
      while(motorFwdRunTime < defaultFwdTime){
          mForward();
          //Read encoders and calculate RPM
      }
      mStop();
      get_ticks_reset();
      telem << ticksLR << ", " << ticksRR << endl;
      stop_flag = 0; 
      etm_millis.stop();
      motorFwdRunTime = 0;
      break;
      
    case 'l' :
      motorTurnTime = 0;
      //===================================================
      // Used for turn calibration curve
      //===================================================
      //compass_update();
      //telem << "Current heading: " << yar_heading << endl;
      //telem << "Turn Multiplier: " << turn_time_mult << endl;
      //telem.println("Turning Left!");
      set_speed(turnSpeed);
      //compass_update();
      //telem << "Change heading: " << turn_time_mult*100 << ", " << (float) yar_heading << ", ";
       mLeft();
      //delay(400);  //was 2000
      delay(turn_time_mult * 100);
      mStop();
      while(motorTurnTime < defaultTurnTime) {
       }
      mStop();
      //compass_update();
      //telem << (float) yar_heading << endl;

      motorTurnTime = 0;
      break;
      
    case 'r' :
      //===================================================
      // Used for turn calibration curve
      //===================================================
      //telem << "Turn Multiplier: " << turn_time_mult << endl;
      //telem.println("Turning Right!");
      //compass_update();
      set_speed(turnSpeed);
      //compass_update();
      //telem << "Change heading: " << turn_time_mult*100 << ", " << (float) yar_heading << ", ";
      mRight();
      //delay(400);
      delay(turn_time_mult * 100);
      mStop();
      while(motorTurnTime < defaultTurnTime) {
        }
      mStop();
      //compass_update();
      //telem << (float) yar_heading << endl;
      motorTurnTime = 0;
      break;
      
    case 'b' :    
      motorRevTime = 0;    
      telem.println("Moving Backward!");
      //moveBackward(motorSpeed);
      set_speed(backup_high);
      mBackward();
      while(motorRevTime < defaultRevTime) {
        }
      mStop();
      motorRevTime = 0;
      break;
      
    case 's' :      
      telem.println("Stop!");
      mStop();
      break;

    case 't' :      
      telem.println("toggle Roam Mode"); 
      set_speed(speed);
      toggleRoam();
      break;
	  
    case 'c' :
      telem.println("toggle RC control mode");
      toggleRC();
      break;

    case 'o' :
      telem << "Odometry Activated" << endl;
      toggleOdo();
      break;

    case 'g' :
      telem.println("Read Sensors and turn");
      readSensors();
      break;

    case 'e' : 
      motorFwdRunTime = 0;
      motorFwd = 0;
      //odo_timer = 0;
      set_speed(speed);
      gDirection = DIRECTION_FORWARD;        
      telem.println("Rolling Forward!");
      mForward();
      smartDelay2(10000);
      mStop();
      stop_flag = 0; 
      motorFwdRunTime = 0;
      break;
    }
          
    delay(1);  
    telem.println("I'm Ready to receive telem Commands![f, b, r, l, s, t]"); // Tell us I"m ready
  }

  if(roam == 0){ 
      //just listen for telem commands and wait
      }
  else if(roam == 1){  //If roam active- drive autonomously
    goRoam();
    }

  if(odo_mode_toggle == 0){ 
      //just listen for telem commands and wait
     }
  else if(odo_mode_toggle == 1) {  //If roam active- drive autonomously
    toggleOdo();
    }
	
  if(rc_mode_toggle == 0){ 
      //just listen for telem commands and wait
      }
  else if(rc_mode_toggle == 1) {  //If roam active- drive autonomously
    goRC();
    }
  
  if(unRCInShared > RC_MODE_TOGGLE && rc_mode_toggle == 0) {
        telem << "toggle RC Mode On via SW" << endl; 
        rc_sw_on = 1;
        toggleRC();
     }
}

void toggleRoam(){
  // This method chooses to make the robot roam or else use the telem command input.
  if(roam == 0){
   roam = 1;
   etm_millis.start();
   telem.println("Activated Roam Mode");
  } else {
    etm_millis.stop();
    etm_millis.reset();
    roam = 0;
    mStop();
    telem.println("De-activated Roam Mode");
    telem.println("I'm Ready to receive telem Commands![g, f, b, r, l, s, t, c]"); // Tell us I"m ready

  }
}

void goRoam() {  
  Roam();
}

void toggleRC(){
  // This method chooses to make the robot roam or else use the telem command input.
  if(rc_mode_toggle == 0){
   rc_mode_toggle = 1;
   telem.println("Activated RC Mode");
   etm_millis.start();
  } else {
    rc_mode_toggle = 0;
    throttleLeft = throttleRight = speed;
    mStop();
    etm_millis.stop();
    etm_millis.reset();
    telem.println("De-activated RC Mode");
	  telem.println("I'm Ready to receive telem Commands![g, f, b, r, l, s, t, c, w, o]"); // Tell us I"m ready
  }
}

void goRC() {  
	rc_control();   
}

static void smartDelay2(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    //getTicks();
    //send_telemetry();
  } while (millis() - start < ms);
}

void toggleOdo(){
  if(odo_mode_toggle == 0) {
    odo_mode_toggle = 1;
    telem << "Odometry Nav Activated" << endl;
    telem.println("I'm Ready to receive telem Commands![f (inches), b (inches), r (degs), l (degs), s, o]");
    pos_x = pos_y = 0;
    odometry();
  } else {
    odo_mode_toggle = 0;
    mStop();
    telem << "Odometry Nav De-activated" << endl;
    telem.println("I'm Ready to receive telem Commands![g, f, b, r, l, s, t, c, w, o]"); // Tell us I"m ready
  }
}



