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

//******************   moveForward *******************
//void moveForward(int Speed)
void mForward()
{ 
  //gDirection = DIRECTION_FORWARD;
  //motor_on == 1;
	lMotor->setSpeed(throttleLeft*lf_mtr_adj);
	rMotor->setSpeed(throttleRight*rt_mtr_adj);
	lMotor->run(FORWARD); // Motor 1 goes forward
	rMotor->run(FORWARD); // Motor 2 goes forward
	//telem << "Going forward" << endl;
  //telem.println("Forward");
}

//void moveBackward(int Speed)
void mBackward()
{
  //gDirection = DIRECTION_REVERSE;
  //motor_on = 1;
	lMotor->setSpeed(throttleLeft*lf_mtr_adj);
	rMotor->setSpeed(throttleRight*rt_mtr_adj);
	lMotor->run(BACKWARD); // Motor 2 goes back
	rMotor->run(BACKWARD); // Motor 1 goes back
  //telem.println("Backward");
}

void mRight()
{
	//gDirection = DIRECTION_ROTATE_RIGHT;
	//motor_on = 1;
	lMotor->setSpeed(throttleLeft*lf_mtr_adj);
	rMotor->setSpeed(throttleRight*rt_mtr_adj);
	rMotor->run(BACKWARD); // Turns off motor 2
	lMotor->run(FORWARD); // Motor 1 goes forward
	//telem.println("Right");
}

void mLeft()
{
	//gDirection = DIRECTION_ROTATE_LEFT;
	//motor_on = 1;
	lMotor->setSpeed(throttleLeft*lf_mtr_adj);
	rMotor->setSpeed(throttleRight*rt_mtr_adj);
	rMotor->run(FORWARD); // Motor 2 goes forward
	lMotor->run(BACKWARD); // turns off motor 1
  //telem.println("Left");
}

void mStop()
{
  gDirection = DIRECTION_STOP;
  //motor_on = 0;
	lMotor->run(RELEASE); // Turns off motor 1
	rMotor->run(RELEASE); // Turns off motor 2
  //telem.println("Brake");
}

void set_speed(int motor_speed) {
  throttleRight = motor_speed;
  throttleLeft = motor_speed;
}









