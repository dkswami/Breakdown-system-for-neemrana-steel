#include <ArduinoJson.h>  
#include <WiFiNINA.h>
#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;         // your network SSID (name)
char pass[] = SECRET_PASS;         // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;
char server[] = "io.adafruit.com"; // name address for Adafruit IOT Cloud

// Initialize the client library
WiFiClient client;

int state = 2;
int relay_pin = 5;

void setup() {
  Serial.begin(9600);
  while (!Serial);   // wait for serial port to connect. Needed for native USB port only
  pinMode(relay_pin, OUTPUT);     // Relay
  ConectToWIFI(); 
  httpRequestPost();
}

void loop() {
    httpRequest();
    if (state == 1) 
     tone(relay_pin, 1000);   //Turn on relay
    else
      noTone(relay_pin);   //Turn off relay

    Serial.println(state);
 
}

// this method makes a HTTP connection to the server:
void httpRequest() 
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
      client.println("GET /api/v2/" IO_USERNAME "/feeds/on-off/data/last HTTP/1.1"); 
      
      client.println("Host: io.adafruit.com");  
      client.println("Connection: close");
      client.println("Content-Type: application/json");  
      client.println("X-AIO-Key: " IO_KEY); 
      
      // Terminate headers with a blank line
      if (client.println() == 0) {
        Serial.println(F("Failed to send request"));
        return;
      }
        
      // Check HTTP status
      char status[32] = {0};
      client.readBytesUntil('\r', status, sizeof(status));
      if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return;
      }
      
      // Skip HTTP headers
      char endOfHeaders[] = "\r\n\r\n";
      if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response1"));
        return;
      }

      // Skip Adafruit headers
      char endOfHeaders2[] = "\r";
      if (!client.find(endOfHeaders2)) {
        Serial.println(F("Invalid response2"));
        return;
      }

      //Deserialize JSon
      const size_t capacity = JSON_OBJECT_SIZE(12) + 170;
      StaticJsonDocument<capacity> doc;

      DeserializationError error = deserializeJson(doc, client);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }
      
      const char* value = doc["value"];
      if (strcmp(value, "ON") == 0) 
          state = 1;   
       else if (strcmp(value, "OFF") == 0) 
          state = 0;
       else
          state = 2;   
      
      Serial.print("get data!:");
      Serial.println(value);

       
    
  } else {
      // if you couldn't make a connection:
      Serial.println("connection failed");
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

  doc["key"] = "outputstationstatus";
  doc["value"] = "Connected";
  /*// Add the "feeds" array
  JsonArray feeds = doc.createNestedArray("feeds");
  JsonObject feed1 = feeds.createNestedObject();
  feed1["key"] = "outputstationstatus";
  feed1["value"] = "connected";*/
  
  // close any connection before send a new request.
  // This will free the socket on the Nina module
  client.stop();

  Serial.println("\nStarting connection to server...");
  if (client.connect(server, 80)) 
  {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /api/v2/" IO_USERNAME "/feeds/outputstationstatus/data HTTP/1.1"); 
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
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  Serial.println("Connected to wifi");
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
