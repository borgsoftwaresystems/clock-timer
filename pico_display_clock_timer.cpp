#include "clock_timer_common.hpp"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include <math.h>

extern "C" {
  #include "hardware/rtc.h"
  #include "pico/util/datetime.h"
}

#include "pico_display.hpp"

#include "ctime"

#include "libraries/pico_graphics/gfxfont.h"
#include "fonts/FreeMono9pt7b.h"
#include "fonts/FreeMono24pt7b.h"
#include "fonts/FreeSans24pt7b.h"
#include <inttypes.h>

using namespace pimoroni;

uint16_t bufferD[PicoDisplay::WIDTH * PicoDisplay::HEIGHT]; 
PicoDisplay pico_display(bufferD);

int globalBrightness = 100;
bool showingMenu = false;
bool forDisplay = true;

void updateDisplay() {
  pico_display.update();
}

bool isPressedA() {
  return pico_display.is_pressed(pico_display.A);
}

bool isPressedB() {
  return pico_display.is_pressed(pico_display.B);
}

bool isPressedX() {
  return pico_display.is_pressed(pico_display.X);
}

bool isPressedY() {
  return pico_display.is_pressed(pico_display.Y);
}

void setBacklight(int brightness) {
  pico_display.set_backlight(brightness);
}

PicoGraphics getDisplay() {
  return pico_display;
}

void setLEDOn() {
  pico_display.set_led(128, 0, 0);
}

void setLEDOff() {
  pico_display.set_led(0, 0, 0);
}

int main() {
  stdio_init_all();

  pico_display.init();
  pico_display.set_backlight(globalBrightness);
  printf("Hello GPIO IRQ\n");

  setupColours(pico_display);

  int hour = 0;
  int min = 0;

  setClockTime(pico_display, hour, min);

  startRTC(hour, min);  //Not as accurate when the next second shows but no problem with propagating beyond 60 seconds 

  while (true) {    
    if (showingMenu) {
      showMenu(pico_display);
    }
    else {
      showTheClockRunning(pico_display);
    }
  }
}