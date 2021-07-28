// AIO_LED_Pot - AIO_LED_Pot.ino


// Defines
#define AIO_USERNAME    "dkswami"
#define AIO_KEY         "aio_hicZ88WvIeJFV2JiKKJj61ArcJJ1"
#define WIFI_SSID       "GK"
#define WIFI_PASS       "nothing20"
#define USE_AIRLIFT     // required for Arduino Uno WiFi R2 board compatability
#define AIO_button1_FEED "pushbuttona"
#define AIO_button2_FEED "pushbuttonb"
#define AIO_button3_FEED "pushbuttonc"
#define AIO_buzzer_FEED "on-off"
#define AIO_connection_FEED "controllerdevicestatus"
// Libraries

#include <AdafruitIO_WiFi.h>
#include <ArduinoJson.h>  

// Pin Mapping
int button1 = 8;
int button2 = 9;
int button3 = 10;
int button4 = 11;
int button5 = 12;
int button6 = 13;
int powerLED = 2;
int RedWifiLED = 3;
int GreenWifiLED = 4;
int InternetLED = 5;
int BuzzerLED = 6;
int status = WL_IDLE_STATUS;

String b1status = "NOT Pressed";
String b2status = "NOT Pressed";
String b3status = "NOT Pressed";

String b6status = "NOT Pressed";

// for INTERNET LED to blink
unsigned long previousMillis = 0;
const long interval = 500;
int ledState = LOW;


// Constructors
AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS, SPIWIFI_ACK, SPIWIFI_RESET, NINA_GPIO0, &SPI);
AdafruitIO_Feed *pushbuttona = aio.feed(AIO_button1_FEED);
AdafruitIO_Feed *pushbuttonb = aio.feed(AIO_button2_FEED);
AdafruitIO_Feed *pushbuttonc = aio.feed(AIO_button3_FEED);
AdafruitIO_Feed *buzzer = aio.feed(AIO_buzzer_FEED);
AdafruitIO_Feed *ControllerDeviceStatus = aio.feed(AIO_connection_FEED);

WiFiClient client;
char server[] = "io.adafruit.com";

void setup() {
   // Pin configuration
   pinMode(button1, INPUT_PULLUP);
   pinMode(button2, INPUT_PULLUP);
   pinMode(button3, INPUT_PULLUP);
   pinMode(button4, INPUT_PULLUP);
   pinMode(button5, INPUT_PULLUP);
   pinMode(button6, INPUT_PULLUP);
   pinMode(powerLED, OUTPUT);
   pinMode(RedWifiLED, OUTPUT);
   pinMode(GreenWifiLED, OUTPUT);
   pinMode(InternetLED, OUTPUT);
   pinMode(BuzzerLED, OUTPUT);

   digitalWrite(powerLED, HIGH);
   
   // Serial bus initialization (Serial Monitor)
   Serial.begin(9600);
   while(!Serial);  // wait for serial connection

   //connects to the wifi and cloud service
   connecting();
   
   //sending connection status to cloud
   String connectionStatus = "Connected";
   ControllerDeviceStatus->save(connectionStatus);
   
   // Synchronize current state
   pushbuttona->get();// request feed value (message) from AIO
   pushbuttonb->get();
   pushbuttonc->get();
   buzzer->get();
   ControllerDeviceStatus->get();
}


