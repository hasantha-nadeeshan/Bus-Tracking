#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <Arduino.h>
#include <U8x8lib.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

/*GPS Configs*/
TinyGPSPlus gps;
SoftwareSerial SerialGPS(12, 14); 
int GPS_ACCU = 8;

U8X8_SH1107_SEEED_128X128_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

#define RESET_BTN 16

String API_GW_IP = "http://192.168.1.11:5000";

String EMPTY_STRING = "";

const char* ssid = "SLT FIBER";
const char* password = "Hazi10268*";

//Your Domain name with URL path or IP address with path
String SERVER_GET_DOC_ID     = API_GW_IP+ "/getDocID";
String SERVER_SET_TRIP_START = API_GW_IP+ "/updateStartField";
String SERVER_SET_TRIP_END   = API_GW_IP+ "/updateEndField";
String SERVER_UPDATE_LOCATION  = API_GW_IP+ "/updateLocation";

String BUS_ID = "NB-2346";
String HTTP_REQ = "{\"busId\":\"" + BUS_ID + "\"}";
String docId = EMPTY_STRING;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
// Check if RESET button is pressed
bool isStarted = false;
bool gpsState = false;

String curLatitude = "0.00";
String curLongitude = "0.00";
String prevLongitude = "0.00";
String prevLatitude = "0.00";
String lonOffset = "0";
String latOffset = "0";
String currentLonString,currentLatString;

int year , month , date, hour , minute , second;
float latitude , longitude;

String date_str , time_str , lat_str , lng_str;

int pm;


void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(4,0);
  digitalWrite(5,0);
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  pinMode(RESET_BTN, INPUT);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  reconnectWifi();
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
    displayInfo("Device is Ready",0,0);
    if (digitalRead(RESET_BTN) == HIGH) {
      delay(200);  // Debounce delay  
      if (setTripStart()) {  // Call isAppStarted() and check state
          isStarted = true;
          displayInfo("Device Connected",0,2);
          displayInfo(BUS_ID,0,4);
          delay(2000);
          lastTime = millis();
      }
      
      while (isStarted) {  
        
        if (digitalRead(RESET_BTN) == HIGH) {
          isStarted = false;
          
          bool state = setTripEnd();
          if (state == true) {
            displayInfo("Trip Stopped",0,2);
            displayInfo("   Thank You!   ",0,4);
          }
          else{
            displayInfo("Error Occured",0,2);
          }
          delay(5000);  // Avoid multiple presses
          clearDisplay();
          break;
        }
        
        if ((millis() - lastTime)> timerDelay) {
          displayInfo("gps...",0,8);
          gpsState = readGPSData();
          if (gpsState == true){
            displayInfo("Communicating...",0,6);
          }      
          lastTime = millis();
        }
        // Yield control to avoid WDT reset
        yield();
      }
    delay(1000);  // Avoid multiple presses
    clearDisplay();
    }
}

bool readGPSData()
{
  while (SerialGPS.available() > 0){
    Serial.println("came in");
    if (gps.encode(SerialGPS.read()))
    {
      Serial.println("came in 2");
        Serial.println("get gps valid");
        curLatitude = String(gps.location.lat() , GPS_ACCU);
        curLongitude = String(gps.location.lng() , GPS_ACCU);
        lonOffset = String((curLongitude.toFloat() - prevLongitude.toFloat())*pow(10,GPS_ACCU));
        latOffset = String((curLatitude.toFloat() - prevLatitude.toFloat())*pow(10,GPS_ACCU));
        Serial.println(curLatitude+","+curLongitude);
        Serial.println(latOffset+","+lonOffset);
        bool updateFlag = updateLocation();

      if (gps.time.isValid()) {

         time_str = "";

         hour = gps.time.hour();

         minute = gps.time.minute();

         second = gps.time.second();

         minute = (minute + 30);

         if (minute > 59) {

           minute = minute - 60;

           hour = hour + 1;

         }

         hour = (hour + 5) ;

         if (hour > 23)

           hour = hour - 24;

        if (hour >= 12)

          pm = 1;

        else

          pm = 0;

        hour = hour % 12;

        if (hour < 10)

          time_str = '0';

        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)

          time_str += '0';

        time_str += String(minute);

        time_str += " : ";

        if (second < 10)

           time_str += '0';

         time_str += String(second);

         if (pm == 1)

           time_str += " PM ";

         else

           time_str += " AM ";

       }
       Serial.println("Date: "+date_str+", time: "+time_str);
      
    }
      delay(100); 

  }
  return false;
}

void reconnectWifi()
{
  displayInfo("Connecting...",0,0);
  displayInfo(String(ssid),0,3);
  while(WiFi.status() != WL_CONNECTED) {     
    delay(500);
  }
 clearDisplay();
}

void displayInfo(String msg, int row, int col){
  u8x8.drawString(row,col, msg.c_str());
}

void clearDisplay(){
  u8x8.clearDisplay();
}
bool updateLocation() {
  if(WiFi.status() == WL_CONNECTED) { 
    WiFiClient client;
    HTTPClient http;
      
    // Your IP address with path or Domain name with URL path 
    http.begin(client, SERVER_UPDATE_LOCATION);
    
    http.addHeader("Content-Type", "application/json");
    String httpRequestData = "{\"busId\":\"" + BUS_ID + "\",\"lonOffset\":\"" + lonOffset + "\",\"latOffset\":\"" + latOffset + "\"}";  
    int httpResponseCode = http.POST(httpRequestData);
    
    String payload = "{}"; 
    bool state = false;
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      state = true;
    }
    // Free resources
    http.end();
  
    return state;
  }
  else{
    reconnectWifi();
  }
}

bool setTripStart() {
  if(WiFi.status() == WL_CONNECTED) { 
    WiFiClient client;
    HTTPClient http;
      
    // Your IP address with path or Domain name with URL path 
    http.begin(client, SERVER_SET_TRIP_START);
    
    http.addHeader("Content-Type", "application/json");
  
    int httpResponseCode = http.POST(HTTP_REQ);
    
    String payload = "{}"; 
    bool state = false;
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      state = true;
    }
    // Free resources
    http.end();
  
    return state;
  }
  else{
    reconnectWifi();
  }
}


bool setTripEnd() {
  if(WiFi.status() == WL_CONNECTED) { 
    WiFiClient client;
    HTTPClient http;
      
    // Your IP address with path or Domain name with URL path 
    http.begin(client, SERVER_SET_TRIP_END);
    
    http.addHeader("Content-Type", "application/json");
  
    String HTTP_REQ_END = "{\"busId\":\"" + BUS_ID + "\"}";
    int httpResponseCode = http.POST(HTTP_REQ);
    
    String payload = "{}"; 
    bool state = false;
    if (httpResponseCode == 200) {
      state = true;
    }
    // Free resources
    http.end();

    return state;
  }
  else{
    reconnectWifi();
  }
}
