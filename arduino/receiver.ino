#include "M5StickCPlus.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HDC1000.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <HTTPClient.h>

#define SERVO_PIN 26  // Pin connected to the servo
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180


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

  ledcSetup(0, 50, 16);  // Configure LEDC to generate PWM
  ledcAttachPin(SERVO_PIN, 0);  // Attach the pin to the LEDC channel

  //myServo.attach(21);

  // Add additional delay for stable sensor initialization
  delay(500);

  //setup wifi
  setup_wifi();

  // Initialize MQTT client
  client = new PubSubClient(espClient);
  client->setServer(mqtt_server, 1883);
  client->setCallback(callback);
  delay(10000);
  
}

// Callback function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {

  float temperature, pressure;
  String motorStatus = "0FF";

  String receivedTopic = String(topic);

  if (receivedTopic.equals("temperature")) {
    float temperature = atof((char *)payload);
    M5.Lcd.println("Received temperature: ");
    M5.Lcd.println(fabs(temperature));
    if (fabs(temperature) >25){

    M5.Lcd.println("in if");

    for (int angle = SERVO_MIN_ANGLE; angle <= SERVO_MAX_ANGLE; angle++) {
    moveServo(angle);
    delay(10);  // Adjust delay for the servo speed
    }
    delay(1000);  // Wait for 1 second at max angle

    // Move servo from max to min angle
    for (int angle = SERVO_MAX_ANGLE; angle >= SERVO_MIN_ANGLE; angle--) {
      moveServo(angle);
      delay(10);  // Adjust delay for the servo speed
    }
    delay(1000);  // Wait for 1 second at min angle
    motorStatus = "ON";
    sendDataToServer(fabs(temperature),pressure,motorStatus);

  }

  if (receivedTopic.equals("pressure")) {
    float pressure = atof((char *)payload);
    M5.Lcd.println("Received pressure: ");
    M5.Lcd.println(fabs(pressure));
    if (fabs(pressure) >35){

    for (int angle = SERVO_MIN_ANGLE; angle <= SERVO_MAX_ANGLE; angle++) {
    moveServo(angle);
    delay(10);  // Adjust delay for the servo speed
    }
    delay(1000);  // Wait for 1 second at max angle

    // Move servo from max to min angle
    for (int angle = SERVO_MAX_ANGLE; angle >= SERVO_MIN_ANGLE; angle--) {
      moveServo(angle);
      delay(10);  // Adjust delay for the servo speed
    }
    delay(1000);  // Wait for 1 second at min angle
    motorStatus = "ON";
    sendDataToServer(fabs(temperature),pressure,motorStatus);

  }
  
}



}

void sendDataToServer(float temperature, float pressure, String motoreStatus ) {
  
  // Prepare JSON data for the POST request
  String jsonPayload = "{\"temperature\":" + String(temperature) + "," + 
  "\"pressure\":" + String(pressure) + "," + 
  "\"motorStatus\":\"" + motoreStatus + "\"" +  
  "}";

  M5.Lcd.println("Sending data to server!! -> ");

  M5.Lcd.println("jsonPayload : ");
  M5.Lcd.println(jsonPayload);

  // HTTP POST request
  HTTPClient http;
  http.begin("https://emerging-technology-project.onrender.com/enterData");   
  http.addHeader("Content-Type", "application/json");

  // Send the POST request with the JSON payload
  int httpCode = http.POST(jsonPayload);

  M5.Lcd.println("httpcode : ");
  M5.Lcd.println(httpCode);

  if (httpCode == 200) {
    String payload = http.getString();
    M5.Lcd.println("HTTP Response:");
    M5.Lcd.println(payload);
    M5.Lcd.println();
  } else {
    M5.Lcd.println("HTTP request failed");
    M5.Lcd.println();
  }

  http.end();
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

    M5.Lcd.print("Attempting MQTT connection...");
    M5.Lcd.println("\n");
    String clientId = "M5StickCPlus-receiver"; 
    
    if (client->connect(clientId.c_str())) {
      M5.Lcd.println("connected");
      M5.Lcd.println("\n");

      //subscribe to the publisher for receiving data 
      client->subscribe("temperature"); 
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

void moveServo(int angle) {
  uint32_t duty = map(angle, 0, 180, 1000, 2000);  // Map angle to PWM duty cycle
  ledcWrite(0, duty);  // Write the duty cycle to the servo pin
}


void loop() {

  if (!client->connected()) {
    reconnect();
  }

 

  client->loop();
 
}
