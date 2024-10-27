/*
 * Name: SmartFan
 * Author: eggseal
 * Version: 1.0
 * Description:
 *   This program allows the connection and monitoring of a smart ventilation system using Ubidots and a ESP32 via MQTT protocol
 *   When the temperature goes above a threshold of 25°C the motor will begin to rotate, the on/off state and temperature are monitored by the dashboard indicator
 *   The velocity in which the motor rotates is controlled by the dashboard slider 
 *   The whole system can be brought to a halt using the pause switch on the dashboard
 */

// Libraries
#include "UbidotsEsp32Mqtt.h"

// Pin I/O
#define MOTOR 12   // Controls the state of the motor
#define ON_LED 13  // Indicates the on/off state of the system
#define LM35 33    // Reads the ambient temperature

// Constants
#define UBIDOTS_TOKEN ""   // The API token of your Ubidots account
#define WIFI_SSID ""       // The SSID of your Wi-Fi network
#define WIFI_PASS ""       // The password of your Wi-Fi network
#define PUB_INTERVAL 1000  // The interval (ms) for publishing messages
#define MAX_TEMP 25.0      // The temperature threshold to turn on the motor

#define DEVICE "smart-fan"  // The API label of your Ubidots device
#define PAUSE_VAR "pause"   // The API label of the pause switch
#define MOTOR_VAR "motor"   // The API label of the motor indicator
#define SPEED_VAR "speed"   // The API label of the speed slider
#define TEMP_VAR "temp"     // The API label of the temperature thermometer

// Variables
bool paused = false;
bool motorOn = false;
byte motorSpeed = 255;
float temp = 0;

unsigned long pubTimer = 0;

Ubidots ubidots(UBIDOTS_TOKEN);

void reconnect() {
  ubidots.reconnect();
  ubidots.subscribeLastValue(DEVICE, PAUSE_VAR);
  ubidots.subscribeLastValue(DEVICE, SPEED_VAR);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  String payloadStr = String((char*)payload, length);
  Serial.print("Message Arrived [");
  Serial.print(topicStr);
  Serial.print("] ");
  Serial.println(payloadStr);

  if (topicStr.indexOf(DEVICE) != -1) {
    if (topicStr.indexOf(PAUSE_VAR) != -1) {
      // Update the paused state to match the dashboard switch (0-1 -> 0-1)
      paused = payloadStr.toInt();
    } else if (topicStr.indexOf(SPEED_VAR) != -1) {
      // Update the motor speed to match the dashboard slider (0-100 -> 0-255)
      motorSpeed = payloadStr.toInt() * 255 / 100;
    }
  }
}

void setup() {
  pinMode(ON_LED, OUTPUT);
  pinMode(MOTOR, OUTPUT);

  Serial.begin(115200);

  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  reconnect();

  pubTimer = millis();
}

void loop() {
  // Reconnect to ubidots in case of connection loss
  if (!ubidots.connected())
    reconnect();

  // Update ubidots and exit early if the system is paused
  ubidots.loop();
  if (paused) return;

  // Read and scale temperature
  temp = analogRead(LM35) * (5.0 / 4095.0) * 100;
  // Turn on the motor at the set speed if the temperature surpassed the threshold
  motorOn = temp > MAX_TEMP;
  if (motorOn)
    analogWrite(MOTOR, motorSpeed);
  else
    analogWrite(MOTOR, 0);

  // Upload the variables to Ubidots
  if (millis() - pubTimer > PUB_INTERVAL) {
    ubidots.add(MOTOR_VAR, motorOn);
    ubidots.add(TEMP_VAR, temp);
    ubidots.publish(DEVICE);

    pubTimer = millis();
  }
}
