#ifndef OSC_DISPATCHER_H
#define OSC_DISPATCHER_H

#include "Arduino.h"

class OscDispatcherClass {

public:
  OscDispatcherClass();

  void add(const char* const address, void (*process)(const char* const typeTags, const void* const arguments));
  void process(uint8_t* const packet, size_t packetSize);

private:
  static const int maxNumberOfAddresses = 16;
  int numberOfAddresses;
  char addresses[maxNumberOfAddresses][32];
  void (*processes[maxNumberOfAddresses])(const char* const typeTags, const void* const arguments);

  static int parsePacket(uint8_t* const packet, const size_t packetSize, const char** const address, const char** const typeTags, const void** const arguments);
  static int parseString(const uint8_t* const source, const size_t sourceSize, int* const sourceIndex);
  static int parseArgument(uint8_t* const source, const size_t sourceSize, int* const sourceIndex, const size_t numberOfBytes);
};

extern OscDispatcherClass OscDispatcher;

#endif
