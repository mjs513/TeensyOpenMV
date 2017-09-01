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
//

void BNO055_Init() {
  
    delay(5000);
    telem.println("Orientation Sensor Test"); telem.println("");

    /* Initialise the sensor */
    if (!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        telem.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1);
    }

    int eeAddress = 0;
    long bnoID;

    EEPROM.get(eeAddress, bnoID);

    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;

    /*
    *  Look for the sensor's unique ID at the beginning oF EEPROM.
    *  This isn't foolproof, but it's better than nothing.
    */
    bno.getSensor(&sensor);
    if (bnoID != sensor.sensor_id)
    {
        telem.println("\nNo Calibration Data for this sensor exists in EEPROM");
        delay(500);
    }
    else
    {
        telem.println("\nFound Calibration for this sensor in EEPROM.");
        eeAddress += sizeof(long);
        EEPROM.get(eeAddress, calibrationData);

        displaySensorOffsets(calibrationData);

        telem.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets(calibrationData);

        telem.println("\n\nCalibration data loaded into BNO055");
    }

    delay(1000);

    /* Display some basic information on this sensor */
    displaySensorDetails();

    /* Optional: Display current status */
    displaySensorStatus();

    bno.setExtCrystalUse(true);

    sensors_event_t event;
    bno.getEvent(&event);
 
    telem.println("\n--------------------------------\n");
    delay(500);
    
}

void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  telem.println("------------------------------------");
  telem.print  ("Sensor:       "); telem.println(sensor.name);
  telem.print  ("Driver Ver:   "); telem.println(sensor.version);
  telem.print  ("Unique ID:    "); telem.println(sensor.sensor_id);
  telem.print  ("Max Value:    "); telem.print(sensor.max_value); telem.println(" xxx");
  telem.print  ("Min Value:    "); telem.print(sensor.min_value); telem.println(" xxx");
  telem.print  ("Resolution:   "); telem.print(sensor.resolution); telem.println(" xxx");
  telem.println("------------------------------------");
  telem.println("");
  delay(500);
}

/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
void displaySensorStatus(void)
{
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the telem Monitor */
  telem.println("");
  telem.print("System Status: 0x");
  telem.println(system_status, HEX);
  telem.print("Self Test:     0x");
  telem.println(self_test_results, HEX);
  telem.print("System Error:  0x");
  telem.println(system_error, HEX);
  telem.println("");
  delay(500);
}

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  telem.print("\t");
  if (!system)
  {
    telem.print("! ");
  }

  /* Display the individual values */
  telem.print("Sys:");
  telem.print(system, DEC);
  telem.print(" G:");
  telem.print(gyro, DEC);
  telem.print(" A:");
  telem.print(accel, DEC);
  telem.print(" M:");
  telem.print(mag, DEC);
}

/**************************************************************************/
/*
    Display the raw calibration offset and radius data
    */
/**************************************************************************/
void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData)
{
    telem.print("Accelerometer: ");
    telem.print(calibData.accel_offset_x); telem.print(" ");
    telem.print(calibData.accel_offset_y); telem.print(" ");
    telem.print(calibData.accel_offset_z); telem.print(" ");

    telem.print("\nGyro: ");
    telem.print(calibData.gyro_offset_x); telem.print(" ");
    telem.print(calibData.gyro_offset_y); telem.print(" ");
    telem.print(calibData.gyro_offset_z); telem.print(" ");

    telem.print("\nMag: ");
    telem.print(calibData.mag_offset_x); telem.print(" ");
    telem.print(calibData.mag_offset_y); telem.print(" ");
    telem.print(calibData.mag_offset_z); telem.print(" ");

    telem.print("\nAccel Radius: ");
    telem.print(calibData.accel_radius);

    telem.print("\nMag Radius: ");
    telem.print(calibData.mag_radius);
}










