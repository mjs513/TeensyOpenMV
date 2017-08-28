void readSensors()
{
   float new_heading;
   int numAngles, indexBestAngle, actAngle;
   float camAngles;
   
   readTSL2561();
   String str3 = "";
   int tempAngles = 0;
   
   if(telem2.available() > 0) {

      numAngles = 0;
  	  for(j = 0; j < numPanAngles; j++){
  		  panServo.write(panAngles[j] + panZero);

  		  for(i = 0; i < numMVObs; i++){
    			strObsArray[i] = telem2.readStringUntil('\n');
    			if(DEBUG == 1) telem << strObsArray[i] << endl;
  		  }
		  
  		  for(i = 0; i < numMVObs; i++){
    			tempAngles = getValue(strObsArray[i], ',', 0).toInt();       //Get number of gaps
    			if(tempAngles == 0){
    				continue;
    			} else {
    				for(ii = 1; ii < tempAngles + 1; ii++){
              actAngle = (panAngles[j] + getValue(strObsArray[i], ',', ii).toInt());
    					str3 = str3 + actAngle + ",";
    				}
    				numAngles += tempAngles;
    			}
  		  }
       
        delay(400);	  
	    }
     
      if(DEBUG == 1) {
        telem << numAngles << endl;
        telem << str3 << endl;
        telem << endl;
      }
       
      if(numAngles > 0){
        for(i = 0; i < numAngles; i++){
          camAngles = getValue(str3, ',', i).toInt();   //Get gap angles
          gapAngle[i] = camAngles;         
        }

        int newNumAngles = getDups(gapAngle, numAngles);
        numAngles = newNumAngles;
        if(DEBUG == 1){
          for(ii = 0; ii < numAngles; ii++) {
            if(gapAngle[ii] != -1)
            telem << gapAngle[ii] << " ";
          }
          telem << endl;
        }

        
        for(i = 0; i < numAngles; i++){
          panServo.write(gapAngle[i]);          //Move servo to first gap
          delay(200);
          gapDist[i] = sensor.readRangeSingleMillimeters()/10;  //range in cm
          delay(100);
          if(DEBUG == 1) telem << "Gap Angle: " << gapAngle[i] << ", Gap Distance: ";
          if (sensor.timeoutOccurred()) { 
            telem << " TIMEOUT" << endl; 
            return;
          }
          if(DEBUG == 1) telem << gapDist[i] << " -- " << endl;
        }

        panServo.write(panZero);
        delay(100);
        
        indexBestAngle = getIndexOfMaximumValue(gapDist, numAngles);
        if(DEBUG == 1) {
          telem << "Best Angle: ";
          telem << indexBestAngle << ", " << gapAngle[indexBestAngle] << ", " << gapDist[indexBestAngle] << endl;
        }
        
        if(gapDist[indexBestAngle] < obsDist) {
          Select_Direction();
          compass_update(); 
          new_heading = yar_heading - rebound_angle;
          if(DEBUG == 1) telem << "Heading from (CAMSD): " << endl;
          turnCorrection(rebound_angle);
        } else {
          if(DEBUG == 1) telem << "Heading from (cam): " << endl;
          new_heading = new_heading;
          turnCorrection(gapAngle[indexBestAngle]); 
        }
      } else {
        camAngles = getValue(str3, ',', 1).toInt();
        if(camAngles == -99) {
          telem << "NO VISIABLE GAP!" << endl;
          if(sensor.readRangeSingleMillimeters()/10 < obsDist){ 
            Select_Direction();
            telem << "Heading from (DC): " << endl;
            turnCorrection(rebound_angle);
          } else {
            telem << "Ok to move forward 25 cm" << endl;
            panServo.write(panZero);
          }
        } else if(camAngles == 99) {
          telem << "No Gaps - 100% Obstacle !" << endl;
          Select_Direction();
          telem << "Heading from (DC): " << endl;
          turnCorrection(rebound_angle);    
        }
     }
  }
}

void turnCorrection(float delta_heading){
  compass_update();
  float new_heading = yar_heading - delta_heading;
  if(new_heading > 360.0f)
     new_heading -= 360.0f;
  else if(new_heading < 0.0f)
    new_heading = 360.0f + new_heading;

  telem << "Current heading: " << yar_heading << endl;
  telem << "Actual calc for new heading: " << new_heading << endl;

  pivotTo(new_heading);

  delay(1000);
}

void compass_update() {
    sensors_event_t event;
    bno.getEvent(&event);
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    imu::Vector<3> accel_linear = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    imu::Vector<3> rot_rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    
    roll = (float)event.orientation.y;
    pitch = (float)event.orientation.z;
    yar_heading = (float)event.orientation.x;

    gyroz = (float) rot_rate.z();
    accelx = (float) accel_linear.x();
    accely = (float) accel_linear.y();

    // Adjust heading to account for declination
    wp_heading = yar_heading;
    wp_heading += DEC_ANGLE;
    
    //telem << "Compass Yar/dec Heading: " << yar_heading << endl;
    
    // Correct for when signs are reversed.
    if(wp_heading < 0)
      wp_heading += 360.;
    
    // Check for wrap due to addition of declination.
    if(wp_heading > 360.)
      wp_heading -= 360.;
    
    //telem << roll << "\t" << pitch << "\t" << yar_heading << endl;
    //telem << "Changed heading: " << yar_heading << endl;
   
  }  


  
// Curtesy of
// https://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


int getIndexOfMaximumValue(float* array, int size){
  int maxIndex = 0;
  float maxx = array[maxIndex];
  for (int i=1; i<size; i++){
    if (maxx < array[i]){
      maxx = array[i];
      maxIndex = i;
    }
  }
  return maxIndex;
}

int getDups(int* arr, int size)
{
  for(int i = 0; i < size; ++i)
    for(int j = i+1; j < size;)
    {
      if(arr[i] == arr[j])
      {
        for(k = j; k < size-1; ++k)
            arr[k] = arr[k+1];
        --size;
      } else {
        ++ j;
      }
    }

    return size;
}


