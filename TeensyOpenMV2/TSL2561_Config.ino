
/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void TSL2561_displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  telem << "------------------------------------" << endl;
  telem << "Sensor:       " << sensor.name << endl;
  telem << "Driver Ver:   " << sensor.version << endl;
  telem << "Unique ID:    " << sensor.sensor_id << endl;
  telem << "Max Value:    " << sensor.max_value << " lux" << endl;
  telem << "Min Value:    " << sensor.min_value << " lux" << endl;
  telem << "Resolution:   " << sensor.resolution << " lux" << endl; 
  telem << "------------------------------------" << endl;
  delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void TSL2561_configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  //Serial.println("------------------------------------");
  //Serial.print  ("Gain:         "); Serial.println("Auto");
  //Serial.print  ("Timing:       "); Serial.println("13 ms");
  //Serial.println("------------------------------------");
}

void readTSL2561(void) 
{  
  /* Get a new sensor event */ 
  sensors_event_t TSLevent;
  tsl.getEvent(&TSLevent);

  /* Display the results (light is measured in lux) */
  if (TSLevent.light)
  {
    valueLux = TSLevent.light;
    telem << valueLux << " lux" << endl;
    /* Populate broadband and infrared with the latest values */
    //tsl.getLuminosity (&broadband, &infrared);
    //Serial.print("Broadband: "); Serial.print(broadband); Serial.print(",  Infrared: "); Serial.println(infrared); 

  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    telem << "Sensor overload" << endl;
  }
  delay(50);
}




