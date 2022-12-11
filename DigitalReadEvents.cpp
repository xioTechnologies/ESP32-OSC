#include "DigitalReadEvents.h"

DigitalReadEventsClass::DigitalReadEventsClass() {
  numberOfEvents = 0;
}

void DigitalReadEventsClass::add(const int pin, void (*event)(const bool state)) {
  if (numberOfEvents >= maxNumberOfEvents) {
    return;
  }
  pins[numberOfEvents] = pin;
  previousStates[numberOfEvents] = false;
  timeouts[numberOfEvents] = 0;
  events[numberOfEvents] = event;
  numberOfEvents++;
}

void DigitalReadEventsClass::tasks() {
  for (int index = 0; index < numberOfEvents; index++) {
    const bool state = digitalRead(pins[index]);
    if (state == previousStates[index]) {
      continue;
    }
    const unsigned long time = millis();
    if (time < timeouts[index]) {
      continue;
    }
    previousStates[index] = state;
    timeouts[index] = time + 20;  // 20 ms holdoff for debouncing
    events[index](state);
  }
}

DigitalReadEventsClass DigitalReadEvents;
