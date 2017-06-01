 void rc_control() {
 
	while( rc_mode_toggle == 1) {
		//telem.println();
		// create local variables to hold a local copies of the channel inputs
		// these are declared static so that thier values will be retained
		// between calls to loop.
		static uint16_t unThrottleIn;
		static uint16_t unSteeringIn;
  
		// local copy of update flags
		static uint8_t bUpdateFlags;

		  // check shared update flags to see if any channels have a new signal
		if(bUpdateFlagsShared)
		{
			noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

			// take a local copy of which channels were updated in case we need to use this in the rest of loop
			bUpdateFlags = bUpdateFlagsShared;

			// in the current code, the shared values are always populated
			// so we could copy them without testing the flags
			// however in the future this could change, so lets
			// only copy when the flags tell us we can.

			if(bUpdateFlags & THROTTLE_FLAG)
			{
				unThrottleIn = unThrottleInShared;
			}

			if(bUpdateFlags & STEERING_FLAG)
			{
				unSteeringIn = unSteeringInShared;
			}

			// clear shared copy of updated flags as we have already taken the updates
			// we still have a local copy if we need to use it in bUpdateFlags
			bUpdateFlagsShared = 0;

			interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
			// as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
			// service routines own these and could update them at any time. During the update, the
			// shared copies may contain junk. Luckily we have our local copies to work with :-)
  
		}

    //Tests to determine if throttle or steering is in the specified dead zone
    tDeadZoneRange = rangeTest(unThrottleIn, ThrottleDeadBandMin, ThrottleDeadBandMax);
    sDeadZoneRange = rangeTest(unSteeringIn, SteeringDeadBandMin, SteeringDeadBandMax);

		// do any processing from here onwards
		// only use the local values unAuxIn, unThrottleIn and unSteeringIn, the shared
		// variables unAuxInShared, unThrottleInShared, unSteeringInShared are always owned by
		// the interrupt routines and should not be used in loop

		// we are checking to see if the channel value has changed, this is indicated 
		// by the flags. For the simple pass through we don't really need this check,
		// but for a more complex project where a new signal requires significant processing
		// this allows us to only calculate new values when we have new inputs, rather than
		// on every cycle.
		if(bUpdateFlags & THROTTLE_FLAG)
		{
		  // A good idea would be to check the before and after value, 
		  // if they are not equal we are receiving out of range signals
		  // this could be an error, interference or a transmitter setting change
		  // in any case its a good idea to at least flag it to the user somehow
		  unThrottleIn = constrain(unThrottleIn, unThrottleMin, unThrottleMax);
      
		  if(unThrottleIn < unThrottleCenter) 
		  { 
        gThrottle = map(unThrottleIn, unThrottleCenter, unThrottleMin, PWM_MIN, PWM_MAX );
        throttleLeft = throttleRight = gThrottle;
        gThrottleDirection = DIRECTION_REVERSE;
        //telem.print("Dir_REV:"); telem.println(gThrottle);
		  }
		  else 
		  {
        gThrottle = map(unThrottleIn, unThrottleCenter, unThrottleMax, PWM_MIN, PWM_MAX );
        throttleLeft = throttleRight = gThrottle;
        gThrottleDirection = DIRECTION_FORWARD;
        //telem.print("Dir_FWD:"); telem.println(gThrottle);
		  }
		}
   
		gDirection = gThrottleDirection;
  
		if(gThrottle < IDLE_MAX) {
			gGear = GEAR_IDLE;
		}
		if(gThrottle > IDLE_MAX) {
			gGear = GEAR_FULL;
		}
    if(tDeadZoneRange == 1 && sDeadZoneRange == 1){
  		gGear = GEAR_NEUTRAL;
		}

		//telem.print("GEAR: "); telem.println(gGear);
		
		if(bUpdateFlags & STEERING_FLAG)
		{
			throttleLeft = gThrottle;
			throttleRight = gThrottle;

			gDirection = gThrottleDirection;
			//telem.print("Direction: "); telem.println(gDirection);
    
			// see previous comments regarding trapping out of range errors
			// this is left for the user to decide how to handle and flag
			unSteeringIn = constrain(unSteeringIn, unSteeringMin, unSteeringMax);

			// if idle spin on spot
			switch(gGear)
			{
        case GEAR_NEUTRAL:
          gDirection = DIRECTION_STOP;
          break;
				case GEAR_IDLE:  
					// same changes for steering as for throttle
					if(unSteeringIn < (unSteeringCenter - RC_DEADBAND))
					{
						gDirection = DIRECTION_ROTATE_LEFT;
						// use steering to set throttle
						throttleRight = throttleLeft = map(unSteeringIn, unSteeringCenter, unSteeringMin, PWM_MIN, PWM_MAX);
						//telem.print("Rotate Right: "); telem.println(throttleRight);
					} else if(unSteeringIn > (unSteeringCenter + RC_DEADBAND))
					{
						gDirection = DIRECTION_ROTATE_RIGHT;
						// use steering to set throttle
						throttleRight = throttleLeft = map(unSteeringIn, unSteeringCenter, unSteeringMax, PWM_MIN, PWM_MAX);
						//telem.print("Rotate Left: "); telem.println(throttleRight);
					}
					break;
				// if not idle proportionally restrain inside track to turn vehicle around it
				case GEAR_FULL:
					if(unSteeringIn > (unSteeringCenter + RC_DEADBAND))
					{
						throttleRight = map(unSteeringIn,unSteeringCenter,unSteeringMax,gThrottle,PWM_MIN);
            throttleLeft = throttleLeft;
            if(gThrottle < 100) {
              throttleRight = 1.2* throttleRight;
              throttleLeft = 1.2 * throttleLeft;
            }
						//telem.print("1. Turn Right: "); telem.print(throttleRight);
						//telem.print(" LEFT: "); telem.println(throttleLeft);
					} else if(unSteeringIn < (unSteeringCenter - RC_DEADBAND)) {
						throttleLeft = map(unSteeringIn,unSteeringMin,unSteeringCenter,PWM_MIN,gThrottle);
            throttleRight = throttleRight;
            if(gThrottle < 100) {
              throttleRight = 1.2* throttleRight;
              throttleLeft = 1.2 * throttleLeft;
            }
						//telem.print("2. Turn Left: "); telem.print(throttleRight);
						//telem.print("  LEFT: "); telem.println(throttleLeft);
					}
					break;        
			}
		} 

		// if idle spin on spot

		if(telem.available() > 0 ) {
			int val = telem.read();  //read telem input commands  
			if(val == 'c') {
				telem.println("Toggle RC Mode off"); 
				toggleRC();
				return;
			}
		}

    //Send telemetry data to radio
    //send_telemetry();
    readSensors();

    if(unRCInShared < RC_MODE_TOGGLE && rc_sw_on == 1) {
        mStop();
        telem << "toggle RC Mode off via SW" << endl; 
        rc_sw_on = 0;
        toggleRC();
      }
      
		//telem.print("Direction Updated: "); telem.println(gDirection);
    //telem.println();
		switch(gDirection)
		{
		  case DIRECTION_FORWARD:  
			  if(gGear == GEAR_FULL) {
					mForward();
				}
			  break;
		  case DIRECTION_REVERSE:
          mBackward();
			  break;
		  case DIRECTION_ROTATE_LEFT:
			  mLeft();
			  break;
		  case DIRECTION_ROTATE_RIGHT:
			  mRight();
			  break;
		  case DIRECTION_STOP:
			  mStop();
			  break;
		}

		bUpdateFlags = 0;

	}
}


