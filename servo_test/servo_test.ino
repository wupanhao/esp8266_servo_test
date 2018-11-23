/*
   MQTT RGB Light for Home-Assistant - NodeMCU (ESP8266)
   https://home-assistant.io/components/light.mqtt/
   Libraries :
    - ESP8266 core for Arduino : https://github.com/esp8266/Arduino
    - PubSubClient : https://github.com/knolleary/pubsubclient
   Sources :
    - File > Examples > ES8266WiFi > WiFiClient
    - File > Examples > PubSubClient > mqtt_auth
    - File > Examples > PubSubClient > mqtt_esp8266
    - http://forum.arduino.cc/index.php?topic=272862.0
   Schematic :
    - https://github.com/mertenats/open-home-automation/blob/master/ha_mqtt_rgb_light/Schematic.png
    - LED leg 1 - Resistor 270 Ohms - D1/GPIO5
    - LED leg 2 (longuest leg) - GND
    - LED leg 3 - Resistor 270 Ohms - D2/GPIO4
    - LED leg 4 - Resistor 270 Ohms - D3/GPIO0
   Configuration (HA) : 
    light:
      platform: mqtt
      name: 'Office RGB light'
      state_topic: 'office/rgb1/light/status'
      command_topic: 'office/rgb1/light/switch'
      brightness_state_topic: 'office/rgb1/brightness/status'
      brightness_command_topic: 'office/rgb1/brightness/set'
      rgb_state_topic: 'office/rgb1/rgb/status'
      rgb_command_topic: 'office/rgb1/rgb/set'
      brightness_scale: 100
      optimistic: false
   Samuel M. - v1.1 - 08.2016
   If you like this example, please add a star! Thank you!
   https://github.com/mertenats/open-home-automation
*/

#include <ESP8266WiFi.h>
#include <FS.h>
#include <Servo.h>
#include "server.h"
// pins used for the rgb led

// Wifi: SSID and password
const char* WIFI_SSID = "Xiaomi_HAO";
const char* WIFI_PASSWORD = "wph123789";

Servo servos[12];
uint8_t servo_pins[12] = {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11};
uint8_t count = 12;
void setAngle(uint8_t di,uint8_t vi){
  if(di< 12 && vi < 180)
    servos[di].write(vi);
}
void attachServos(){
  for (uint8_t i = 0; i < count; ++i){
    servos[i].attach(servo_pins[i]);
  }
}

void detachServos(){
  for (uint8_t i = 0; i < count; ++i){
    servos[i].detach();
  }  
}
void setup() {
  // init the serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Server initial");
  //init the server
  SPIFFS.begin();
      Serial.println("SPIFFS begin");
  attachServos();
    Serial.println("Servos attached");
  server.serveStatic("/", SPIFFS, "/index.html");
  server.on("/set-servo", [](){
    String uri = server.uri();
    Serial.println(uri);
    String di = server.arg("di");
    String vi = server.arg("vi");
    Serial.println("di="+di+" vi="+vi);
    setAngle(di.toInt(),vi.toInt());
    server.send(200, "text/plain", String("set to ")+"di="+di+" vi="+vi);
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  server.handleClient();
}
