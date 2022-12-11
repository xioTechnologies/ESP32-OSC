#include "OscDispatcher.h"

OscDispatcherClass::OscDispatcherClass() {
  numberOfAddresses = 0;
}

void OscDispatcherClass::add(const char* const address, void (*process)(const char* const typeTags, const void* const arguments)) {
  if (numberOfAddresses >= maxNumberOfAddresses) {
    return;
  }
  snprintf(addresses[numberOfAddresses], sizeof(addresses[numberOfAddresses]), "%s", address);
  processes[numberOfAddresses] = process;
  numberOfAddresses++;
}

void OscDispatcherClass::process(uint8_t* const packet, size_t packetSize) {
  const char* address;
  const char* typeTags;
  const void* arguments;
  if (parsePacket(packet, packetSize, &address, &typeTags, &arguments) != 0) {
    return;
  }
  for (int index = 0; index < numberOfAddresses; index++) {
    if (strcmp(addresses[index], address) == 0) {
      processes[index](typeTags, arguments);
    }
  }
}

int OscDispatcherClass::parsePacket(uint8_t* const oscPacket, const size_t oscPacketSize, const char** const address, const char** const typeTags, const void** const arguments) {

  // Skip bundle header
  int oscPacketIndex = 0;
  switch (oscPacket[oscPacketIndex]) {
    case '#':
      oscPacketIndex += sizeof("#bundle") + sizeof(uint64_t) + sizeof(uint32_t);
      if (oscPacketIndex >= oscPacketSize) {
        return 1;
      }
      break;
    case '/':
      break;
    default:
      return 1;
  }

  // Address
  if (oscPacket[oscPacketIndex] != '/') {
    return 1;
  }
  *address = (char*)&oscPacket[oscPacketIndex];
  if (parseString(oscPacket, oscPacketSize, &oscPacketIndex) != 0) {
    return 1;
  }

  // Type tag string
  if (oscPacket[oscPacketIndex] != ',') {
    return 1;
  }
  *typeTags = (char*)&oscPacket[oscPacketIndex + 1];
  if (parseString(oscPacket, oscPacketSize, &oscPacketIndex) != 0) {
    return 1;
  }

  // Argument
  *arguments = &oscPacket[oscPacketIndex];
  int typeTagIndex = 0;
  while ((*typeTags)[typeTagIndex] != '\0') {
    switch ((*typeTags)[typeTagIndex++]) {
      case '\0':
        break;
      case 'i':
        if (parseArgument(oscPacket, oscPacketSize, &oscPacketIndex, sizeof(uint32_t)) != 0) {
          return 1;
        }
        break;
      case 'f':
        if (parseArgument(oscPacket, oscPacketSize, &oscPacketIndex, sizeof(float)) != 0) {
          return 1;
        }
        break;
      case 's':
        if (parseString(oscPacket, oscPacketSize, &oscPacketIndex) != 0) {
          return 1;
        }
        break;
      case 't':
        if (parseArgument(oscPacket, oscPacketSize, &oscPacketIndex, sizeof(uint64_t)) != 0) {
          return 1;
        }
        break;
      case 'T':
      case 'F':
        break;
      default:
        return 1;
    }
  }
  if (oscPacketIndex != oscPacketSize) {
    return 1;
  }
  return 0;  // 0 indicates success
}

int OscDispatcherClass::parseString(const uint8_t* const source, const size_t sourceSize, int* const sourceIndex) {

  // Advance index to first termination byte
  while (source[*sourceIndex] != '\0') {
    if (++(*sourceIndex) > sourceSize) {
      return 1;
    }
  }

  // Advance index to last termination byte
  while (((*sourceIndex + 1) % 4) != 0) {
    if (++(*sourceIndex) > sourceSize) {
      return 1;
    }
    if (source[*sourceIndex] != '\0') {
      return 1;
    }
  }

  // Increment index to after last termination byte
  if (++(*sourceIndex) > sourceSize) {
    return 1;
  }
  return 0;  // 0 indicates success
}

int OscDispatcherClass::parseArgument(uint8_t* const source, const size_t sourceSize, int* const sourceIndex, const size_t numberOfBytes) {

  // Copy to buffer
  uint8_t buffer[8];
  if (numberOfBytes > sizeof(buffer)) {
    return 1;
  }
  if ((*sourceIndex + numberOfBytes) > sourceSize) {
    return 1;
  }
  memcpy(buffer, &source[*sourceIndex], numberOfBytes);

  // Copy from buffer in reverse order
  for (int bufferIndex = numberOfBytes - 1; bufferIndex >= 0; bufferIndex--) {
    source[*sourceIndex] = buffer[bufferIndex];
    if (++(*sourceIndex) > sourceSize) {
      return 1;
    }
  }
  return 0;  // 0 indicates success
}

OscDispatcherClass OscDispatcher;
