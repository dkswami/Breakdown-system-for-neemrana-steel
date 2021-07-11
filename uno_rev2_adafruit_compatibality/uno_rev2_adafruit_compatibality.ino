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
#define WIFI_SSID       "PQRS"
#define WIFI_PASS       "nothing20"
#define USE_AIRLIFT     // required for Arduino Uno WiFi R2 board compatability
#define AIO_button1_FEED "pushbuttona"
#define AIO_button2_FEED "pushbuttonb"
#define AIO_button3_FEED "pushbuttonc"

// Libraries
#include <AdafruitIO_WiFi.h>


// Pin Mapping
int button1 = 3;
int button2 = 4;
int button3 = 5;
int button4 = 6;

bool last = false;



// Constructors
AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS, SPIWIFI_ACK, SPIWIFI_RESET, NINA_GPIO0, &SPI);
AdafruitIO_Feed *pushbuttona = aio.feed(AIO_button1_FEED);
AdafruitIO_Feed *pushbuttonb = aio.feed(AIO_button2_FEED);
AdafruitIO_Feed *pushbuttonc = aio.feed(AIO_button3_FEED);

void setup() {
   // Pin configuration
   pinMode(button1, INPUT_PULLUP);
   pinMode(button2, INPUT_PULLUP);
   pinMode(button3, INPUT_PULLUP);

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
   pushbuttona->get();// request feed value (message) from AIO
   pushbuttonb->get();
   pushbuttonc->get();
}


void loop() {
   aio.run();  // keep client connected to AIO service
  // grab the current state of the button.
  // we have to flip the logic because we are
  // using a pullup resistor.
  if(digitalRead(button1) == LOW){
    char b1status[] = "Pressed";
    Serial.print("sending button1 -> ");
    Serial.println(b1status);
    pushbuttona->save(b1status);
    while(digitalRead(button1) == LOW) // Wait for switch to be released
      {
        delay(20);
      }
  }  
  else if(digitalRead(button2) == LOW){
    char b2status[] = "Pressed";
    Serial.print("sending button2 -> ");
    Serial.println(b2status);
    pushbuttonb->save(b2status);
    while(digitalRead(button2) == LOW) // Wait for switch to be released
      {
        delay(20);
      }
  }
  else if(digitalRead(button3) == LOW){
    char b3status[] = "Pressed";
    Serial.print("sending button3 -> ");
    Serial.println(b3status);
    pushbuttonc->save(b3status);
    while(digitalRead(button3) == LOW) // Wait for switch to be released
      {
        delay(20);
      }
  }
  else if(digitalRead(button4) == LOW){
    return;
  }
  /*else{
    char bstatus[] = "Not Pressed";
    Serial.print("sending button ideal status");
    Serial.print(bstatus);
    
  }*/

  /* return if the value hasn't changed
  if(current == last)
    return;*/

  // save the current state to the 'digital' feed on adafruit io


  // store last button state
  //last = current;
}
