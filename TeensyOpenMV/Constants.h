//============================================================================
//    Sketch to test various technicques in robotic car design such as
//    obstacle detection and avoidance, compass as turn guide,
//    motor control, etc.
//    Copyright (C) 2015  Michael J Smorto
//    https://github.com/mjs513/Sainsmart_4WD_Obstacle_Avoid_TestBed.git
//    FreeIMU@gmail.com
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//============================================================================

#define telem Serial3
#define telem2  Serial2

#define rad2deg 57.2957795131
#define deg2rad 0.01745329251
#define m2ft 3.280839895

// the interval in mS 
//#define interval 7500    //was 7500
#define interval 100
#define interval1 2000
#define defaultTurnTime 1000
#define defaultFwdTime 10000 //was 7000
#define defaultRevTime 700
#define defaultTelemTime 1000
#define defaultWayPointTime 1000
#define defaultOdoTime 50
#define sensorUpdate 50
#define sensorUpdate1 50

//Bubble Rebound Parameters
const float V = 21;
const float Ki = 0.2;

//compass reads
//#define DEC_ANGLE -13.1603  // -13.1603 degrees for Flushing, NY
#define DEC_ANGLE 0

const int N = 31;  //was 10, 12 for 12 readings, was 12
const int angle = 6;  //was 20 degrees, was 15 for 12

//const int N = 25;  //was 10, 12 for 12 readings, was 12
//const int angle = 7.5;  //was 20 degrees, was 15 for 12


// Speeds (range: 0 - 255)
int FAST_SPEED = 60;
#define NORMAL_SPEED 40
#define TURN_SPEED 30
int TURN_SPEED_DIFF = 75;
#define SLOW_SPEED 20
//#define NORMAL_SPEED speed

//Set Motor Speed
int speed = 150;
int turnSpeed = 150;
float lf_mtr_adj = 1.5;
float rr_mtr_adj = 0.97;
float lr_mtr_adj = 1.15;
float rf_mtr_adj = 1.0;
int turn_time_mult = 2;
//int turn_time;

//int motorSpeed_right = 87;        //define motor speed parameter which will be mapped as a percentage value
//int motorSpeed_left = 75;         // these are reversed right is left when looking from behind
//was 50 for motor speed
uint8_t throttleRight = 150;          //define motor speed parameter which will be mapped as a percentage value
                                    // with canakit seems like i have to increase this value, increase to 50
uint8_t throttleLeft = 150;           // offset required for differences in motor speed,82, (84/86)
//was 75 for turnspeed
//int turnSpeed = 65;                 //define turning speed parameter, was 75, was 87
                                    //change due to canakit driver
//was 40
//int speed = 255;                     //increase from 40 to 50 (2/6)
//int turn_time_mult = 2;
int turn_time;

int backup_high = 65;
int backup_low = 50;

const int left_37 = 443;  //was 387 for 37 deg, 250 for 22deg (443
const int left_45 = 513; 
const int left_57 = 614;  //was 571 for 57, 461 for 45 (499)

const int right_37 = 482; //was 440 for 37, 295 for 22 deg 482
const int right_45 = 543;
const int right_57 = 628; //was 636 for 57, 519 for 45 (543)
   
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

#define IDLE_MAX 40

#define MODE_RUN 1
#define MODE_PROGRAM 0

uint8_t gMode = MODE_RUN;
uint32_t ulProgramModeExitTime = 0; 


//***Odometry
#define TRACK 8.0
#define WHEEL_DIA 3.0
#define CLICKS_PER_REV 350






