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

//**********************************************************
void attach_encoders(){
  attachInterrupt(l_encoder, revs_count_l, CHANGE);   
  attachInterrupt(r_encoder, revs_count_r, CHANGE);
}


void detach_encoders(){
  detachInterrupt(l_encoder);   
  detachInterrupt(r_encoder); 
}

void init_ticks_counter(){
  kcount_revs_l = 0;
  kcount_revs_r = 0;
  ticksRR = 0;
  ticksLR = 0;
}

void get_ticks_noreset(){
  ticksRR = kcount_revs_r/rt_mtr_adj;
  ticksLR = kcount_revs_l/lf_mtr_adj;
  //telem << ticksLR << ", " << ticksRR << ", ";
}

void get_ticks_reset(){
  ticksRR = kcount_revs_r/rt_mtr_adj;
  ticksLR = kcount_revs_l/lf_mtr_adj;
  kcount_revs_l = 0;
  kcount_revs_r = 0;
}

 void revs_count_l()
 {
   kcount_revs_l++;
   //Each rotation, this interrupt function is run twice
 }

 void revs_count_r()
 {
   kcount_revs_r++;
   //Each rotation, this interrupt function is run twice
 }


//**********************************************************
//IsTime() function - David Fowler, AKA uCHobby, http://www.uchobby.com 01/21/2012

#define TIMECTL_MAXTICKS  4294967295L
#define TIMECTL_INIT      0

int IsTime(unsigned long *timeMark, unsigned long timeInterval){
  unsigned long timeCurrent;
  unsigned long timeElapsed;
  int result=false;
  
  timeCurrent=millis();
  if(timeCurrent<*timeMark) {  //Rollover detected
    timeElapsed=(TIMECTL_MAXTICKS-*timeMark)+timeCurrent;  //elapsed=all the ticks to overflow + all the ticks since overflow
  }
  else {
    timeElapsed=timeCurrent-*timeMark;  
  }

  if(timeElapsed>=timeInterval) {
    *timeMark=timeCurrent;
    result=true;
  }
  return(result);  
}
