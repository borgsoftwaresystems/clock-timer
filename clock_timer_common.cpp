#include "clock_timer_common.hpp"

#include "arc.hpp"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "ctime"

extern "C" {
  #include "hardware/rtc.h"
  #include "pico/util/datetime.h"
}

#include "pico_explorer.hpp"
#include "pico_display.hpp"

#include "libraries/pico_graphics/gfxfont.h"
#include "fonts/FreeMono9pt7b.h"
#include "fonts/FreeMono24pt7b.h"
#include "fonts/FreeSans24pt7b.h"
#include <inttypes.h>

using namespace pimoroni;

const int BUTTON_DEBOUNCE_PAUSE = 200;

uint16_t BLACK;
uint16_t WHITE;
uint16_t RED;
uint16_t GREEN;
uint16_t YELLOW;

bool modeIsSimple = true;
bool showTimer = false;
int showTimerAlarmCompleteCountDown = 0;
bool timerRunning = false;
bool showingAlarmLEDOn = false;

alarm_id_t alarmId = -1;
uint64_t timerLastChecked = 0;
uint64_t timerLeft = 0;
uint64_t timerDuration = 0;

void drawClockFace(PicoGraphics display) {
    // display.set_pen(100, 100, 100);    
    // display.text("12", Point(55, 10), 240, 1);
    // display.text("1", Point(91, 23), 240, 1);
    // display.text("2", Point(109, 41), 240, 1); 
    // display.text("3", Point(120, 72), 240, 1);
    // display.text("4", Point(111, 98), 240, 1);
    // display.text("5", Point(91, 116), 240, 1);    
    // display.text("6", Point(62, 130), 240, 1);
    // display.text("7", Point(30, 116), 240, 1);
    // display.text("8", Point(10, 97), 240, 1);
    // display.text("9", Point(3, 72), 240, 1);
    // display.text("10", Point(3, 41), 240, 1);
    // display.text("11", Point(26, 17), 240, 1);    
}

void drawTimerIcon(PicoGraphics display) {
  int screenHeight = 240; //Sized for Pico Explorer display

  if (forDisplay) {
    screenHeight = 134; //Sized for Pico Display Pack
  }

  display.set_pen(WHITE);

  if (timerLeft == 0) {
    //Stop  
  }
  else if (timerRunning) {
    //Play
    display.triangle(Point((int)(screenHeight/2)-5, (int)(screenHeight/2)-5),
      Point((int)(screenHeight/2)-5, (int)(screenHeight/2)+5),
      Point((int)(screenHeight/2)+5, (int)(screenHeight/2))
      );
  }
  else {
    //Pause    
    display.rectangle(Rect((int)(screenHeight/2)-3, (int)(screenHeight/2)-5, 3, 10));
    display.rectangle(Rect((int)(screenHeight/2)+3, (int)(screenHeight/2)-5, 3, 10));
  }
}

void drawClockSimple(PicoGraphics display) {
  char datetime_buf[256];
  char *datetime_str = &datetime_buf[0];
  datetime_t t;

  rtc_get_datetime(&t);

  display.set_pen(BLACK);
  display.clear();

  //Draw timer background bar
  if (timerRunning) {
    //Determine proportion
    uint64_t timeNow = 0;

    //Calculate what is left
    timeNow = time_us_64();

    if (timerLeft <= (timeNow-timerLastChecked)) { //WTF - this goes very positive when negative if I just do <=0!
      //Stop
      timerLeft = 0;
      timerRunning = false;
      timerLastChecked = 0;
      //Should I be cancelling an alarm here?
    }
    else {
      timerLeft = timerLeft-(timeNow-timerLastChecked);
      timerLastChecked = timeNow;
    }  
     
    uint64_t numSecs = timerLeft / 1000000ULL;
    
    float proportion = (float)timerDuration/(float)numSecs;

    printf("NUM SECS: %" PRIu64 "\n", numSecs);
    printf("START DURATION: %" PRIu64 "\n", timerDuration);
    
    display.set_pen(RED);
    display.rectangle(Rect(Point(0, 0), Point(240, 20)));
    display.set_pen(GREEN);
    display.rectangle(Rect(Point(0, 0), Point((int)((float)240/proportion), 20)));
  }
  else {
    if (timerLeft != 0) {
      //Paused
      display.set_pen(YELLOW);
      display.rectangle(Rect(Point(0, 0), Point(240, 20)));
    }   
  }

  //Show time
  display.customFontSetFont((const pimoroni::GFXfont&)FreeSans24pt7b);

  display.set_pen(WHITE);
  char buffer[50];
  sprintf(buffer, "%02d:%02d", t.hour, t.min);    
  display.text(buffer, Point(2, forDisplay ? 100 : 155), 240, 2);
  
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);  
}

