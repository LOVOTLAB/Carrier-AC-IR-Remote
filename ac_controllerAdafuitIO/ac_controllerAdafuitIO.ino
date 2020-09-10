// Adafruit IO Subscription Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/

// set up the 'counter' feed
AdafruitIO_Feed *acpower = io.feed("acpower");
AdafruitIO_Feed *actemp = io.feed("actemp");
AdafruitIO_Feed *acmode = io.feed("acmode");
AdafruitIO_Feed *acindicator = io.feed("acindicator");

int o, t, m = 0;

// IR Remote
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
int count = 0;

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData[99] = {};

// DoorSwitch
int lastStatus = 0;

void setup() {
  // IR Remote
  irsend.begin();
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
  for (int i = 0; i < 99; i++) {
    if (i % 2 == 1) rawData[i] = 500;
    else rawData[i] = 550;
  }

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);

  Serial.print("Connecting to Adafruit IO");

  // start MQTT connection to io.adafruit.com
  io.connect();

  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  acpower->onMessage(acpowerHandleMessage);
  actemp->onMessage(actempHandleMessage);
  acmode->onMessage(acmodeHandleMessage);

  // wait for an MQTT connection
  // NOTE: when blending the HTTP and MQTT API, always use the mqttStatus
  // method to check on MQTT connection status specifically
  while (io.mqttStatus() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Because Adafruit IO doesn't support the MQTT retain flag, we can use the
  // get() function to ask IO to resend the last value for this feed to just
  // this MQTT client after the io client is connected.
  acpower->get();
  actemp->get();
  acmode->get();

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  pinMode(D1, INPUT_PULLUP);
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // Because this sketch isn't publishing, we don't need
  // a delay() in the main program loop.
  int currentStatus = digitalRead(D1);
  if (lastStatus != currentStatus) {
    Serial.println(currentStatus);
    if (currentStatus) acindicator->save(1);
    else acindicator->save(0);
    lastStatus = currentStatus;
    Serial.println(currentStatus);
    delay(10);
  }
}

// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.
void acpowerHandleMessage(AdafruitIO_Data *data) {
  o = data->toInt();
  makeRemoteCode(o, m, 18, t, 0, 0);
}

void actempHandleMessage(AdafruitIO_Data *data) {
  t = data->toInt();
  makeRemoteCode(o, m, 18, t, 0, 0);
}

void acmodeHandleMessage(AdafruitIO_Data *data) {
  m = data->toInt();
  makeRemoteCode(o, m, 18, t, 0, 0);
}
