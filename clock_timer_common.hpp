#ifndef _CLOCK_TIMER_COMMON_H_
#define _CLOCK_TIMER_COMMON_H_

#include "pico_explorer.hpp"
#include "pico_display.hpp"

//#include "arc.hpp"

extern void updateDisplay();
extern bool isPressedA();
extern bool isPressedB();
extern bool isPressedX();
extern bool isPressedY();
extern void setBacklight(int brightness);
extern pimoroni::PicoGraphics getDisplay();
extern void setLEDOn();
extern void setLEDOff();

extern bool showingMenu;
extern int globalBrightness;
extern bool forDisplay;

void showMenu(pimoroni::PicoGraphics display);
void setButtonInterrupts(bool enabled);
void turnOnAlarmLED();

void setupColours(pimoroni::PicoGraphics display);
void setClockTime(pimoroni::PicoGraphics display, int& hour, int &min);
void startRTC(int hour, int min);
void showTheClockRunning(pimoroni::PicoGraphics display);

#endif