void drawTimerSimple(PicoGraphics display) {
  char datetime_buf[256];
  char *datetime_str = &datetime_buf[0];
  datetime_t t;

  rtc_get_datetime(&t);

  display.set_pen(BLACK);
  display.clear();

  //Draw timer background bar
  if (timerRunning) {
    //Determine proportion
    uint64_t timeNow = 0;

    //Calculate what is left
    timeNow = time_us_64();

    if (timerLeft <= (timeNow-timerLastChecked)) { //WTF - this goes very positive when negative if I just do <=0!
      //Stop
      timerLeft = 0;
      timerRunning = false;
      timerLastChecked = 0;
      //Should I be cancelling an alarm here?
    }
    else {
      timerLeft = timerLeft-(timeNow-timerLastChecked);
      timerLastChecked = timeNow;
    }  
     
    uint64_t numSecs = timerLeft / 1000000ULL;
  
    float proportion = (float)timerDuration/(float)numSecs;

    printf("NUM SECS: %" PRIu64 "\n", numSecs);
    printf("START DURATION: %" PRIu64 "\n", timerDuration);
    
    display.set_pen(RED);
    display.rectangle(Rect(Point(0, 0), Point(240, 20)));
    display.set_pen(GREEN);
    display.rectangle(Rect(Point(0, 0), Point((int)((float)240/proportion), 20)));
  }
  else {
    if (timerLeft != 0) {
      //Paused
      display.set_pen(YELLOW);
      display.rectangle(Rect(Point(0, 0), Point(240, 20)));
    }   
  }

  //Show timer

  uint64_t temp;
  uint8_t numHours = 0;
  uint8_t numMins = 0;
  uint8_t numSecs2 = 0;

  temp = timerLeft / 1000000ULL;
  numHours = temp / (3600);
  temp -= (numHours * (3600));
  numMins = temp / (60);
  temp -= (numMins * (60));
  numSecs2 = temp;

  printf("Hour: %" PRIu64 "\n", numHours);
  printf("Min: %" PRIu64 "\n", numMins);
  printf("Sec: %" PRIu64 "\n", numSecs2);

  display.customFontSetFont((const pimoroni::GFXfont&)FreeSans24pt7b);

  display.set_pen(WHITE);
  char buffer[100];

  if (numSecs2 % 2 == 0)
  {
    sprintf(buffer, "%02d:%02d.%02d", numHours, numMins, numSecs2);
  }
  else
  {
    sprintf(buffer, "%02d:%02d.%02d", numHours, numMins, numSecs2);
  }
    
  display.text(buffer, Point(30, forDisplay ? 85 : 140), 240, 1);
  
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);
}

