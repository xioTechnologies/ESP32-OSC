#ifndef DIGITAL_READ_EVENTS_H
#define DIGITAL_READ_EVENTS_H

#include "Arduino.h"

class DigitalReadEventsClass {

public:
  DigitalReadEventsClass();

  void add(const int pin, void (*event)(const bool state));
  void tasks();

private:
  static const int maxNumberOfEvents = 16;
  int numberOfEvents;
  int pins[maxNumberOfEvents];
  bool previousStates[maxNumberOfEvents];
  unsigned long timeouts[maxNumberOfEvents];
  void (*events[maxNumberOfEvents])(const bool state);
};

extern DigitalReadEventsClass DigitalReadEvents;

#endif
