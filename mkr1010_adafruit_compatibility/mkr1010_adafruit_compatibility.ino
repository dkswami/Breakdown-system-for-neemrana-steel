#include <ArduinoJson.h>  
#include <WiFiNINA.h>
#include <LiquidCrystal.h>
#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;         // your network SSID (name)
char pass[] = SECRET_PASS;         // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;
char server[] = "io.adafruit.com"; // name address for Adafruit IOT Cloud

// Initialize the client library
WiFiClient client;

int state = 2;
int relay_pin = 6;

// LCD variables
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int potentPin = A0;
int potentVal = 300;

void setup() {
  analogWrite(potentPin, potentVal/4);
  pinMode(relay_pin, OUTPUT);     // Relay  
  
  lcd.begin(16, 2);
  lcd.print("GK Industrial Solutions");
  delay(2000);
   for (int positionCounter = 0; positionCounter < 23; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(500);
  }
   
  Serial.begin(9600);
    // wait for serial port to connect. Needed for native USB port only
  
  ConectToWIFI(); 
  httpRequestPost();
}

void loop() {

  String BuzzerValue = httpRequest("on-off");
  Serial.println(BuzzerValue);
  if (BuzzerValue == "ON") 
    state = 1;   
  else if (BuzzerValue == "OFF") 
    state = 0;
  else
    state = 2; 


  if (state == 1) { 
    digitalWrite(relay_pin, HIGH); //Turn on relay
    //To check which button is pressed
    if (httpRequest("pushbuttond") == "Pressed"){
      lcd.setCursor(0,1);
      lcd.print("Maint Issue");
      Serial.println("Pushbuttond : Maint Issue");
    }
    else if (httpRequest("pushbuttonc") == "Pressed"){
      lcd.setCursor(0,1);
      lcd.print("Quality Issue");
      Serial.println("Pushbuttonc : Quality Issue");
    }
    else if (httpRequest("pushbuttonb") == "Pressed"){
      lcd.setCursor(0,1);
      lcd.print("Production Issue");
      Serial.println("Pushbuttonb : Production Issue");
    }
    else if (httpRequest("pushbuttona") == "Pressed"){
      lcd.setCursor(0,1);
      lcd.print("ToolRoom Issue");
      Serial.println("Pushbuttona : Tool Room Issue");
    }
    else if (httpRequest("pushbuttone") == "Pressed"){
      lcd.setCursor(0,1);
      lcd.print("Planning Issue");
      Serial.println("Pushbuttone : Planning Issue");
    }
    
  } else {
    digitalWrite(relay_pin, LOW); //Turn off relay
    lcd.clear();
    lcd.print("WiFi Connected!");
    lcd.setCursor(0,1);
    lcd.print("Buzzer OFF");
  }
  Serial.println(state);
}


// this method makes a HTTP connection to the server:
String httpRequest(String FEED_NAME) 
{
  // JSon 
/*
 * GET: /api/v2/{username}/feeds/{feed_key}/data/last
{
  "id": "string",
  "value": "string",
  "feed_id": 0,
  "group_id": 0,
  "expiration": "string",
  "lat": 0,
  "lon": 0,
  "ele": 0,
  "completed_at": "string",
  "created_at": "string",
  "updated_at": "string",
  "created_epoch": 0
}
 */

  // close any connection before send a new request.
  // This will free the socket on the Nina module
  client.stop();

  Serial.println("\nStarting connection to server...");
  if (client.connect(server, 80)) 
  {
      Serial.println("connected to server");
      // Make a HTTP request:
      client.println("GET /api/v2/" IO_USERNAME "/feeds/"+FEED_NAME+"/data/last HTTP/1.1"); 
      
      client.println("Host: io.adafruit.com");  
      client.println("Connection: close");
      client.println("Content-Type: application/json");  
      client.println("X-AIO-Key: " IO_KEY); 
      
      // Terminate headers with a blank line
      if (client.println() == 0) {
        Serial.println(F("Failed to send request"));
      }
      
      // Check HTTP status
      char status[32] = {0};
      client.readBytesUntil('\r', status, sizeof(status));
      if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
      }
      
      // Skip HTTP headers
      char endOfHeaders[] = "\r\n\r\n";
      if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response1"));
      }
      
      // Skip Adafruit headers
      char endOfHeaders2[] = "\r";
      if (!client.find(endOfHeaders2)) {
        Serial.println(F("Invalid response2"));
      }

      //Deserialize JSon
      StaticJsonDocument<384> doc;

      DeserializationError error = deserializeJson(doc, client);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
      }
      
      const char* value = doc["value"];
    
      Serial.print("get data!:");
      Serial.println(value);
      return value;   
    
  } else {
      // if you couldn't make a connection:
      Serial.println("connection failed");
      ConectToWIFI();
      state = 2;
  }

}

void httpRequestPost() 
{
/*
 * https://io.adafruit.com/api/docs/#operation/createGroupData
 * 
 * POST /{username}/groups/{group_key}/data
 * 
 * JSON:
 * 
{
  "location": {
    "lat": 0,
    "lon": 0,
    "ele": 0
  },
  "feeds": [
    {
      "key": "string",
      "value": "string"
    }
  ],
  "created_at": "string"
}
 */

  StaticJsonDocument<96> doc;

  doc["key"] = "actuatordevicestatus";
  doc["value"] = "Connected";
  /*// Add the "feeds" array
  JsonArray feeds = doc.createNestedArray("feeds");
  JsonObject feed1 = feeds.createNestedObject();
  feed1["key"] = "outputstationstatus";
  feed1["value"] = "connected";*/
  
  // close any connection before send a new request.
  // This will free the socket on the Nina module

  Serial.println("\nStarting connection to server...");
  if (client.connect(server, 80)) 
  {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /api/v2/" IO_USERNAME "/feeds/actuatordevicestatus/data HTTP/1.1"); 
    client.println("Host: io.adafruit.com");  
    client.println("Connection: close");  
    client.print("Content-Length: ");  
    client.println(measureJson(doc));  
    client.println("Content-Type: application/json");  
    client.println("X-AIO-Key: " IO_KEY); 

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


void ConectToWIFI()
{
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    lcd.clear();
    lcd.print("Connecting...");
    lcd.blink();
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  Serial.println("Connected to wifi");
  lcd.clear();
  lcd.noBlink();
  lcd.print("WiFi Connected!");
  printWifiStatus();
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
