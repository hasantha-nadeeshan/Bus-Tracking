#include <TinyGPSPlus.h>

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>

/*Firebase Configs*/
#define API_KEY              "AIzaSyAc1XeDwAKqIOVa2emk-3KfiLjlCY3emAs"
#define FIREBASE_PROJECT_ID  "bus-tracking-system-6713f"
#define FIREBASE_HOST "bus-tracking-system-6713f-default-rtdb.firebaseio.com"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

/*End points*/
String PATH = "trips";

/*GPS Configs*/
TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); 

/*Wifi Details*/
const char* ssid = "SLT FIBER";
const char* password = "Hazi10268*";

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , currentLatString , currentLonString, prevLatString, prevLonString;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  Serial.println("");

  /*Print firebase client version*/
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  

}

void loop()
{
  
  delay(100);
}

void readGPSData()
{
    if (gps.encode(SerialGPS.read()))
    {
        Latitude = gps.location.lat();
        currentLatString = String(Latitude , 6);
        Longitude = gps.location.lng();
        currentLonString = String(Longitude , 6);


      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += String(date);

        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += String(month);
        DateString += " / ";

        if (year < 10)
        DateString += '0';
        DateString += String(year);
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+ 5; //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();
    
        if (hour < 10)
        TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += String(second);
      }

    }
    else{
      int a = 10;
      //Display Error
    }
}

void updateDB()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    //Error Msg in Display
    int ab = 10;
  }

}

void getDocumentID()
{
  Serial.println("Get Entire Collection");
  if (Firebase.Firestore.getDocument(&fbdo,FIREBASE_PROJECT_ID, "", PATH.c_str(), ""))
  {
    Serial.println("OK");
    Serial.println(fbdo.payload().c_str());
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, fbdo.payload().c_str());
    if (!error)
    {
      for (JsonObject document : doc["documents"].as<JsonArray>())
      {
        const char *document_name = document["name"];
        Serial.print(document_name);
      }
    }
    else
    {
      Serial.println("Errors are their");
    }
  }
}
