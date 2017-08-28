void Roam()
{
  int range;
  telem << "Entered Roam mode" << endl;
	while(1){
		if(telem.available() > 0) {
			int val = telem.read();  //read telem input commands  
			if(val == 't') {
				telem.println("Toggle Roam Mode Off"); 
				toggleRoam();
				return;
			}
	  }

    //read camera sesor and turn to avoid obstacle
    readSensors();
    //do a double check on the distance
    range = sensor.readRangeSingleMillimeters()/10;
    //telem << "Range1 = " << range;
    if(range < obsDist) 
        readSensors();
    delay(100);
    //if still less than 10cm run pseudo vfh method
    range = sensor.readRangeSingleMillimeters()/10;
    //telem << "      Range2 = " << range << endl;
    if(range < obsDist) {
        Select_Direction();
        compass_update(); 
        new_heading = yar_heading - rebound_angle;
        turnCorrection(rebound_angle);
    }

    //Dup code from odometry - probably make separate function
    //measure direction and move forward 3/4 of the distance
    if(range > maxRange)
        range = rangeModifier;
    turn_time_mult = 0.50 * (range);
    pos_x = pos_y = 0.0;
    set_speed(speed);
    gDirection = DIRECTION_FORWARD;
    telem << "Rolling Forward!" << endl;
    //telem << turn_time_mult << endl;

    compass_update();
    if(yar_heading > 180.0) {
      init_heading = yar_heading - 360.0;
    } else {
      init_heading = yar_heading;
    }
    etm_millis.start();
    //send_odometry();
    mForward();
    
     while(abs(pos_x) < turn_time_mult){
      //currentTime = millis();
      if(telem.available() > 0) {
        int val = telem.read();  //read telem input commands  
        if(val == 't') {
          telem.println("Toggle Roam Mode Off");
          toggleRoam();
          return;
        }
      }
      if (odo_timer > defaultOdoTime){
        //compass_update();
        send_odometry();
        odo_timer = 0;
      }
      range = sensor.readRangeSingleMillimeters()/10;
      //telem << "      Range2 = " << range << endl;
      if(range < obsDist) {
        mStop();
        odo_timer = 0;
        break;
      }
      mForward();
     }
    etm_millis.stop(); 
    etm_millis.reset();
    //pos_x = pos_y = 0;
    mStop(); 
	}
}








