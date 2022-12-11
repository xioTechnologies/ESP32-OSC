#ifndef OSC_MESSAGE_H
#define OSC_MESSAGE_H

#include "Arduino.h"

class OscMessage {

public:
  OscMessage(const char* const address, const char* typeTags, ...);

  const uint8_t* getPacket();
  const size_t getPacketSize();

private:
  uint8_t packet[2048];
  size_t packetSize;

  static int copy(uint8_t* const destination, const size_t destinationSize, int* const destinationIndex, const void* const source, const size_t numberOfBytes);
  static int copyReversed(uint8_t* const destination, const size_t destinationSize, int* const destinationIndex, const void* const source, const size_t numberOfBytes);
  static int terminate(uint8_t* const destination, const size_t destinationSize, int* const destinationIndex);
};

#endif
