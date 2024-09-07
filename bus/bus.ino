#include <ESP8266Firebase.h>

#include <ESP8266WiFi.h>
//#include <FirebaseESP8266.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "SLT FIBER";
const char* password = "Hazi10268*";

// Firebase configuration
const char* apiKey = "AIzaSyAsNdU6PVRhNXFTMBqxoyWrMJ4iT2seC00";
const char* authDomain = "bus-tracking-system-6713f.firebaseapp.com";
const char* databaseURL = "https://bus-tracking-system-6713f-default-rtdb.firebaseio.com";
const char* projectId = "bus-tracking-system-6713f";
const char* storageBucket = "bus-tracking-system-6713f.appspot.com";
const char* messagingSenderId = "995044715139";
const char* appId = "1:995044715139:web:8b1d1147174faf52f109b6";
const char* measurementId = "G-31KE3Z4DCX";

// Firebase Data object
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  // Initialize Firebase
  Firebase.begin(databaseURL, apiKey);
  Firebase.reconnectWiFi(true);

  // Retrieve document names from "trips" collection
  if (Firebase.getJSON(firebaseData, "/trips")) {
    if (firebaseData.dataType() == "json") {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, firebaseData.jsonData());
      JsonObject trips = doc.as<JsonObject>();
      for (JsonPair kv : trips) {
        Serial.println(kv.key().c_str());
      }
    }
  } else {
    Serial.println("Failed to retrieve data");
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
  // Nothing to do here
}
