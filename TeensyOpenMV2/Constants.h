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

#define telem Serial3   //Connection for radio on 3, change to serial for telem
#define telem2  Serial2 //Connection for OpenMV

#define rad2deg 57.2957795131
#define deg2rad 0.01745329251
#define m2ft 3.280839895

//Malek Systems RGB LED WS2812B
#define DATA_PIN 31
#define NUM_LEDS 8
#define BRIGHTNESS  64
CRGB leds[NUM_LEDS];

// the interval in mS 
//#define interval 7500    //was 7500
#define interval 100
#define interval1 2000
#define defaultTurnTime 1000
#define defaultFwdTime 2500 //was 7000
#define defaultRevTime 700
#define defaultTelemTime 1000
#define defaultWayPointTime 1000
#define defaultOdoTime 50
#define sensorUpdate 50
#define sensorUpdate1 50

//Bubble Rebound Parameters
const float V = 21;
const float Ki = 0.2;
const int N = 30;  //was 10, 12 for 12 readings, was 12
const int angle = 6;  //was 20 degrees, was 15 for 12
//const int N = 25;  //was 10, 12 for 12 readings, was 12
//const int angle = 7.5;  //was 20 degrees, was 15 for 12

unsigned int obsDist = 35;  //was 29
unsigned int maxRange = 250;
unsigned int rangeModifier = 100;
unsigned int printFlag = 0;
float MIN_DISTANCE = 10;
//int fowardheadThreshold = 29; //was 49, 39, 29; was 27, increase to 32 (2/6)

//compass reads
//#define DEC_ANGLE -13.1603  // -13.1603 degrees for Flushing, NY
#define DEC_ANGLE 0

// Compass  Constants
#define HEADING_TOLERANCE 5     // tolerance +/- (in degrees) within which we don't attempt to turn to intercept targetHeading
float wp_heading;

//Motor constants
// Speeds (range: 0 - 255)
#define FAST_SPEED = 60;
#define NORMAL_SPEED 40
#define TURN_SPEED 30
#define TURN_SPEED_DIFF = 75;
#define SLOW_SPEED 20
//#define NORMAL_SPEED speed

//Set Motor Speed
int speed = 150;
int turnSpeed = 175;
int backup_high = 150;
int backup_low = 100;
uint8_t throttleRight = 175;          //define motor speed parameter which will be mapped as a percentage value
                                      // with canakit seems like i have to increase this value, increase to 50
uint8_t throttleLeft = 175;           // offset required for differences in motor speed,82, (84/86)

//Adjustment factors to make rover go straight.  
//Also used for odometry
float lf_mtr_adj = 0.96;
float rt_mtr_adj = 1.0;

int turn_time_mult = 2;
int turn_time;

//***Odometry constants, dimenstions in inches
#define TRACK 4.4
#define WHEEL_DIA 2.3
#define CLICKS_PER_REV 168
#define DISTANCE_CALIB  39.5  // centimeters
#define CLICKS_CALIB    392.5 // average clicks for distance calibration










