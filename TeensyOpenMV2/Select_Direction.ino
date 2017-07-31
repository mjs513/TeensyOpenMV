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

float angle_delta;

void Select_Direction() {
    //telem << endl << endl << "************************************" << endl;
    //telem << "Entering Select Direction Function" << endl;

    ///////////////////////////
    //
    // set local variables
    //
    ///////////////////////////
    //int i, start, left, delay_time;
    int i, start, left;
    float fit_time, new_angle, Delta_Angle, angle_delta;
    float numer, denom;
    unsigned int sonar_dist[N+1], Hist[N+1];
    unsigned int nowidegap;

    /////////////////////////////////////////////////////
    //
    // Establish vector pairs to store opening borders
    // and calculated rebound angles.
    // Requires the StandardCplusplus library in repository
    // Original version will not compile correctly in latest
    // Arduino IDEs
    //
    /////////////////////////////////////////////////////

    std::vector<std::pair<int,int> > border;
    std::pair<int,int> new_border;
    //std::vector<std::pair<float,float> > rebound; 
    float rebound_first[15], rebound_second[15];

    start = 0;
    new_angle = Delta_Angle = angle_delta = 0.0f;
    nowidegap = 1;
    
    /////////////////////////////////////////////////////////
    //
    // Rotate head by N angles defined in Constants.h
    // default is 12 sectors and 15 Degrees from 0-180 degrees
    //
    /////////////////////////////////////////////////////////

    for(i = 0; i <= N; i++)
    {
      panServo.write(i*angle);               
      delay(100);    //was 550                         

      /////////////////////////////////////////////////////////
      //
      // Read head ultrasonic sensor  
      // Head sensor already attached in rover3_controller.ino
      // This code assumes you are using the NewPing library.
      // If you can not use NewPing you will have to provide your
      // sensor read function, i.e., your own timings and distance
      // conversion.
      //
      /////////////////////////////////////////////////////////

      sonar_dist[i] = sensor.readRangeSingleMillimeters()/10; 
      if(sonar_dist[i] == 0) sonar_dist[i] = MIN_DISTANCE;

      /////////////////////////////////////////////////////////
      //
      // Essentially creating a binary histogram of obstacles
      // 
      /////////////////////////////////////////////////////////
      //telem << "GAP TEST:  ";
      if(sonar_dist[i] <= obsDist) 
        Hist[i] = 1;
       else
        Hist[i] = 0;
        //telem << i*angle << ", ";
        //telem <<  Hist[i] << ", " << sonar_dist[i] << endl;    
    } 
    //telem << endl;
    
    panServo.write(panZero);
    delay(100);
    
    /////////////////////////////////////////////////////////
    //
    // Start GAP analysis based of vFH
    // Code based on select_direction from VFH_algorithm.cpp
    // of Orca Robot Components
    //
    /////////////////////////////////////////////////////////

    for(i = 0;i <= N;i++) 
    {
      if (Hist[i] == 1) 
      {
          start = i;
          break;
      }
    }
    
    ///////////////////////////////////////////////////
    //
    // Find the left and right borders of each opening
    //
    ///////////////////////////////////////////////////

    border.clear();

    telem << "START: " << start << endl;
    start = 0;
    left = 1;
    for(i=start;i<=(start+N);i++) {
      if ((Hist[i % (N+1)] == 0) && (left)) {
        new_border.first = (i % (N+1)) * angle;
        left = 0;
        //telem << "A-BORDER: " << new_border.first << endl;
      }

      if ((Hist[i % (N+1)] == 1) && (!left)) {
        new_border.second = ((i % (N+1)) - 1) * angle;
        border.push_back(new_border);
        left = 1;
        //telem << "B-BORDER: " << new_border.second << endl;
      }
    }
    
    if(left == 0){
      new_border.second = (N) * angle;
      border.push_back(new_border);
      left = 1;
      //telem << "B-BORDER: " << new_border.second << endl;
    }
   
    //////////////////////////////////////////////////
    //
    // Evaluate each opening - narrow or wide
    // currently looking for wide openings only while
    // still identifying narrow openings.
    //
    //////////////////////////////////////////////////

    int angle_cnt = 0;
    for(i=0;i < (int)border.size();i++) 
    {
      //telem << "BORDER: " << border[i].first << " ,   " << border[i].second << endl;
      Delta_Angle = border[i].second - border[i].first + angle;
      //telem << "DELTA ANGLE: " << Delta_Angle << endl << endl;
    
      if (fabs(Delta_Angle) < 15)   // was 60, 30
      {
        ////////////////////////////////////////////////////
        //
        // narrow opening: aim for the centre if so desired
        // 
        ////////////////////////////////////////////////////
        new_angle = border[i].first + (border[i].second - border[i].first) / 2.0;
        //telem << "DELTA ANGLE: " << Delta_Angle << endl;
        //telem << "Narrow Opening (New Angle):  " << new_angle << endl;
      } else {
        new_angle = (border[i].second - border[i].first) / 2.0;
        //telem << "Wide Opening (Center):  " << new_angle << endl;
        nowidegap = 0;
        
        ////////////////////////////////////////////////////
        //
        // Calculate Bubble rebound angles for wide opening  
        //
        ////////////////////////////////////////////////////
        numer = 0;  denom =0;
        //telem << "a-BORDER: " << border[i].first << " ,   " << border[i].second << endl;
        int start_index = border[i].first/angle;
        int end_index = border[i].second/angle;
        
        //telem << start_index << ", " << end_index << endl;
        for(int i=start_index; i <= end_index; i++) {
          numer = numer + radians(angle*i-90.)*sonar_dist[i];
          denom = denom + sonar_dist[i];
          //telem << i << ", " << numer << ", " << denom << endl;
        }

        //////////////////////////////////////////////////
        //
        // Load Rebound vector with rebound angle and 
        // average of read distances of the opening
        // this will be used as a descriminator for the
        //  prefered opening  
        //
        //////////////////////////////////////////////////        
        rebound_second[angle_cnt] = (denom / (end_index-start_index+1));
        //telem << "Average Distance: " << rebound_second[angle_cnt] <<endl;       
        rebound_first[angle_cnt] = ((numer/denom)*rad2deg);
        //telem << "REBOUND ANGLE = " << rebound_first[angle_cnt] << " degrees" << endl;
        //telem << endl;
        angle_cnt = angle_cnt + 1;
      }
    }

    //////////////////////////////////////////////////////
    //
    // Best angle based on highest average value of sensor
    // distances for opening
    //
    //////////////////////////////////////////////////////

    //if(nowidegap == 0) {
      int maxIndex = 0;
      float maxa = rebound_second[maxIndex];
      for (int i=1; i < angle_cnt; i++){
        if (maxa < rebound_second[i]){
          maxa = rebound_second[i];
          maxIndex = i;
        }
      }
   
      rebound_angle = rebound_first[maxIndex];
      telem << "Best Angle:  " << rebound_angle << endl;

      //////////////////////////////////////////////////
      //
      // read compass, calculate expected new compass
      // heading
      //
      //////////////////////////////////////////////////

      ////compass_update(); 
      //telem << "Current heading: " << yar_heading << endl; 
  
      ////float new_heading = yar_heading + (rebound_angle);
      //telem << "Actual calc for new heading: " << new_heading << endl;
  
      ////if(new_heading > 360.0f)
      ////   new_heading -= 360.0f;
      ////else if(new_heading < 0.0f)
      ////  new_heading = 360.0f + new_heading;


      //////////////////////////////////////////////////////
      //
      // Determine whether turn direction will be clockwise
      // or counterclockwise
      //
      //////////////////////////////////////////////////////
  
      //boolean clockwise;
      ////clockwise = false;
  
      ////angle_delta = abs(new_heading - yar_heading);
      ////clockwise = (rebound_angle >= 0.0f);
  
      //telem << "New Heading: " << new_heading << "  Clockwise: " << clockwise << endl;
      //telem << "Angle delta: " << angle_delta << endl;
      //telem << endl;

	  //turnDelay();  // determine turn time based on calibration 
	  
      //compass_update();

		//telem << "New Heading: " << yar_heading << endl;
//    	} else {
      //telem << "No viable (wide) gap !" << endl;
      
      // Do a clockwise 180 and return
//      float rebound_angle = 180;
      //fit_time = 0.000006*pow(rebound_angle,3)-0.0081*pow(rebound_angle,2)+7.1036*rebound_angle+84.232;
//	  fit_time = (rebound_angle+8.2371)/0.1026;
//      delay_time = ceil(fit_time);
      //telem << "180 Curve Fit (CW): " << fit_time << endl;
//      mRight();
 //     delay(delay_time);
 //   }

}












