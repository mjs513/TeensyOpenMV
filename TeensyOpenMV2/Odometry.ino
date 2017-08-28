void send_odometry(){
    //===  Telemetry section =========
    //if(telem_timer > defaultTelemTime) {

      //DateTime time = rtc.now();
      //telem << time.timestamp(DateTime::TIMESTAMP_TIME);
      //telem << utc << ",";

      if(printFlag == 1) telem << etm_millis.elapsed()/1000. << ",";
           
      // IMU
      compass_update();
      if(printFlag == 1) 
            telem << (float) -roll << "," << (float) -pitch << "," << (float) yar_heading << ",";

      //Wheel Encoders
      get_ticks_noreset();

      /********************************************************************/
      /*                                                                  */
      /*        Begin odometry                                            */
      /*                                                                  */
      /********************************************************************/  

      int left_encoder_count = 0;
      int right_encoder_count = 0;
      float theta;
      
	    switch (gDirection){
        case DIRECTION_STOP:
          {
            left_encoder_count = 0;
            right_encoder_count = 0;
            break;
          }
        case DIRECTION_FORWARD:
          left_encoder_count = ticksLR;
          right_encoder_count = ticksRR;
          break;
        case DIRECTION_REVERSE:
			    left_encoder_count = -ticksLR;
			    right_encoder_count = -ticksRR;
			    break;
		    case DIRECTION_ROTATE_LEFT:
			    left_encoder_count = -ticksLR;
			    right_encoder_count = ticksRR;
			    break;
		    case DIRECTION_ROTATE_RIGHT:
			    left_encoder_count = ticksLR;
			    right_encoder_count = -ticksRR;
			    break;			
		    default:
			    left_encoder_count = 0;
			    right_encoder_count = 0;
			    break;
	    }

      // zeros out encoder counts and reads encoders zero value
      //encA.write(0); encB.write(0); encC.write(0); encD.write(0);
      
      float Displacement = (left_encoder_count + right_encoder_count)*ENCODER_SCALE_FACTOR/2.0;
      //float Rotation = (left_encoder_count - right_encoder_count)*ENCODER_SCALE_FACTOR/TRACK;

      if(yar_heading > 180.0 && gDirection == DIRECTION_FORWARD){
        theta = yar_heading - 360.0;
      }

      // Calculate back bearing if going backward
      if(gDirection == DIRECTION_REVERSE) {
        if(yar_heading >= 180.0) {     
          theta = yar_heading - 180.0;
        } else if( yar_heading < 180) {
          theta = yar_heading + 180.0; 
        }
      }

      //radial distance = pos_x
      //phi (radial angle from polar axis = 360 degrees - yar_heading 
      pos_x = pos_x + Displacement * cos(radians(theta-init_heading));  //radial distance
      pos_y = pos_y + Displacement * sin(radians(theta-init_heading));

      if(printFlag == 1) telem << pos_x << "," << pos_y << endl;

      init_ticks_counter();
      //telem_timer = 0;
    //}
}

void odometry(){
  // Convert from String Object to String.
  String serialResponse = "";
  String cmd;
  char sz[] = "f100, l280, r50, b50, f100, l280, r50, b50, f100, l280, r50, b50";

  //ENCODER_SCALE_FACTOR = WHEEL_DIA*PI/CLICKS_PER_REV;
  ENCODER_SCALE_FACTOR = DISTANCE_CALIB / CLICKS_CALIB;
  float odo_start_distance = 0.0;
  
  float old_rx, old_ry, rx, ry;
  old_rx = old_ry = 0;

  telem << endl << "Avaialbe Commands: [f, b, l, r, o]. " << endl;
  telem << "Follow direction commands with number of incdhes, f5 for forward 5 inches" << endl;
  telem << " o - to exit odometry mode" << endl << endl;

  pos_x = pos_y = 0;

  while(odo_mode_toggle == 1){
    delay(100);
    while (telem.available() > 0) {
      serialResponse = telem.readStringUntil('\r\n');
      // Convert from String Object to String.
      char buf[sizeof(sz)];
      serialResponse.toCharArray(buf, sizeof(buf));
      char *p = buf;
      char *str;
      while ((str = strtok_r(p, ",", &p)) != NULL){ // delimiter is the comma
        cmd = (String) str;
        int val = (int) cmd.charAt(0);
        if(cmd.length() > 1) {
          turn_time_mult = atoi(cmd.substring(1, cmd.length()).c_str());
        } else {
          turn_time_mult = 0;
        }

        //turn_time_mult = turn_time_mult + odo_start_distance;  
  
        if(turn_time_mult == 0)
                  turn_time_mult = 0;          
  
        //if(odo_start == 0){
        //  odo_start = 1;
        //}
      
        //odo_start_distance = turn_time_mult;
        odo_start_distance = 0.0;
      
        runODO(val, turn_time_mult);
  
        //positional analysis
        float phi = radians(360 - yar_heading);
        rx = pos_x * cos(phi) + old_rx;
        ry = pos_x * sin(phi) + old_ry;
        telem << "New Position (rx, ry) from start: " << rx << ", " << ry << ", " << phi << endl;
        old_rx = rx;
        old_ry = ry;
        pos_x = pos_y = 0;
        delay(150);
      }
      telem << "Avaialbe Commands: [f, b, l, r, o]. " << endl;

    }
  }
}

