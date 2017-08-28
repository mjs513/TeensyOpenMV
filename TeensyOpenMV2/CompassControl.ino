/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
void pivotTo(int target){
  
  while(1){
    compass_update();
    int currentAngle = wp_heading;
    int diff = target - currentAngle;

    if(printFlag == 1) {
      telem << "Compass Control: " << endl;
      telem << "\t" << currentAngle << ", " << target << ", " << diff << endl;
    }

    
    if(diff > 0) {
      if(printFlag == 1) telem << "Turning Right" << endl;
      throttleRight = turnSpeed;
      throttleLeft = turnSpeed;
      mRight();//right
      delay(50);
    } else {
      if(printFlag == 1) telem << "Turning Left" << endl;
      throttleRight = turnSpeed;
      throttleLeft = turnSpeed;
      mLeft();//left
      delay(50);
    }
    
    if(abs(diff) < HEADING_TOLERANCE){
      mStop();
      return;
    }
  }
  mStop();
}









