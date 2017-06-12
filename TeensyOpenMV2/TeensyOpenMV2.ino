#include <Wire.h>
#include <EEPROM.h>
#include <StopWatch.h>
#include <elapsedMillis.h>
#include <Streaming.h>
#include <StopWatch.h>
#include <Servo.h>
#include <vector>

#include "Constants.h"
#include "IOpins.h"
#include "init.h"
#include "initrcduino.h"

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

/* Set the delay between fresh samples for BNO055*/
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55);

Servo panServo;
Servo tiltServo;

// Set elapsed timers
elapsedMillis motorFwd;
elapsedMillis motorFwdRunTime;
elapsedMillis motorTurnTime;
elapsedMillis motorRevTime;
elapsedMillis turn_timer;
elapsedMillis odo_timer;

StopWatch etm_millis;

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

    //Calculate Encoder Scale factor
    ENCODER_SCALE_FACTOR = WHEEL_DIA*PI/CLICKS_PER_REV;

    telem.println("I'm Ready to receive telem Commands![m, o, c, t]"); // Tell us I"m ready

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
    
    switch(val)
    {
      case 'm':
        telem << "Toggle Manual Mode ON" << endl;
        goManual();
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
    }
          
    delay(1);  
    telem.println("I'm Ready to receive Mode Commands![m, o, c, t]"); // Tell us I"m ready
  }

  if(manual == 0){ 
      //just listen for telem commands and wait
      }
  else if(manual == 1){  //If roam active- drive autonomously
    goManual();
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
    telem.println("I'm Ready to receive Mode Commands![m, o, c, t]"); // Tell us I"m ready
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
    telem.println("I'm Ready to receive Mode Commands![m, o, c, t]"); // Tell us I"m ready
  }
}

void goRC() {  
	rc_control();   
}

void goManual(){
  modeManual();
}

void toggleManual(){
  // This method chooses to make the robot roam or else use the telem command input.
  if(manual_toggle == 0){
   manual_toggle = 1;
   telem.println("Manual Mode Activated");
  } else {
    manual_toggle = 0;
    mStop();
    telem.println("De-activated Manual Mode");
    telem.println("I'm Ready to receive Mode Commands![m, o, c, t]"); // Tell us I"m ready
  }
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
    telem.println("I'm Ready to receive Mode Commands![m, o, c, t]"); // Tell us I"m ready
  }
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




