

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "Enter you wifi ssd"
#define WLAN_PASS       "Enter your password"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "adafruit username"
#define AIO_KEY         "key"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light");
Adafruit_MQTT_Subscribe light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light2");
Adafruit_MQTT_Subscribe fan   = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/fan");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for light and fan feed.
  mqtt.subscribe(&light);
  mqtt.subscribe(&light2);
  mqtt.subscribe(&fan);
}

uint32_t x=0;

void loop() {
  MQTT_connect();
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &light) {
      Serial.print(F("Got_light: "));
      Serial.println((char *)light.lastread);
      uint16_t num = atoi((char *)light.lastread);
      digitalWrite(D0,num);
    }
 
       if (subscription == &fan){
      uint16_t num1 = atoi((char *)fan.lastread);
      analogWrite(D1, num1);
    }
         if (subscription == &light2) {
      Serial.print(F("Got_light: "));
      Serial.println((char *)light2.lastread);
      uint16_t num2 = atoi((char *)light2.lastread);
      digitalWrite(D2,num2);
    }
    
  }
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
