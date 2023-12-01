/*This code serves as the publisher, extracting temperature, humidity, and pressure 
data from the ENV Hat. The gathered information is then transmitted through MQTT to 
a Mosquitto broker, which a subscriber can connect to for retrieval on the same server. 
My partner is Harini Ala who is the subscriber.*/

#include "M5StickCPlus.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HDC1000.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

// Create instances of sensor libraries
Adafruit_HDC1000 hdc;
Adafruit_BMP280 bmp;

// Add your credentials
const char* ssid = "lit"; 
const char* password = "chickenbiryani";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient * client;


void setup() {

  // Initial delay to ensure stable startup
  delay(500);

  // Initialize M5StickC
  M5.begin();
  Wire.begin();

  // Add additional delay for stable sensor initialization
  delay(500);

  // setup wifi
  setup_wifi();

  // Initialize MQTT client
  client = new PubSubClient(espClient);
  client->setServer(mqtt_server, 1883);

  if (!hdc.begin(0x40)) {
    M5.Lcd.print("Couldn't find HDC1000");
  }

  if (!bmp.begin(0x76)) {
    M5.Lcd.print("Couldn't find BMP280");
  } 
  
}

// Function to set up WiFi connection
void setup_wifi() {
  delay(10);
  M5.Lcd.print("Connecting to ");
  M5.Lcd.println(ssid);
  M5.Lcd.println("\n");
  WiFi.begin(ssid, password);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }

  // Seed random number generator
  randomSeed(micros());

  M5.Lcd.println("WiFi connected");
  M5.Lcd.println("\n");
}

// Reconnect to MQTT broker
void reconnect() {

  // Loop until we’re reconnected
  while (!client->connected()) {
    M5.Lcd.println("\n");
    M5.Lcd.print("Attempting MQTT connection...");
    M5.Lcd.println("\n");
    String clientId = "M5StickCPlus"; 
    // Attempt to connect
    // Insert your password
    if (client->connect(clientId.c_str())) {
      M5.Lcd.println("connected");
      M5.Lcd.println("\n");

      // Once connected, publish an the data
      // client->publish("temperature", tempC.c_str());
      // client->publish("pressure", pressurePa.c_str());
      // delay(10000);
    } else {
      //handle failed case
      M5.Lcd.println("\n");
      M5.Lcd.print("failed, rc = ");
      M5.Lcd.println("\n");
      M5.Lcd.print(client->state());
      M5.Lcd.println("\n");
      M5.Lcd.println(" try again in 5 seconds");
      M5.Lcd.println("\n");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {

  if (!client->connected()) {
    reconnect();
  }

  float tempC, tempF, humidity;
  float pressurePa, pressureHg;

  //collect data from the connected sensor

  tempC = hdc.readTemperature();
  humidity = hdc.readHumidity();

  pressurePa = bmp.readPressure();
  pressureHg = pressurePa / 133.322;

  client->loop();
  // M5.Lcd.println("Temperature: ");
  // M5.Lcd.println(tempC);
  // M5.Lcd.println("\n");

  // M5.Lcd.println("pressure: ");
  // M5.Lcd.println(pressurePa);
  // M5.Lcd.println("\n");

  char tempString[10];
  char preString[10];
  dtostrf(tempC, 5, 2, tempString);
  dtostrf(pressurePa, 5, 2, preString);

  M5.Lcd.println("Temperature: ");
  M5.Lcd.println(tempString);
  M5.Lcd.println("\n");

  M5.Lcd.println("pressure: ");
  M5.Lcd.println(preString);
  M5.Lcd.println("\n");


  // Once connected, publish an the data
  client->publish("temperature", tempString);
  client->publish("pressure", preString);
  delay(10000);
  
}