void drawClock(PicoGraphics display) {
  char datetime_buf[256];
  char *datetime_str = &datetime_buf[0];
  datetime_t t;

  rtc_get_datetime(&t);

  int screenHeight = 240; //Sized for Pico Explorer display

  if (forDisplay) {
    screenHeight = 134; //Sized for Pico Display Pack
  }
  
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono24pt7b);

  int hourRadius = 100;
  int minRadius = 75;
  int secRadius = 45;
  int tickRadius = 108;
  
  int hourThickness = 24;
  int minThickness = 24;
  int secThickness = 12;
  int tickThickness = 5;

  //Timer arcs
  Arc arc1 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(hourRadius*(screenHeight/240.0)), (int)(hourThickness*(screenHeight/240.0)), 0, 0, 128, 0, 0);
  Arc arc2 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(minRadius*(screenHeight/240.0)), (int)(minThickness*(screenHeight/240.0)), 0, 0, 0, 128, 0);
  Arc arc3 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(secRadius*(screenHeight/240.0)), (int)(secThickness*(screenHeight/240.0)), 0, 0, 0, 0, 128);
  Arc arc4 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(hourRadius*(screenHeight/240.0)), (int)(2*(screenHeight/240.0)), 0, 360, 128, 128, 128);

  //Ticks
  Arc arc12a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 357, 0, 128, 128, 128);
  Arc arc12b = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 0, 3, 128, 128, 128);
  Arc arc1a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 27, 33, 128, 128, 128);
  Arc arc2a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 57, 63, 128, 128, 128);
  Arc arc3a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 87, 93, 128, 128, 128);
  Arc arc4a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 117, 123, 128, 128, 128);
  Arc arc5a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 147, 153, 128, 128, 128);
  Arc arc6a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 177, 183, 128, 128, 128);
  Arc arc7a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 207, 213, 128, 128, 128);
  Arc arc8a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 237, 243, 128, 128, 128);
  Arc arc9a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 267, 273, 128, 128, 128);
  Arc arc10a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 297, 303, 128, 128, 128);
  Arc arc11a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 327, 333, 128, 128, 128); 
 
  arc1.setEndingAngle(Arc::convertHourToDegree(t.hour));
  arc2.setEndingAngle(Arc::convertMinToDegree(t.min));
  arc3.setEndingAngle(Arc::convertMinToDegree(t.sec));

  display.set_pen(BLACK);
  display.clear();

  arc1.draw(display);
  arc2.draw(display);
  arc3.draw(display);
  arc4.draw(display);
  
  arc12a.draw(display);
  arc12b.draw(display);
  arc1a.draw(display);
  arc2a.draw(display);
  arc3a.draw(display); 
  arc4a.draw(display); 
  arc5a.draw(display); 
  arc6a.draw(display); 
  arc7a.draw(display); 
  arc8a.draw(display); 
  arc9a.draw(display); 
  arc10a.draw(display);
  arc11a.draw(display);
  
  if (!forDisplay) {
    drawClockFace(display);
  }
  
  if (forDisplay) {
    //Show time as well
    display.customFontSetFont((const pimoroni::GFXfont&)FreeMono24pt7b);

    display.set_pen(WHITE);
    char buffer[50];
    sprintf(buffer, "%02d:", t.hour);    
    display.text(buffer, Point(150, 30), 240, 1);
    sprintf(buffer, "%02d", t.min);    
    display.text(buffer, Point(150, 80), 240, 1);
    //sprintf(buffer, "%02d", t.sec);    
    //display.text(buffer, Point(150, 130), 240, 1);

    display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);
  }
    
  //DrawTimerIcon
  drawTimerIcon(display);
}

