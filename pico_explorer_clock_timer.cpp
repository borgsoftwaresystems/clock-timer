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

#include "pico_explorer.hpp"

#include "ctime"

#include "libraries/pico_graphics/gfxfont.h"
#include "fonts/FreeMono9pt7b.h"
#include "fonts/FreeMono24pt7b.h"
#include "fonts/FreeSans24pt7b.h"
#include <inttypes.h>

using namespace pimoroni;

uint16_t bufferD[PicoExplorer::WIDTH * PicoExplorer::HEIGHT]; 
PicoExplorer pico_explorer(bufferD);

int globalBrightness = 100;
bool showingMenu = false;
bool forDisplay = false;

void updateDisplay() {
  pico_explorer.update();
}

bool isPressedA() {
  return pico_explorer.is_pressed(pico_explorer.A);
}

bool isPressedB() {
  return pico_explorer.is_pressed(pico_explorer.B);
}

bool isPressedX() {
  return pico_explorer.is_pressed(pico_explorer.X);
}

bool isPressedY() {
  return pico_explorer.is_pressed(pico_explorer.Y);
}

void setBacklight(int brightness) {
  //No backlight  
}

PicoGraphics getDisplay() {
  return pico_explorer;
}

void setLEDOn() {
  //No LED
}

void setLEDOff() {
  //No LED
}

int main() {
  stdio_init_all();

  pico_explorer.init();
  //pico_explorer.set_backlight(globalBrightness); //No more backlight
  printf("Hello GPIO IRQ\n");

  setupColours(pico_explorer);

  int hour = 0;
  int min = 0;

  setClockTime(pico_explorer, hour, min);

  startRTC(hour, min);  //Not as accurate when the next second shows but no problem with propagating beyond 60 seconds 

  while (true) {    
    if (showingMenu) {
      showMenu(pico_explorer);
    }
    else {
      showTheClockRunning(pico_explorer);
    }
  }
}