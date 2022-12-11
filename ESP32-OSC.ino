#include <AsyncUDP.h>
#include "DigitalReadEvents.h"
#include "OscDispatcher.h"
#include "OscMessage.h"
#include "TimerEvents.h"
#include <WiFi.h>

const char networkName[] = "network name";             // your Wi-Fi router network name
const char password[] = "password";                    // your Wi-Fi router password
const IPAddress remoteIP = IPAddress(192, 168, 1, 2);  // your computer's IP address
const uint16_t sendPort = 8000;                        // the port the ESP32 sends to and your computer receives on
const uint16_t receivePort = 9000;                     // the port your computer sends to and the ESP32 receives on

const int button1Pin = 14;
const int button2Pin = 12;
const int adc1Pin = 36;
const int adc2Pin = 39;
const int adc3Pin = 34;
const int redLedPin = 32;
const int greenLedPin = 33;
const int blueLedPin = 25;
const int connectedLedPin = 26;

bool connected;
AsyncUDP udp;

void setup() {
  Serial.begin(115200);

  WiFi.onEvent(wifiEvent);
  WiFi.begin(networkName, password);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(connectedLedPin, OUTPUT);

  DigitalReadEvents.add(button1Pin, sendButton1);
  DigitalReadEvents.add(button2Pin, sendButton2);

  TimerEvents.add(10, sendAdc);              // send ADC values at 100 Hz
  TimerEvents.add(100, updateConnectedLed);  // update connected LED every 100 ms

  OscDispatcher.add("/rgb", processRgb);
  OscDispatcher.add("/print", processPrint);
}

void loop() {
  TimerEvents.tasks();
  DigitalReadEvents.tasks();
}

void wifiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("Connected");
      Serial.println(WiFi.localIP());
      connected = true;
      udp.listen(receivePort);
      udp.onPacket([](AsyncUDPPacket packet) {
        OscDispatcher.process(packet.data(), packet.length());
      });
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected");
      connected = false;
      break;
    default:
      break;
  }
}

void sendButton1(const bool state) {
  sendOscMessage(OscMessage("/button1", state ? "T" : "F"));  // send button 1 state as boolean
}

void sendButton2(const bool state) {
  sendOscMessage(OscMessage("/button2", "s", state ? "true" : "false"));  // send button 2 state as string
}

void sendAdc() {
  sendOscMessage(OscMessage("/adc", "fff", (float)analogRead(adc1Pin), (float)analogRead(adc2Pin), (float)analogRead(adc3Pin)));
}

void sendOscMessage(OscMessage message) {
  if (connected == true) {
    udp.writeTo(message.getPacket(), message.getPacketSize(), remoteIP, sendPort);
  }
}

void processRgb(const char* const typeTags, const void* const arguments) {
  if (strcmp(typeTags, "iii") != 0) {
    return;  // do nothing if arguments are not int, int, int
  }
  const int* const rgb = (const int* const)arguments;
  analogWrite(redLedPin, rgb[0]);
  analogWrite(greenLedPin, rgb[1]);
  analogWrite(blueLedPin, rgb[2]);
}

void processPrint(const char* const typeTags, const void* const arguments) {
  if (strcmp(typeTags, "s") != 0) {
    return;  // do nothing if argument is not a string
  }
  Serial.println((char*)arguments);
}

void updateConnectedLed() {
  if (connected) {
    digitalWrite(connectedLedPin, HIGH);  // LED on if connected
  } else {
    digitalWrite(connectedLedPin, !digitalRead(connectedLedPin));  // flash LED if not connected
  }
}
