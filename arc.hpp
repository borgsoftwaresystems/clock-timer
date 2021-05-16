#ifndef _ARC_H_
#define _ARC_H_

#include "pico_explorer.hpp"
#include "pico_display.hpp"

class Arc {
  private:
    int center_x;
    int center_y;
    int radius;
    int thickness;
    int starting_angle;
    int ending_angle;
    int colour_red;
    int colour_green;
    int colour_blue;

    float calculateAngle(int center_x, int center_y, float radius, float x, float y);

  public:
    Arc(int center_x, int center_y, int radius, int thickness, int starting_angle, int ending_angle, int colour_red, int colour_green, 
      int colour_blue) {
      this->center_x = center_x;
      this->center_y = center_y;
      this->radius = radius;
      this->thickness = thickness;
      this->starting_angle = starting_angle;
      this->ending_angle = ending_angle;
      this->colour_red = colour_red;
      this->colour_green = colour_green;
      this->colour_blue = colour_blue;
    }

    void setStartingAngle(int starting_angle);
    void setEndingAngle(int ending_angle);
    void draw(pimoroni::PicoGraphics pico_explorer);  

    static int convertHourToDegree(int hour);
    static int convertMinToDegree(int min);
};

#endif