void loop() { 
      // the interval at which you want to blink the LED.
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        // if the LED is off turn it on and vice-versa:
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
    
        // set the LED with the ledState of the variable:
        digitalWrite(InternetLED, ledState);
      }

      
      aio.run();  // keep client connected to AIO service
      // grab the current state of the button.
      // we have to flip the logic because we are
      // using a pullup resistor.
      if(digitalRead(button1) == LOW){
        b1status = "Pressed";
        String buzzerstatus = "ON";
        Serial.print("sending button1 -> ");
        Serial.println(b1status);
        pushbuttona->save(b1status);
        buzzer->save(buzzerstatus);
        digitalWrite(BuzzerLED, HIGH);
        while(digitalRead(button1) == LOW) // Wait for switch to be released
          {
            delay(20);
          }
      }  
      else if(digitalRead(button2) == LOW){
        b2status = "Pressed";
        String buzzerstatus = "ON";
        Serial.print("sending button2 -> ");
        Serial.println(b2status);
        pushbuttonb->save(b2status);
        buzzer->save(buzzerstatus);
        digitalWrite(BuzzerLED, HIGH);
        while(digitalRead(button2) == LOW) // Wait for switch to be released
          {
            delay(20);
          }
      }
      else if(digitalRead(button3) == LOW){
        b3status = "Pressed";
        String buzzerstatus = "ON";
        Serial.print("sending button3 -> ");
        Serial.println(b3status);
        pushbuttonc->save(b3status);
        buzzer->save(buzzerstatus);
        digitalWrite(BuzzerLED, HIGH);
        while(digitalRead(button3) == LOW) // Wait for switch to be released
          {
            delay(20);
          }
      }
      else if(digitalRead(button4) == LOW){
        httpRequestPost("pushbuttond","Pressed");
        String buzzerstatus = "ON";
        buzzer->save(buzzerstatus);
        digitalWrite(BuzzerLED, HIGH);        
        while(digitalRead(button4) == LOW) // Wait for switch to be released
          {
            delay(20);
          }
      }
      else if(digitalRead(button5) == LOW){
        httpRequestPost("pushbuttone","Pressed");
        String buzzerstatus = "ON";
        buzzer->save(buzzerstatus);
        digitalWrite(BuzzerLED, HIGH);        
        while(digitalRead(button5) == LOW) // Wait for switch to be released
          {
            delay(20);
          }
      }
      else if(digitalRead(button6) == LOW){
        b1status = "NOT Pressed";
        b2status = "NOT Pressed";
        b3status = "NOT Pressed";
        //b6status = "Reset Button Pressed";
        
        
        pushbuttona->save(b1status);
        pushbuttonb->save(b2status);
        pushbuttonc->save(b3status);
        httpRequestPost("pushbuttond","NOT Pressed");
        httpRequestPost("pushbuttone","NOT Pressed");
        httpRequestPost("resetbutton","Pressed");
        String buzzerstatus = "OFF";
        buzzer->save(buzzerstatus);
        digitalWrite(BuzzerLED, LOW);
        Serial.println("All value reset and Sound Alert Stopped");
        while(digitalRead(button5) == LOW) // Wait for switch to be released
          {
            delay(20);
          }
      }
}

void httpRequestPost(String key, String value) {
  StaticJsonDocument<96> doc;

  doc["key"] = key;
  doc["value"] = value;
  
  client.stop();

  Serial.println("\nStarting connection to server...");
  if (client.connect(server, 80)) 
  {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /api/v2/" AIO_USERNAME "/feeds/" + key +"/data HTTP/1.1"); 
    client.println("Host: io.adafruit.com");  
    client.println("Connection: close");  
    client.print("Content-Length: ");  
    client.println(measureJson(doc));  
    client.println("Content-Type: application/json");  
    client.println("X-AIO-Key: " AIO_KEY); 

    // Terminate headers with a blank line
    client.println();
    serializeJsonPretty(doc, Serial);
    // Send JSON document in body
    serializeJson(doc, client);
    Serial.println("data sent!");
    
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed!");
  }
}


void connecting() {
   // Adafruit IO connection and configuration
   Serial.print("Connecting to Adafruit IO");
   aio.connect();  // connect to Adafruit IO service
   while(aio.status() < AIO_CONNECTED) {
      Serial.print(".");
      digitalWrite(RedWifiLED, HIGH);
      delay(500);  // wait 1 second between checks
      digitalWrite(RedWifiLED, LOW);
      delay(500);
   }

    digitalWrite(GreenWifiLED, HIGH);
   Serial.println(aio.statusText());  // print AIO connection status
   
}
