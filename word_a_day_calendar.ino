#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <Arduino_JSON.h>
#include "time.h"
#include <M5Core2.h>

const char* ssid     = "JioFiber-p6RKa";
const char* password = "k2PEDNmyHAYmMGyg";
String openWeatherMapApiKey = "253126090068251f3ba4898c83f65742";
String city = "Guwahati";
String country = "IN";
char cdate[3], cmonth[10], cyear[5], cday[10];
char thour[3], tmin[3]; 
String word1, word_def;

const char* ntpServer = "pool.ntp.org";
long  gmtOffset_sec = 0;
int   daylightOffset_sec = 3600;

String jsonBuffer;

void setup()
{
    M5.begin();
    M5.Lcd.begin(); 
  
    M5.Lcd.fillScreen(GREENYELLOW);
  
    M5.Lcd.setTextColor(BLACK); 
    M5.Lcd.setTextSize(3); 

    M5.Lcd.setCursor(90, 15); 
    M5.Lcd.print("WELCOME");

    M5.Lcd.setTextSize(5);
  
    M5.Lcd.setCursor(10, 95); 
    M5.Lcd.print("WORD A DAY");
  
    M5.Lcd.setCursor(40, 150); 
    M5.Lcd.print("CALENDAR");
    
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

//    delay(5000);
 
}

int value = 0;

void loop()
{
    HTTPClient http;

    http.begin("http://api.thingspeak.com/apps/thinghttp/send_request?api_key=WTIC24PIXWXMYPJQ");
    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
//      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        word1 = payload;
//        Serial.println(payload);
        Serial.print("Word : "); Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();

    http.begin("http://api.thingspeak.com/apps/thinghttp/send_request?api_key=AKYQZK2FWDRJYIT2");
    httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
//      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        word_def = payload;
//        Serial.println(payload);
        Serial.print("Meaning : "); Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();

    String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city +  "&APPID=" + openWeatherMapApiKey;

    jsonBuffer = httpGETRequest(serverPath.c_str());
//      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);

    if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

    Serial.print("Temperature: ");
    Serial.println(myObject["main"]["temp"]);
    Serial.print("Pressure: ");
    Serial.println(myObject["main"]["pressure"]);
    Serial.print("Humidity: ");
    Serial.println(myObject["main"]["humidity"]);
    Serial.print("Timezone: ");
    Serial.println(myObject["timezone"]);

    gmtOffset_sec = int(myObject["timezone"]);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();


    // Print values on LCD
    M5.Lcd.fillScreen(CYAN);
  
    M5.Lcd.setTextColor(BLACK); 
    M5.Lcd.setTextSize(3.5); 

    M5.Lcd.setCursor(10, 30); 
    M5.Lcd.print(city + ", " + country);

    M5.Lcd.setCursor(10, 70); 
    M5.Lcd.print(cday);
    M5.Lcd.print(" , ");
    M5.Lcd.print(cdate);
    M5.Lcd.print(" ");
    M5.Lcd.print(cmonth);
    M5.Lcd.print(" ");
    M5.Lcd.print(cyear);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.print("Time : ");
    M5.Lcd.print(15);
    M5.Lcd.print(" : ");
    M5.Lcd.print(tmin);

    M5.Lcd.setCursor(10, 150); 
    int temp = myObject["main"]["temp"];
    M5.Lcd.print("Temp : ");
    M5.Lcd.print(temp - 273);
    M5.Lcd.print(" deg C");

    delay(3000);
    M5.Lcd.fillScreen(CYAN);

    M5.Lcd.setTextColor(RED); 
    M5.Lcd.setTextSize(5); 
    M5.Lcd.setCursor(10, 10); 
    M5.Lcd.print("\"");
    M5.Lcd.print(word1);
    M5.Lcd.print("\"");

    M5.Lcd.setTextColor(MAROON); 
    M5.Lcd.setTextSize(5); 
    M5.Lcd.setCursor(10, 65); 
//    M5.Lcd.print("\"");
    M5.Lcd.print(word_def);
//    M5.Lcd.print("\"");

    delay(20000);
    
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  strftime(cdate,3, "%d", &timeinfo);
  Serial.println(cdate);
  strftime(cmonth,4, "%B", &timeinfo);
  Serial.println(cmonth);
  strftime(cday,4, "%A", &timeinfo);
  Serial.println(cday);
  strftime(cyear,5, "%Y", &timeinfo);
  Serial.println(cyear);
  strftime(thour,3, "%H", &timeinfo);
  Serial.println(cday);
  strftime(tmin,3, "%M", &timeinfo);
  Serial.println(cyear);
  

//  Serial.println("Time variables");
//  char timeHour[3];
//  strftime(timeHour,3, "%H", &timeinfo);
//  Serial.println(timeHour);
//  char timeWeekDay[10];
//  strftime(timeWeekDay,10, "%A", &timeinfo);
//  Serial.println(timeWeekDay);
//  Serial.println();
}
