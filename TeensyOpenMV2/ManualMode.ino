void modeManual(){

  int val;
  while(1){
    if(telem.available() > 0) {
      val = telem.read();  //read telem input commands  

    turn_time_mult = telem.parseInt();
    if(turn_time_mult == 0)
                turn_time_mult = 4;  
                
    switch(val)
    {
    case 'm':
        telem << "Toggle Manual Mode Off" << endl; 
        manual_toggle = 1;
        toggleManual();
        return;
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

    case 'g' :
      telem.println("Read Sensors and turn");
      readSensors();
      break;
      
    case 'L' :
      telem.println("Turn Lights On");
      for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
        // Turn our current led on to white, then show the leds
        leds[whiteLed] = CRGB::White;
      }
      // Show the leds (only one of which is set to white, from above)
      FastLED.show();
      break;
      
    case 'O' :
      telem.println("Turn Lights Off");
      for(int blkLed = 0; blkLed < NUM_LEDS; blkLed = blkLed + 1) {
        // Turn our current led back to black for the next loop around
        leds[blkLed] = CRGB::Black;
      }
      break;
      }
     telem << endl << "Available commands [f, b, l, r, m]" << endl;
    }
  }
}




