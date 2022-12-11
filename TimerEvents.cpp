#include "TimerEvents.h"

TimerEventsClass::TimerEventsClass() {
  numberOfEvents = 0;
}

void TimerEventsClass::add(unsigned long period, void (*event)()) {
  if (numberOfEvents >= maxNumberOfEvents) {
    return;
  }
  periods[numberOfEvents] = period;
  timeouts[numberOfEvents] = millis();
  events[numberOfEvents] = event;
  numberOfEvents++;
}

void TimerEventsClass::tasks() {
  const unsigned long time = millis();
  for (int index = 0; index < numberOfEvents; index++) {
    if (time >= timeouts[index]) {
      timeouts[index] = time + periods[index];
      events[index]();
    }
  }
}

TimerEventsClass TimerEvents;