void drawTimer(PicoGraphics display) {
  uint64_t timeNow = 0;

  if (timerRunning) {
    //Calculate what is left
    timeNow = time_us_64();

    if (timerLeft <= (timeNow-timerLastChecked)) { //NOTE: - this goes very positive when negative if I just do <=0!
      //Stop
      timerLeft = 0;
      timerRunning = false;
      timerLastChecked = 0;
      //Should I be cancelling an alarm here?
    }
    else {
      timerLeft = timerLeft-(timeNow-timerLastChecked);
      timerLastChecked = timeNow;
    }
    
    printf("TIMER LEFT: %" PRIu64 "\n", timerLeft);    
  }

  uint64_t temp;
  temp = timerLeft;
  uint64_t numHours = temp / (3600*1000000ULL);
  temp -= (numHours * (3600*1000000ULL));
  uint64_t numMins = temp / (60*1000000ULL);
  temp -= (numMins * (60*1000000ULL));
  uint64_t numSecs = temp / 1000000ULL;

  printf("Hour: %" PRIu64 "\n", numHours);
  printf("Min: %" PRIu64 "\n", numMins);
  printf("Sec: %" PRIu64 "\n", numSecs);
  
  int screenHeight = 240; //Sized for Pico Explorer display

  if (forDisplay) {
    screenHeight = 134; //Sized for Pico Display Pack
  }

  int hourRadius = 100;
  int minRadius = 75;
  int secRadius = 45;
  int tickRadius = 108;
  
  int hourThickness = 24;
  int minThickness = 24;
  int secThickness = 12;
  int tickThickness = 5;

  Arc arc1 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(hourRadius*(screenHeight/240.0)), (int)(hourThickness*(screenHeight/240.0)), 0, 0, 128, 0, 0);
  Arc arc2 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(minRadius*(screenHeight/240.0)), (int)(minThickness*(screenHeight/240.0)), 0, 0, 0, 128, 0);
  Arc arc3 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(secRadius*(screenHeight/240.0)), (int)(secThickness*(screenHeight/240.0)), 0, 0, 0, 0, 128);
  Arc arc4 = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(hourRadius*(screenHeight/240.0)), (int)(2*(screenHeight/240.0)), 0, 360, 128, 128, 128);

  arc1.setEndingAngle(Arc::convertHourToDegree(numHours));
  arc2.setEndingAngle(Arc::convertMinToDegree(numMins));
  arc3.setEndingAngle(Arc::convertMinToDegree(numSecs));

  Arc arc12a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 357, 0, 128, 128, 128);
  Arc arc12b = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 0, 3, 128, 128, 128);
  Arc arc1a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 27, 33, 128, 128, 128);
  Arc arc2a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 57, 63, 128, 128, 128);
  Arc arc3a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 87, 93, 128, 128, 128);
  Arc arc4a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 117, 123, 128, 128, 128);
  Arc arc5a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 147, 153, 128, 128, 128);
  Arc arc6a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 177, 183, 128, 128, 128);
  Arc arc7a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 207, 213, 128, 128, 128);
  Arc arc8a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 237, 243, 128, 128, 128);
  Arc arc9a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 267, 273, 128, 128, 128);
  Arc arc10a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 297, 303, 128, 128, 128);
  Arc arc11a = Arc((int)(screenHeight/2), (int)(screenHeight/2), (int)(tickRadius*(screenHeight/240.0)), (int)(tickThickness*(screenHeight/240.0)), 327, 333, 128, 128, 128); 

  display.set_pen(BLACK);
  display.clear();

  arc1.draw(display);
  arc2.draw(display);
  arc3.draw(display); 
  arc4.draw(display);  

  arc12a.draw(display);
  arc12b.draw(display);
  arc1a.draw(display);
  arc2a.draw(display);
  arc3a.draw(display); 
  arc4a.draw(display); 
  arc5a.draw(display); 
  arc6a.draw(display); 
  arc7a.draw(display); 
  arc8a.draw(display); 
  arc9a.draw(display); 
  arc10a.draw(display);
  arc11a.draw(display);
    
  
  if (!forDisplay) {
    drawClockFace(display);
  }

  if (forDisplay) {
    //Show time as well
    display.customFontSetFont((const pimoroni::GFXfont&)FreeMono24pt7b);

    display.set_pen(WHITE);
    char buffer[50];
    sprintf(buffer, "%02d:", numHours);    
    display.text(buffer, Point(150, 30), 240, 1);
    sprintf(buffer, "%02d.", numMins);    
    display.text(buffer, Point(150, 80), 240, 1);
    sprintf(buffer, "%02d", numSecs);    
    display.text(buffer, Point(150, 130), 240, 1);

    display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);
  }

  //DrawTimerIcon
  drawTimerIcon(display);
}

void blankScreen(PicoGraphics display) {
  display.set_pen(BLACK);
  display.clear();
  updateDisplay();
}

int64_t timerAlarmCallback(alarm_id_t id, void *user_data) {
  printf("ALARM!\n");

  alarmId = -1;
  timerRunning = false;
  timerLeft = 0;
  timerLastChecked = 0;

  PicoGraphics display = getDisplay();

  display.set_pen(YELLOW);
  display.clear();      
  display.set_pen(BLACK);
  display.customFontSetFont((const pimoroni::GFXfont&)FreeSans24pt7b);
  display.customFontWrite("Time's up!", Point(10, forDisplay ? 70 : 135), 240, 1);
  updateDisplay();

  showTimerAlarmCompleteCountDown = 10;
  //Need this to stay on screen for 5 seconds when in clock or timer mode but cannot sleep here!
  //Note. If was in the menu mode then this will stay until the user presses any button - not ideal!
  //Cannot sleep here! It locks up.

  if (forDisplay) {
    turnOnAlarmLED();
  }

  return 0;
}

void turnOffAlarmLED() {
  if (showingAlarmLEDOn) {
    setLEDOff();
  }
}

void turnOnAlarmLED() {
  showingAlarmLEDOn = true;
  setLEDOn();
}

void cancelTimerAlarm() {
  if (alarmId != -1) {
    cancel_alarm(alarmId);
    alarmId = -1;
    printf("Alarm is cancelled!\n");
  }
}

void setTimerAlarm() {
  //Cancel any previous alarm
  cancelTimerAlarm();

  if (timerLeft != 0 && timerRunning) {
    uint64_t timeNow = time_us_64();    

    timerLeft = timerLeft-(timeNow-timerLastChecked);    
    timerLastChecked = timeNow;
    alarmId = add_alarm_in_us(timerLeft, timerAlarmCallback, NULL, false);
    printf("Alarm is set!\n");    
  }
}

