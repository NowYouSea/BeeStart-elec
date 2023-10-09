#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

#include "creds.h"

#define DHT11PIN 16

// Replace the next variables with your SSID/Password combination
const char *ssid = MSSID;
const char *password = PASSWORD;
const char *mqtt_server = MQTTSERVER;

const char *deviceId = DEVICEID;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

DHT dht(DHT11PIN, DHT11);

float temperature = 0;
float humidity = 0;

void setup()
{
  delay(100);
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  // status = bme.begin();
  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  /*long now = millis();
  if (now - lastMsg > 6000) {
    lastMsg = now;*/

  // Temperature in Celsius
  temperature = dht.readTemperature();
  // Uncomment the next line to set temperature in Fahrenheit
  // (and comment the previous temperature line)
  // temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit

  // Convert the value to a char array
  char tempString[8];
  dtostrf(temperature, 1, 2, tempString);
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // client.publish("/home/bedroom/lit/temperature", tempString);

  humidity = dht.readHumidity();

  // Convert the value to a char array
  char humString[8];
  dtostrf(humidity, 1, 2, humString);
  Serial.print("Humidity: ");
  Serial.println(humString);
  // client.publish("/home/bedroom/lit/humidity", humString);

  StaticJsonDocument<80> doc;

  char topic[80] = "/beehive/";
  strcat(topic, deviceId);
  strcat(topic, "/");
  Serial.println(topic);

  char output[80];
  doc["id"] = deviceId;
  doc["t"] = temperature;
  doc["h"] = humidity;
  serializeJson(doc, output);
  client.publish(topic, output);

  delay(1000);
  esp_sleep_enable_timer_wakeup(1800000000);
  esp_deep_sleep_start();
  //}
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(""))
    {
      Serial.println("connected");
      // Subscribe
      // client.subscribe("esp32/output");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop()
{
}
