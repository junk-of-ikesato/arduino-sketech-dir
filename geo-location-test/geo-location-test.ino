/*
ESP8266_GeoLocate_2

GeoLocation using local Wireless Access Points

m0xpd

August 2016

For further information:
http://m0xpd.blogspot.com/2016/08/esp8266-geolocation.html
=======================================================
*/

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include "secret.h"


//Credentials for Google GeoLocation API...
const char* Host = "www.googleapis.com";
String thisPage = "/geolocation/v1/geolocate?key=";
String key = "AIzaSyD9uR9bGKWEaYQ4T-nq7tRurWChgAc2fqQ";

//Credentials for Mozilla GeoLocation API...
//const char* Host = "location.services.mozilla.com";
//String thisPage = "/v1/geolocate?key=";
//String key = <API_Key>;

int status = WL_IDLE_STATUS;
String jsonString = "{\n";

double latitude    = 0.0;
double longitude   = 0.0;

int more_text = 1; // set to 1 for more debug output

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
}

void loop() {
  char bssid[6];
  DynamicJsonBuffer jsonBuffer;
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");

  if(0){
    // Print out the formatted json...
    Serial.println("{");
    Serial.println("\"homeMobileCountryCode\": 234,");  // this is a real UK MCC
    Serial.println("\"homeMobileNetworkCode\": 27,");   // and a real UK MNC
    Serial.println("\"radioType\": \"gsm\",");          // for gsm
    Serial.println("\"carrier\": \"Vodafone\",");       // associated with Vodafone
    //Serial.println("\"cellTowers\": [");                // I'm not reporting any cell towers
    //Serial.println("],");
    Serial.println("\"wifiAccessPoints\": [");
    for (int i = 0; i < n; ++i)
    {
      Serial.println("{");
      Serial.print("\"macAddress\" : \"");
      Serial.print(WiFi.BSSIDstr(i));
      Serial.println("\",");
      Serial.print("\"signalStrength\": ");
      Serial.println(WiFi.RSSI(i));
      if(i<n-1)
      {
      Serial.println("},");
      }
      else
      {
      Serial.println("}");
      }
    }
    Serial.println("]");
    Serial.println("}");
  }
    Serial.println(" ");
  }
// now build the jsonString...
jsonString="{\n";
jsonString +="\"homeMobileCountryCode\": 234,\n";  // this is a real UK MCC
jsonString +="\"homeMobileNetworkCode\": 27,\n";   // and a real UK MNC
jsonString +="\"radioType\": \"gsm\",\n";          // for gsm
jsonString +="\"carrier\": \"Vodafone\",\n";       // associated with Vodafone
jsonString +="\"wifiAccessPoints\": [\n";
    for (int j = 0; j < n; ++j)
    {
      jsonString +="{\n";
      jsonString +="\"macAddress\" : \"";
      jsonString +=(WiFi.BSSIDstr(j));
      jsonString +="\",\n";
      jsonString +="\"signalStrength\": ";
      jsonString +=WiFi.RSSI(j);
      jsonString +="\n";
      if(j<n-1)
      {
      jsonString +="},\n";
      }
      else
      {
      jsonString +="}\n";
      }
    }
    jsonString +=("]\n");
    jsonString +=("}\n");
//--------------------------------------------------------------------

 Serial.println("");
 if (more_text) {
  Serial.println(jsonString);
 }


 WiFiClientSecure client;
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(myssid);
  WiFi.begin(myssid, mypass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println(".");

  //Connect to the client and make the api call
  Serial.print("Requesting URL: ");
  Serial.println("https://" + (String)Host + thisPage + "<API_Key>");
  Serial.println(" ");
  if (client.connect(Host, 443)) {
    Serial.println("Connected");
    String request = "";
    request += "POST " + thisPage + key + " HTTP/1.1\r\n";
    request += "Host: "+ (String)Host + "\r\n";
    request += "User-Agent: ESP8266\r\n";
    request += "Content-Type: application/json\r\n";
    //request += "Accept: */*\r\n";
    request += "Content-Length: " + String(jsonString.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += jsonString;
    client.print(request);
    //delay(500);
  }
  Serial.println("aaa 0");

//  // Wait for response
//  int timer = millis();
//  while (millis() - timer < 5000) {
//    if (client.available())
//      break;
//  }
//
//  Serial.println("aaa 1");
//  String response = "";
//  String body = "";
//  while (client.available()) {
//    Serial.print(".");
//    response += client.readString();
//  }
//  Serial.println("closing connection");
//  Serial.println();
//  client.stop();
//
//  Serial.println("aaa 2");
//  Serial.println(response);

  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  while (client.connected()) {
    Serial.println("aaa 0.01");
    String line = client.readStringUntil('\n');
    Serial.println(String("aaa 0.02:len=") + line.length() + " [" + line + "]");
     if (line == "\r") {
       Serial.println("headers received");
       break;
     }
  }

  Serial.println("aaa 0.1");

  //Read and parse all the lines of the reply from server
  Serial.println("aaa 0.2");
  while (client.available()) {
  Serial.println("aaa 1");
    String line = client.readStringUntil('\r');
    if(more_text){
      Serial.println("aaa 2:" + line);
    }
    JsonObject& root = jsonBuffer.parseObject(line);
    if(root.success()){
    latitude    = root["location"]["lat"];
    longitude   = root["location"]["lng"];
    }
  }

  Serial.println("closing connection");
  Serial.println();
  client.stop();

  Serial.print("Latitude = ");
  Serial.println(latitude,6);
  Serial.print("Longitude = ");
  Serial.println(longitude,6);

  // Wait for ever...
  while(1){
    delay(1000);
  }

}


