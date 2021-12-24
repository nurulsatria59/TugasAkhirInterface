#define BLYNK_PRINT Serial
#include <SPI.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "TRIGGER_WIFI.h"
#include "TRIGGER_GOOGLESHEETS.h"

#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht11(DHTPIN, DHTTYPE);

//Configure Blynk
char auth[] = "5D07vWrK9AkwJ_CJJg3kQ6AksIfDBOE3";
char ssid[]="Samsung123";
char pass[]="ulululul44";

// Configure Firebase
#define FIREBASE_HOST "tugasakhirinterface-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "E5Yf7h38ZdzzAm433hg8qQe9rWW81W9qSH7GXm6p"

// Configure Google Sheet
char column_name_in_sheets[ ][20] = {"value1","value2"};
String Sheets_GAS_ID = "AKfycbyuQZOUDAcMtY78-VAaXdT_Ow7NliJ3qxO0g4G0SfwukvmA6vOi5kTRH3j2WUeNbuCENg";
int No_of_Parameters = 2;

// Wifi
#define WIFI_SSID "Samsung123"
#define WIFI_PASSWORD "ulululul44"

FirebaseData firebaseData;

// Connect ke app blynk
SimpleTimer timer;
void sendSensor(){
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
}

void setup() {
  Serial.begin(115200);

  //Blynk
  dht11.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(400);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Blynk.begin(auth, ssid, pass);
  dht11.begin();
  timer.setInterval(1000L, sendSensor);

  //Google Sheet
  while (!Serial);
  dht11.begin();
  WIFI_Connect("Samsung123","ulululul44");
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
}

void loop() {
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  //Blynk
  Blynk.run();
  timer.run();
  delay(1000);

  //Firebase
  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    delay(1000);
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println();
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println();
  
  if (Firebase.setFloat(firebaseData, "/Result/Humidity", h)){
    Serial.println("Humidity Sent");
  }else{
    Serial.println("Humidity Failed to Send");
    Serial.println("Error: "+firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Result/Temperature", t)){
    Serial.println("Temperature Sent");
  }else{
    Serial.println("Temperature Failed to Send");
    Serial.println("Error: "+firebaseData.errorReason());
  }

  //Google Sheet
  Data_to_Sheets(No_of_Parameters,  h,  t);
  Serial.println();
  delay(5000);                       
}