void runODO(int val, int turn_time_mult)
{    
    switch(val)
    {
      case 'f' : 
        odo_timer = 0;
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
        if (odo_timer > defaultOdoTime){
          //compass_update();
          send_odometry();
          odo_timer = 0;
        }
        mForward();
       }
      etm_millis.stop(); 
      etm_millis.reset();
      //pos_x = pos_y = 0;
      mStop(); 
      break;

    case 'b' :
        odo_timer = 0;
        set_speed(speed);
        gDirection = DIRECTION_REVERSE;        
        telem.println("Rolling Backward!");

        compass_update();
        
        // Calculate back bearing
        if(yar_heading > 180.0) {
          init_heading = yar_heading - 180.0;
        } else {
          init_heading = yar_heading + 180.0;
        }

        mBackward();
        etm_millis.start();
        send_odometry();
        
       while(abs(pos_x) < turn_time_mult){
        //currentTime = millis();
        if (odo_timer > defaultOdoTime){
          //compass_update();
          send_odometry();
          odo_timer = 0;
        }
        mBackward();
       }
      mStop();
      etm_millis.stop();
      etm_millis.reset();
      //pos_x = pos_y = 0;
      break;

    case 'l' :
      telem.println("Turning to New Heading");
      set_speed(turnSpeed);
      
      compass_update();
      new_heading = yar_heading - turn_time_mult;
      
      if(new_heading < 0){
        new_heading = 360. - new_heading;
      }

      //telem << turn_time_mult << "," << yar_heading << endl;
      
      pivotToOdo(new_heading, yar_heading);
           
      mStop();
      break;

    case 'r' :
      telem.println("Turning to New Heading");
      set_speed(turnSpeed);
      
      compass_update();
      new_heading = turn_time_mult + yar_heading;

      if(new_heading > 360.){
        new_heading = new_heading - 360.;
      }

      //telem << turn_time_mult << "," << (float) yar_heading << endl;
      
      pivotToOdo(new_heading, yar_heading);
           
      mStop();
      break;
     
    case 's' :      
      telem.println("Stop!");
      mStop();
      break;

    case 'g' :
      telem.println("Read Sensors and turn");
      readSensors();
      break;

    case 'o' :
      //telem.println("Toggle Odometry!");
      mStop();
      odo_mode_toggle = 1;
      toggleOdo();
      return;
    }
}


void pivotToOdo(int target, int currentAngle){

  //int currentAngle = yar_heading;
  int diff = target - currentAngle;
  
  etm_millis.start();
  
  while(abs(diff) > HEADING_TOLERANCE){
    //telem << "Compass Control: " << endl;
    //telem << "\t" << currentAngle << ", " << target << ", " << diff << endl;
    //if (odo_timer > defaultOdoTime){
      //compass_update();
    //  send_odometry();
    //  odo_timer = 0;
    //}
    
    if(diff > 0) {
      throttleRight = turnSpeed;
      throttleLeft = turnSpeed;
      mRight();//right
    } else {
      throttleRight = turnSpeed;
      throttleLeft = turnSpeed;
      mLeft();//left
    }

    compass_update();
    currentAngle = yar_heading;
    diff = target - currentAngle;
  }
  
  etm_millis.stop();
  etm_millis.reset(); 
  mStop();
  return;
}

 






