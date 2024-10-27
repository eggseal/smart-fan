# Smart Fan Project

This program allows the connection and monitoring of a smart ventilation system using Ubidots and a ESP32 via MQTT protocol
 *   When the temperature goes above a threshold of 25°C the motor will begin to rotate, the on/off state and temperature are monitored by the dashboard indicator
 *   The velocity in which the motor rotates is controlled by the dashboard slider 
 *   The whole system can be brought to a halt using the pause switch on the dashboard

 The purpose of this project is to be a practical example of the connection of an IoT device with a real IoT platform

## Libraries Used

1. **Ubidots for ESP32**
   - [UbidotsEsp32Mqtt](https://github.com/ubidots/esp32-mqtt)
   - A library for connecting the ESP32 to Ubidots using MQTT.
1. **PubSubClient**
   - [PubSubClient](https://github.com/knolleary/pubsubclient)
   - A library for communicating as a MQTT client
