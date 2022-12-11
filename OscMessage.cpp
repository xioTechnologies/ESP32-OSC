#include "OscMessage.h"

OscMessage::OscMessage(const char* const address, const char* typeTags, ...) {
  packetSize = 0;

  // Address
  int packetIndex = 0;
  if (copy(packet, sizeof(packet), &packetIndex, address, strlen(address)) != 0) {
    return;
  }
  if (terminate(packet, sizeof(packet), &packetIndex) != 0) {
    return;
  }

  // Type tag string
  if (copy(packet, sizeof(packet), &packetIndex, ",", 1) != 0) {
    return;
  }
  if (copy(packet, sizeof(packet), &packetIndex, typeTags, strlen(typeTags)) != 0) {
    return;
  }
  if (terminate(packet, sizeof(packet), &packetIndex) != 0) {
    return;
  }

  // Arguments
  va_list arguments;
  va_start(arguments, typeTags);
  while (*typeTags != '\0') {
    switch (*(typeTags++)) {
      case 'i':
        {
          const int32_t number = va_arg(arguments, int32_t);
          if (copyReversed(packet, sizeof(packet), &packetIndex, &number, sizeof(number)) != 0) {
            return;
          }
          break;
        }
      case 'f':
        {
          const double float64 = va_arg(arguments, double);
          float float32 = (float)float64;
          if (copyReversed(packet, sizeof(packet), &packetIndex, &float32, sizeof(float32)) != 0) {
            return;
          }
          break;
        }
      case 's':
        {
          const char* const string = (const char* const)va_arg(arguments, void*);
          if (copy(packet, sizeof(packet), &packetIndex, string, strlen(string)) != 0) {
            return;
          }
          if (terminate(packet, sizeof(packet), &packetIndex) != 0) {
            return;
          }
          break;
        }
      case 't':
        {
          const int64_t timeTag = va_arg(arguments, int64_t);
          if (copyReversed(packet, sizeof(packet), &packetIndex, &timeTag, sizeof(timeTag)) != 0) {
            return;
          }
          break;
        }
      case 'T':
        break;
      case 'F':
        break;
      default:
        return;
    }
  }
  va_end(arguments);
  packetSize = packetIndex;
}

int OscMessage::copy(uint8_t* const destination, const size_t destinationSize, int* const destinationIndex, const void* const source, const size_t numberOfBytes) {
  if ((*destinationIndex + numberOfBytes) > destinationSize) {
    return 1;
  }
  memcpy(&destination[*destinationIndex], source, numberOfBytes);
  *destinationIndex += numberOfBytes;
  return 0;  // 0 indicates success
}

int OscMessage::copyReversed(uint8_t* const destination, const size_t destinationSize, int* const destinationIndex, const void* const source, const size_t numberOfBytes) {
  if ((*destinationIndex + numberOfBytes) > destinationSize) {
    return 1;
  }
  for (int sourceIndex = numberOfBytes - 1; sourceIndex >= 0; sourceIndex--) {
    destination[(*destinationIndex)++] = ((uint8_t*)source)[sourceIndex];
  }
  return 0;  // 0 indicates success
}

int OscMessage::terminate(uint8_t* const destination, const size_t destinationSize, int* const destinationIndex) {
  do {
    if (*destinationIndex >= destinationSize) {
      return 1;
    }
    destination[(*destinationIndex)++] = '\0';
  } while ((*destinationIndex % 4) != 0);
  return 0;  // 0 indicates success
}

const uint8_t* OscMessage::getPacket() {
  return packet;
}

const size_t OscMessage::getPacketSize() {
  return packetSize;
}
