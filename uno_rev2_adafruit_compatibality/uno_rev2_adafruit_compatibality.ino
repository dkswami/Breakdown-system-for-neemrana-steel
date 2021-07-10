// AIO_LED_Pot - AIO_LED_Pot.ino
//
// Description:
// Interfaces an LED and a potentiometer on an Arduino Uno WiFi Rev2 with the
// Adafruit IO service.
// Note: Must use Adafruit's modified version of the WiFiNINA library
// (https://github.com/adafruit/WiFiNINA), define USE_AIRLIFT, and instantiate
// AdafruitIO_WiFi with pin connections for Arduino Uno WiFi Rev2 compatability.
// NOTE: The sketch sometimes gets stuck initially connecting to the service and
// needs to be reuploaded.
//
// Circuit:
// Red LED connected to pin D2.
// 10K potentiometer connected to pin A0.
//
// Created by John Woolsey on 05/29/2019.
// Copyright © 2019 Woolsey Workshop.  All rights reserved.


// Defines
#define AIO_USERNAME    "dkswami"
#define AIO_KEY         "aio_hicZ88WvIeJFV2JiKKJj61ArcJJ1"
#define AIO_button_FEED "push-button"
#define WIFI_SSID       "PQRS"
#define WIFI_PASS       "nothing20"
#define USE_AIRLIFT     // required for Arduino Uno WiFi R2 board compatability


// Libraries
#include <AdafruitIO_WiFi.h>


// Pin Mapping
const byte BUTTON_PIN =  1;
bool current = false;
bool last = false;



// Constructors
AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS, SPIWIFI_ACK, SPIWIFI_RESET, NINA_GPIO0, &SPI);
AdafruitIO_Feed *digital = aio.feed(AIO_button_FEED);


void setup() {
   // Pin configuration
   pinMode(BUTTON_PIN, INPUT);

   // Serial bus initialization (Serial Monitor)
   Serial.begin(9600);
   while(!Serial);  // wait for serial connection

   // Adafruit IO connection and configuration
   Serial.print("Connecting to Adafruit IO");
   aio.connect();  // connect to Adafruit IO service
   while(aio.status() < AIO_CONNECTED) {
      Serial.print(".");
      delay(1000);  // wait 1 second between checks
   }
   Serial.println();
   Serial.println(aio.statusText());  // print AIO connection status

   // Synchronize current state
   digital->get();  // request feed value (message) from AIO
}


void loop() {
   aio.run();  // keep client connected to AIO service
  // grab the current state of the button.
  // we have to flip the logic because we are
  // using a pullup resistor.
  if(digitalRead(BUTTON_PIN) == LOW)
    current = true;
  else
    current = false;

  // return if the value hasn't changed
  if(current == last)
    return;

  // save the current state to the 'digital' feed on adafruit io
  Serial.print("sending button -> ");
  Serial.println(current);
  digital->save(current);

  // store last button state
  last = current;
}