void buttonPressed(uint gpio, uint32_t events) {
  static absolute_time_t lastTime = make_timeout_time_us(0);

  absolute_time_t absTime = get_absolute_time(); 

  if (absolute_time_diff_us(lastTime, absTime) < 300000) { //150000
    //On edge rise : need 500 microseconds.
    //On edge fall: (better because this is pulled up so detects immediately) but needs about 150 milliseconds. This seems quite high.
    //Ignore interrupts for this amount of time - debounce 
    return;
  }

  lastTime = absTime;

  if (showingMenu) {
    //This is dealt with elsewhere in a tight loop not an interrupt
    printf("Reject button press interrupt");
    return;
  }

  if (gpio == pimoroni::PicoExplorer::A)
  {
    setButtonInterrupts(false);
    
    printf("A pressed\n");
    showingMenu = true;  
    return;    
  }

  if (gpio == pimoroni::PicoExplorer::B)
  {
    turnOffAlarmLED();

    uint64_t timeNow = 0;

    printf("B pressed\n");
    
    if (timerLeft == 0) {
      //Timer not set so cannot pause or un-pause
      return;
    }

    timeNow = time_us_64();

    if (timerRunning) {
      timerLeft = timerLeft-(timeNow-timerLastChecked);    
    }

    timerLastChecked = timeNow;
    timerRunning = !timerRunning;

    if(timerRunning) {
      setTimerAlarm();
    }
    else {
      cancelTimerAlarm();
    }
  } 

  if (gpio == pimoroni::PicoExplorer::X)
  {
    printf("X pressed\n"); 
    //showTimer = !showTimer;
    showTimer = false;     
  }

  if (gpio == pimoroni::PicoExplorer::Y)
  {
    printf("Y pressed\n");  
    showTimer = true;     
  }
}

void resetRTC(int hour, int min) {
  // Start on Friday 5th of June 2020 15:45:00
  datetime_t t = {
    .year  = 2020,
    .month = 2,
    .day   = 27,
    .dotw  = 6, // 0 is Sunday
    .hour  = (int8_t)hour,
    .min   = (int8_t)min,
    .sec   = 0
  };

  // Start the RTC
  rtc_set_datetime(&t);  
}

void drawTimeItem(PicoGraphics display, int hour, int min, int pos, bool drawTimer) {
  char buffer[50];
  
  display.set_pen(BLACK);
  display.clear();      
  display.set_pen(WHITE);
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);

  if (drawTimer) {
    display.text("Set Timer:", Point(0, 10), 240, 1);
  }
  else {
    display.text("Set Clock:", Point(0, 10), 240, 1);
  }
  
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono24pt7b);

  sprintf(buffer, "%02d:%02d", hour, min);  
  display.text(buffer, Point(0, 70), 240, 1);
  if (pos == 1) {
    display.text("__", Point(0, 75), 240, 1);
  }
  else {
    display.text("   __", Point(0, 75), 240, 1);
  }
  
  updateDisplay(); 
}

void drawBrightnessItem(PicoGraphics display, int brightness) {  
  char buffer[50];
  
  display.set_pen(BLACK);
  display.clear();      
  display.set_pen(WHITE);
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);
  display.text("Brightness:", Point(0, 10), 240, 1);
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono24pt7b);
  sprintf(buffer, "%03d", brightness);  
  display.text(buffer, Point(0, 70), 240, 1);

  display.text("___", Point(0, 75), 240, 1);

  updateDisplay();
  return;
}

void drawModeItem(PicoGraphics display, bool isSimple) {
  display.set_pen(BLACK);
  display.clear();      
  display.set_pen(WHITE);
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);
  display.text("Mode:", Point(0, 10), 240, 1);
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);

  if (isSimple) {
    display.text("SIMPLE", Point(0, 30), 240, 1);
    display.text("Graphical", Point(0, 50), 240, 1);
  }
  else {
    display.text("Simple", Point(0, 30), 240, 1);
  display.text("GRAPHICAL", Point(0, 50), 240, 1);
  }

  updateDisplay();
  return;
}

