#ifndef TIMER_EVENTS_H
#define TIMER_EVENTS_H

#include "Arduino.h"

class TimerEventsClass {

public:
  TimerEventsClass();

  void add(unsigned long period, void (*event)());
  void tasks();

private:
  static const int maxNumberOfEvents = 16;
  int numberOfEvents;
  unsigned long periods[maxNumberOfEvents];
  unsigned long timeouts[maxNumberOfEvents];
  void (*events[maxNumberOfEvents])();
};

extern TimerEventsClass TimerEvents;

#endif
