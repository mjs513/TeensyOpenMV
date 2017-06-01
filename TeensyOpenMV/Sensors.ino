void readSensors()
{
   if(telem2.available()) {
      for(i = 0; i < 5; i++){
        str1 = telem2.readStringUntil('\n');
        //telem << str1 << endl;
      }
      
      j = getValue(str1, ',', 0).toInt();       //Get number of gaps
      if(j > 0){
        for(i = 1; i < j+1; i++){
          k = getValue(str1, ',', i).toInt();   //Get gap angles
          gapAngle[i-1] = k;
          panServo.write(panZero + k);          //Move servo to first gap
          delay(100);
          
          //telem << "Gap Angle: " << k << ", Gap Distance: ";
          gapDist[i-1] = sensor.readRangeSingleMillimeters()/10;
          if (sensor.timeoutOccurred()) { 
            telem << " TIMEOUT"; 
          }
          
          //telem << gapDist[i-1] << " -- ";
          //telem << endl;
        }
      } else {
        telem << "NO VISIABLE GAP!" << endl;
      }
  }
  panServo.write(panZero);

  if(j > 0){
    k = getIndexOfMaximumValue(gapDist, j);
    telem << j << ", " << gapAngle[k] << ", " << gapDist[k] << endl;
  }
  delay(2000);
  
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
  float max = array[maxIndex];
  for (int i=1; i<size; i++){
    if (max < array[i]){
      max = array[i];
      maxIndex = i;
    }
  }
  return maxIndex;
}