bool showBrightnessItem(PicoGraphics display) {  
  int brightness = globalBrightness;
  drawBrightnessItem(display, brightness);
  sleep_ms(BUTTON_DEBOUNCE_PAUSE);

  while (true) {

    if(isPressedA()) {
      setBacklight(globalBrightness);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
      return false;
    }

    if(isPressedB()) {
      //Set the brightness here
      globalBrightness = brightness;
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
      return true;
    }

    if(isPressedX()) {
      brightness += 10;
      if (brightness > 200) {
        brightness = 50;
      }
      drawBrightnessItem(display, brightness);
      setBacklight(brightness);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedY()) {
      brightness -= 10;
      if (brightness < 50) {
        brightness = 200;
      }
      drawBrightnessItem(display, brightness);
      setBacklight(brightness);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }
  }    
}

bool showTimeItem(PicoGraphics display) {
  datetime_t t;

  rtc_get_datetime(&t);

  int pos = 1;
  int hour = t.hour;
  int min = t.min;

  drawTimeItem(display, hour, min, pos, false);  
  sleep_ms(BUTTON_DEBOUNCE_PAUSE);

  while (true) {
    if(isPressedA()) {
      pos--;
      if (pos < 1) {
        sleep_ms(200);    
        return false;
      }  

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedB()) {
      pos++;
      if (pos > 2) {
        //Set time
        resetRTC(hour, min);
        sleep_ms(200);
        return true;
      }

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedX()) {
      if (pos == 1) {
        hour++;
        if (hour > 23) {
          hour = 0;
        }
      }
      else {
        min++;
        if (min > 59) {
          min = 0;
        }
      }

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedY()) {
      if (pos == 1) {
        hour--;
        if (hour < 0) {
          hour = 23;
        }
      }
      else {
        min--;
        if (min < 0) {
          min = 59;
        }
      }

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }
  }  
}

bool showModeItem(PicoGraphics display) {
  bool isSimple = modeIsSimple;

  drawModeItem(display, modeIsSimple);
  sleep_ms(BUTTON_DEBOUNCE_PAUSE);

  while (true) {
    if(isPressedA()) {      
      sleep_ms(200);    
      return false;     
    }

    if(isPressedB()) {
      //SET
      modeIsSimple = isSimple;

      sleep_ms(200);
      return true;
    }
    
    if(isPressedX()) {     
      isSimple = !isSimple;

      drawModeItem(display, isSimple);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedY()) {
      isSimple = !isSimple;

      drawModeItem(display, isSimple);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }
  }  
}

bool showTimerItem(PicoGraphics display) {  
  int pos = 1;
  int hour = 0;
  int min = 0;

  drawTimeItem(display, hour, min, pos, true);  
  sleep_ms(BUTTON_DEBOUNCE_PAUSE);

  while (true) {
    if(isPressedA()) {
      pos--;
      if (pos < 1) {
        sleep_ms(200);    
        return false;
      }  

      drawTimeItem(display, hour, min, pos, true);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedB()) {
      pos++;
      if (pos > 2) {
        //Set timer
        if (hour == 0 && min == 0) {
          //no timer set
          timerLeft = 0;
          timerLastChecked = 0;
          timerRunning = false;
          timerDuration = 0;
          cancelTimerAlarm();
          
          return true;
        }

        timerLeft = (min*60*1000000ULL) + (hour*3600*1000000ULL);
        timerLastChecked = time_us_64(); 
        timerRunning = true;
        timerDuration = (hour*3600) + (min*60);
        setTimerAlarm();
        sleep_ms(200);
        return true;
      }

      drawTimeItem(display, hour, min, pos, true);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedX()) {
      if (pos == 1) {
        hour++;
        if (hour > 23) {
          hour = 0;
        }
      }
      else {
        min++;
        if (min > 59) {
          min = 0;
        }
      }

      drawTimeItem(display, hour, min, pos, true);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedY()) {
      if (pos == 1) {
        hour--;
        if (hour < 0) {
          hour = 23;
        }
      }
      else {
        min--;
        if (min < 0) {
          min = 59;
        }
      }

      drawTimeItem(display, hour, min, pos, true);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }
  }  
}

