/*By Aruran Sivashankar
 * Read local data from DHT11 and add it to ThingSpeak
 */


#include "DHT.h"//Read measurements from DHT11-sensor (or DHT22)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define TS_ENABLE_SSL //For secure connection
#include "ThingSpeak.h" //To export our local data to ThingSpeak

//Setup WiFi for ESP8266
#define ssid "MY SSID" //Network ssid
#define password "PASSWORD FOR MY SSID"//Password of said network

//Setup reading from DHT11
#define DHTPIN D5 //Digital pin connected DHT sensor
#define DHTTYPE DHT11 //We're using the DHT11 sensor

//Setup for exporting local data to ThingSpeak
#define channelID 000000 //Your channel ID
#define writeAPI "WRITE_API FOR MY CHANNEL" //Write API for your channel
#define FINGERPRINT "27 18 92 DD A4 26 C3 07 09 B9 7A E6 C5 21 B9 5B 48 F7 16 E1" //SHA1 fingerprint for thingspeak.com, check if outdated

#define measurement_delay 30000 //Time interval between each measurement in milliseconds


//Initilize variables

float temp, humidity;//Store local data
DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client; //Create client for ThingSpeak


void setup(){
  Serial.begin(115200);

  //Connect to WiFi
  WiFi.mode(WIFI_STA);//Set WiFi-mode to station

  client.setFingerprint(FINGERPRINT);
  
  Serial.print("Connecting to (WiFi): ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(3000);
    Serial.print("Attempting to connect to ");
    Serial.print(ssid);
    Serial.println("...");
  }

  Serial.println("");
  Serial.println("");
  Serial.print("Successfully connected to ");
  Serial.println(ssid);

  //Initialize ThingSpeak
  ThingSpeak.begin(client);

  //Begin reading from DHT11
  dht.begin();
}



void loop(){
  //Fetch local data
  fetch_temp_and_humidity(temp, humidity);
  //Write to ThingSpeak
  write_to_ThingSpeak(temp, humidity);
  Serial.println("-------------------------------------------------");
  delay(measurement_delay);
}


/*HELPER FUNCTIONS
 * Get local data
 * Write to ThingSpeak
 */


//Read local data from sensors
void fetch_temp_and_humidity(float& temp, float& humidity){

  temp = dht.readTemperature();//Fetch temperature from sensor in calcius
  humidity = dht.readHumidity();//Fetch relative humidity from sensor
  
  if (isnan(temp) || isnan(humidity)){//Check if reading is invalid
    Serial.println("Invalid read from DHT11-sensor");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  return;
}


//Write local data to ThingSpeak
void write_to_ThingSpeak(const float temp, const float humidity){
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humidity);

  int val = ThingSpeak.writeFields(channelID, writeAPI);
  if (val == 200){ 
    Serial.println("Successfully uploaded data to ThingSpeak channel");
  } else{
    Serial.print("Error while uploading data to ThingSpeak channel. Error code: ");
    Serial.println(val);
  } 

  return;
}