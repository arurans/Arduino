/*By Aruran Sivashankar
 * Read local data from DHT11 and display it on ThingSpeak and an OLED display
 * 
 * HARDWARE:
 * ESP8266-12E
 * DHT11
 * 0.96 Inch I2C IIC Serial 128 x 64 OLED display
 */

#include "DHT.h"//Read measurements from DHT11-sensor (or DHT22)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//Display to OLED screen
#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>

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

//Setup for displaying on OLED display
#define SCREEN_ADDRESS 0x3c
#define SDA_PIN D3
#define SDC_PIN D4

#define measurement_delay 30000 //Time interval between each measurement in milliseconds


//Initilize variables
float temp, humidity;//Store local data

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client; //Create client for ThingSpeak

//To display data on OLED screen
SSD1306Wire  display(SCREEN_ADDRESS, SDA_PIN, SDC_PIN);
OLEDDisplayUi ui(&display);


void setup(){
  Serial.begin(115200);
  setupDisplay();

  //Connect to WiFi
  WiFi.mode(WIFI_STA);//Set WiFi-mode to station

  client.setFingerprint(FINGERPRINT);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    drawWiFi(&display);
  }
  
  drawWiFiSuccess(&display);

  //Initialize ThingSpeak
  ThingSpeak.begin(client);

  //Begin reading from DHT11
  dht.begin();
}



void loop(){
  //Fetch local data
  bool validData = fetch_temp_and_humidity(temp, humidity);
  //Write to ThingSpeak
  if (validData){
    displayData(&display, temp, humidity);
    write_to_ThingSpeak(temp, humidity);
  }
  Serial.println("-------------------------------------------------");
  delay(measurement_delay);
}


/*HELPER FUNCTIONS
 * Get local data
 * Write to ThingSpeak
 * Display data on OLED display
 */


//Read local data from sensors
bool fetch_temp_and_humidity(float& temp, float& humidity){

  temp = dht.readTemperature();//Fetch temperature from sensor in calcius
  humidity = dht.readHumidity();//Fetch relative humidity from sensor
  
  if (isnan(temp) || isnan(humidity)){//Check if reading is invalid
    return false;
  }
  return true;
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

//Initialize the display
void setupDisplay(){
  display.init();
  display.clear();
  display.display();

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setContrast(100);

  return;
}

//Almost redundant function, cleaner in the setup function
void drawWiFi(OLEDDisplay *display) {
    display->drawString(5, 10, "Attempting to connect to ");
    display->drawString(15, 25, ssid);
    display->drawString(40, 25, "...");
    display->display();
    delay(3000);
    display->clear();
    
    return;
}

//Display successful WiFi connection
void drawWiFiSuccess(OLEDDisplay *display){
  display->clear();
  display->drawString(5, 10, "Successfully connected to ");
  display->drawString(5, 30, ssid);
  display->display();
  delay(3000);
  return;
}

//Display the local temperature and humidity on the OLED screen
void displayData(OLEDDisplay *display, const float temp, const float humidity){
  display->clear();
  //Display temperature
  display->drawString(5, 10, "Temp: ");
  display->drawString(55, 10, String(temp));
  display->drawString(85, 10, "°C");
  //Display humidity
  display->drawString(5, 30, "Humidity: ");
  display->drawString(55, 30, String(humidity));
  display->drawString(85, 30, "%");
  display->display();
  
  return;
}