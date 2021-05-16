#include "arc.hpp"

#include <math.h>

using namespace pimoroni;

const float PI = 3.14159265;

int Arc::convertHourToDegree(int hour) {
  if(hour > 11)
  {
    hour = hour - 12;
  }
          
  return hour * 30;
}
    
int Arc::convertMinToDegree(int min) {
  return min * 6;
}

void Arc::setStartingAngle(int starting_angle) {
  this->starting_angle = starting_angle;
}

void Arc::setEndingAngle(int ending_angle) {
  this->ending_angle = ending_angle;
}

float Arc::calculateAngle(int center_x, int center_y, float radius, float x, float y) {
  float length_of_line = sqrt(pow(center_y - radius - y, 2) + pow(x - center_x, 2));
  float angle = (asin(length_of_line / 2 / radius) * 180 / PI) * 2;
        
  return angle;
}

void Arc::draw(PicoGraphics display) {
  display.set_pen(colour_red, colour_green, colour_blue);

  float fradius = float(radius);
  float fthickness = float(thickness);
  float d, x, y, angle;
  
  while(fthickness > 0) {
    d = 3 - (2 * fradius);
    x = 0.0;
    y = fradius;

    while(y > 0) {
      if(x <= y) {
        angle = calculateAngle(center_x, center_y, fradius, center_x+x, center_y-y);

        if((180-angle >= starting_angle) && (180-angle <= ending_angle)) {
          display.pixel(Point(center_x+int(x), center_y+int(y)));
        } 
                     
        if((angle >= starting_angle) && (angle <= ending_angle)) {
          display.pixel(Point(center_x+int(x), center_y-int(y)));
        }
                                 
        if((180+angle >= starting_angle) && (180+angle <= ending_angle)) {
          display.pixel(Point(center_x-int(x), center_y+int(y)));
        }
                                
        if((360-angle >= starting_angle) && (360-angle <= ending_angle)) {
          display.pixel(Point(center_x-int(x), center_y-int(y)));
        }
            
        if((90+angle >= starting_angle) && (90+angle <= ending_angle)) {
          display.pixel(Point(center_x+int(y), center_y+int(x)));
        } 
                    
        if((90-angle >= starting_angle) && (90-angle <= ending_angle)) {
          display.pixel(Point(center_x+int(y), center_y-int(x)));
        } 
                    
        if((270-angle >= starting_angle) && (270-angle <= ending_angle)) {
          display.pixel(Point(center_x-int(y), center_y+int(x)));
        }
                    
        if((270+angle >= starting_angle) && (270+angle <= ending_angle)) {
          display.pixel(Point(center_x-int(y), center_y-int(x)));
        } 
      }

      if(d < 0) {
        d = d + (4 * x) + 6;
        x = x + 1;
      }
      else {
        d = d + 4 * (x - y) + 10;
        x = x + 1;
        y = y - 1;
      }
    }

    fradius = fradius - 1;
    fthickness = fthickness - 1;    
  }
}