// simple interrupt service routine
void calcThrottle() {
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  if(digitalRead(THROTTLE_IN_PIN) == HIGH) {
    ulThrottleStart = micros();
   } else {
    // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
    // this gives use the time between the rising and falling edges i.e. the pulse duration.
    unThrottleInShared = (uint16_t)(micros() - ulThrottleStart);
    //telem.println("Throttle: "); telem.print(unThrottleInShared);
    // use set the throttle flag to indicate that a new throttle signal has been received
    bUpdateFlagsShared |= THROTTLE_FLAG;
   }
}

void calcSteering(){
  if(digitalRead(STEERING_IN_PIN) == HIGH) {
    ulSteeringStart = micros();
  } else {
    unSteeringInShared = (uint16_t)(micros() - ulSteeringStart);
    //telem.print("         Steering: "); telem.println(unSteeringInShared);
    bUpdateFlagsShared |= STEERING_FLAG;
  }
}

void togRCMode(){
  if(digitalRead(RCMODE_IN_PIN) == HIGH) {
    ulRCStart = micros();
  } else {
    unRCInShared = (uint16_t)(micros() - ulRCStart);
    //telem.print("         Steering: "); telem.println(unRCInShared);
    bUpdateFlagsShared |= RC_FLAG;
  }
}

bool rangeTest(uint16_t number, uint16_t lower, uint16_t upper) {
  // use a < for an inclusive lower bound and exclusive upper bound
  // use <= for an inclusive lower bound and inclusive upper bound
  // alternatively, if the upper bound is inclusive and you can pre-calculate
  //  upper-lower, simply add + 1 to upper-lower and use the < operator.
  if ((unsigned)(number-lower) <= (upper-lower)) {
    return true;
    } else {
      return false;
    }
}





