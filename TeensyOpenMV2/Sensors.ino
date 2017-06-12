void readSensors()
{
   float new_heading;
   uint8_t numAngles, indexBestAngle;
   float camAngles;
  
   if(telem2.available() > 0) {
      for(i = 0; i < 3; i++){
        str1 = telem2.readStringUntil('\n');
        telem << str1 << endl;
      }
      
      numAngles = getValue(str1, ',', 0).toInt();       //Get number of gaps
      if(numAngles > 0){
        for(i = 1; i < numAngles+1; i++){
          camAngles = getValue(str1, ',', i).toInt();   //Get gap angles
          gapAngle[i-1] = camAngles;
          panServo.write(panZero + camAngles);          //Move servo to first gap
          delay(100);
          
          telem << "Gap Angle: " << camAngles << ", Gap Distance: ";
          gapDist[i-1] = sensor.readRangeSingleMillimeters()/10;
          if (sensor.timeoutOccurred()) { 
            telem << " TIMEOUT"; 
            return;
          }
          telem << gapDist[i-1] << " -- ";
          telem << endl;        
        }

        panServo.write(panZero);
        
        indexBestAngle = getIndexOfMaximumValue(gapDist, numAngles);
        telem << indexBestAngle << ", " << gapAngle[indexBestAngle] << ", " << gapDist[indexBestAngle] << endl;
        
        if(gapDist[indexBestAngle] < obsDist) {
          Select_Direction();
          compass_update(); 
          new_heading = yar_heading - rebound_angle;
          telem << "Heading from (CAMSD): " << endl;
          turnCorrection(rebound_angle);
        } else {
          telem << "Heading from (cam): " << endl;
          turnCorrection(gapAngle[indexBestAngle]); 
        }
        
      } else {
        telem << "NO VISIABLE GAP!" << endl;
        if(sensor.readRangeSingleMillimeters()/10 < obsDist){ 
          Select_Direction();
          telem << "Heading from (DC): " << endl;
          turnCorrection(rebound_angle);
        } else {
          telem << "Ok to move forward 25 cm" << endl;
          panServo.write(panZero);
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