void drawMenu(PicoGraphics display, int menuItem, int pos) {
  display.set_pen(BLACK);
  display.clear();      
  display.set_pen(WHITE);
  display.customFontSetFont((const pimoroni::GFXfont&)FreeMono9pt7b);

  if (menuItem == 0) {
    if (forDisplay) {
      switch (pos) {
        case 0:
          display.text("Menu:\nSET TIMER\nSet Brightness\nSet Mode\nSet Clock", Point(0, 10), 240, 1);
          break;
        case 1:
          display.text("Menu:\nSet Timer\nSET BRIGHTNESS\nSet Mode\nSet Clock", Point(0, 10), 240, 1);
          break;
        case 2:
          display.text("Menu:\nSet Timer\nSet Brightness\nSET MODE\nSet Clock", Point(0, 10), 240, 1);
          break;  
        case 3:
          display.text("Menu:\nSet Timer\nSet Brightness\nSet Mode\nSET CLOCK", Point(0, 10), 240, 1);
          break;  
      }
    }
    else {
      switch (pos) {
        case 0:
          display.text("Menu:\nSET TIMER\nSet Mode\nSet Clock", Point(0, 10), 240, 1);
          break;
        case 1:
          display.text("Menu:\nSet Timer\nSet MODE\nSet Clock", Point(0, 10), 240, 1);
          break;
        case 2:
          display.text("Menu:\nSet Timer\nSET Mode\nSet CLOCK", Point(0, 10), 240, 1);
          break;           
      }
    }  
  }

  updateDisplay(); 
}

void showMenu(PicoGraphics display) { 
  char buffer[50];
  int menuItem = 0;
  int pos = 0;

  drawMenu(display, menuItem, pos);
  sleep_ms(BUTTON_DEBOUNCE_PAUSE);

  while (true) {    
    if(isPressedA()) {       
      showingMenu = false;
      printf("Removing menu");  
      showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode 
      blankScreen(display);
      //sleep_ms(BUTTON_DEBOUNCE_PAUSE);      
      return;
    }

    if(isPressedX()) {
      pos--;
      if (pos < 0) {        
        pos = forDisplay ? 3 : 2;
      }
      drawMenu(display, menuItem, pos);    
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedY()) {
      pos++;
      if (pos > (forDisplay ? 3 : 2)) {
        pos = 0;
      }
      drawMenu(display, menuItem, pos); 
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedB()) {
      //Set
      if (forDisplay) {
        switch (pos) {
          case 0:          
            if (showTimerItem(display)) {            
              //sleep_ms(BUTTON_DEBOUNCE_PAUSE);
              blankScreen(display);
              showingMenu = false;  
              showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode       
              printf("Removing menu");
              return; 
            }          
            break;

          case 1:
            if (showBrightnessItem(display)) {
              //sleep_ms(BUTTON_DEBOUNCE_PAUSE);
              blankScreen(display);
              showingMenu = false;    
              showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode    
              printf("Removing menu");
              return; 
            }  
                  
            break;

          case 2:
            if (showModeItem(display)) {            
              //sleep_ms(BUTTON_DEBOUNCE_PAUSE);
              blankScreen(display);
              showingMenu = false;       
              showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode  
              printf("Removing menu");
              return; 
            }          
            break;

          case 3:
            if (showTimeItem(display)) {            
              //sleep_ms(BUTTON_DEBOUNCE_PAUSE);
              blankScreen(display);
              showingMenu = false;       
              showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode  
              printf("Removing menu");
              return; 
            }          
            break;
        }
      }
      else {
        //No brightness - needs a refactor
        switch (pos) {
          case 0:          
            if (showTimerItem(display)) {            
              //sleep_ms(BUTTON_DEBOUNCE_PAUSE);
              blankScreen(display);
              showingMenu = false;  
              showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode       
              printf("Removing menu");
              return; 
            }          
            break;

          case 1:
            if (showModeItem(display)) {            
              //sleep_ms(BUTTON_DEBOUNCE_PAUSE);
              blankScreen(display);
              showingMenu = false;       
              showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode  
              printf("Removing menu");
              return; 
            }          
            break;

          case 2:
            if (showTimeItem(display)) {            
              //sleep_ms(BUTTON_DEBOUNCE_PAUSE);
              blankScreen(display);
              showingMenu = false;       
              showTimerAlarmCompleteCountDown = 0;   //Prevent pause of display of alarm when back in timer/clock mode  
              printf("Removing menu");
              return; 
            }          
            break;
        }
      }
      
      drawMenu(display, menuItem, pos);       
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }
  }
}

void waitUntilAtBeginningOfSecond() {
  //Tight loop to wait until RTC clock has changed the second
  datetime_t t;  
  rtc_get_datetime(&t);
  int sec = t.sec;

  while (true) {
    sleep_ms(10);
    rtc_get_datetime(&t);
    if (t.sec != sec) {
      return;
    }    
  }
}


void startRTC(int hour, int min) {
  rtc_init();
  resetRTC(hour, min);
}

void showTheClockRunning(PicoGraphics display) {
  int resetInterrupts = true;  

  //sleep_until does not run as consistently (time wise) as rtc_set_alarm - quite often calls slighly ahead of time
  absolute_time_t absTime = get_absolute_time();  

  waitUntilAtBeginningOfSecond();
    
  for(;;) {
    if (showingMenu) {
      return;
    }

    if (!showingMenu) {    
      if (showTimerAlarmCompleteCountDown > 1) {
        //Do not want this interrupted so remove the interrupts
        resetInterrupts = false;
        setButtonInterrupts(false);        

        //Need to show this for 5 seconds
        showTimerAlarmCompleteCountDown--;        
      }
      else if (showTimerAlarmCompleteCountDown == 1) {
        showTimerAlarmCompleteCountDown--;  

        if (forDisplay) {
          //pico_explorer.set_led(128, 0, 0);
          turnOffAlarmLED();
        }

        resetInterrupts = true;

        //pico_explorer.set_led(128, 0, 0);        
      }
      else if (showTimer) {

        if (modeIsSimple) {
          drawTimerSimple(display); 
        }
        else {
          drawTimer(display);
        }
      }
      else {
        if (modeIsSimple) {
          drawClockSimple(display); 
        }
        else {
          drawClock(display);(display);
        }            
      }
            
      updateDisplay();
    }

    //Set up interrupts for buttons  
    if (resetInterrupts) {
      resetInterrupts = false;
      setButtonInterrupts(true);
    }

    absTime = delayed_by_ms(absTime, 1000);
    sleep_until(absTime);    
  }
}

void setButtonInterrupts(bool enabled) {
  gpio_set_irq_enabled_with_callback(pimoroni::PicoExplorer::A, 4, enabled, buttonPressed);
  gpio_set_irq_enabled_with_callback(pimoroni::PicoExplorer::B, 4, enabled, buttonPressed);
  gpio_set_irq_enabled_with_callback(pimoroni::PicoExplorer::X, 4, enabled, buttonPressed);
  gpio_set_irq_enabled_with_callback(pimoroni::PicoExplorer::Y, 4, enabled, buttonPressed); 

  //Red
  gpio_set_irq_enabled_with_callback(0, 4, enabled, buttonPressed); 

  //Green
  gpio_set_irq_enabled_with_callback(1, 4, enabled, buttonPressed); 
}

void setClockTime(PicoGraphics display, int& hour, int &min) {
  int pos = 1;  

  drawTimeItem(display, hour, min, pos, false);
  sleep_ms(BUTTON_DEBOUNCE_PAUSE);

  while (true) {

    if(isPressedA()) {
      pos--;
      if (pos < 1) {
        pos = 1;
      }  

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedB()) {
      pos++;
      if (pos > 2) {
        blankScreen(display);

        buttonPressed(0, 0); //All this does is set the static so that the first button press is not ignored
        return;
      }

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedX()) {
      if (pos == 1) {
        hour++;
        if (hour > 23) {
          hour = 0;
        }
      }
      else {
        min++;
        if (min > 59) {
          min = 0;
        }
      }

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }

    if(isPressedY()) {
      if (pos == 1) {
        hour--;
        if (hour < 0) {
          hour = 23;
        }
      }
      else {
        min--;
        if (min < 0) {
          min = 59;
        }
      }

      drawTimeItem(display, hour, min, pos, false);
      sleep_ms(BUTTON_DEBOUNCE_PAUSE);
    }
  }
}

void setupColours(PicoGraphics display) {
  if (forDisplay) {
    BLACK = display.create_pen(0, 0, 0);
    WHITE = display.create_pen(255, 255, 255);
    RED = display.create_pen(255, 0, 0);
    GREEN = display.create_pen(0, 255, 0);
    YELLOW = display.create_pen(255, 255, 0);
  }
  else {
    BLACK = display.create_pen(0, 0, 0);
    WHITE = display.create_pen(128, 128, 128);
    RED = display.create_pen(128, 0, 0);
    GREEN = display.create_pen(0, 128, 0);
    YELLOW = display.create_pen(128, 128, 0);
  }
}
