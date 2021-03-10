#define rainDigital D7
#define ONE_WIRE_BUS 2

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "config.h"

//By using this, we can read
//Unfortunately this means we cannot read values from A0 anymore (analoge rain sensor data)
ADC_MODE(ADC_VCC);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
HTTPClient sender;

float temperature;
float power;
boolean bIsRaining;

void setup() {
  Serial.begin(115200);
  pinMode(rainDigital, INPUT);

  WiFi.begin(wifiSsid, wifiKey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println("Connected!");
  sensors.begin();
  sensors.requestTemperatures();

  temperature = sensors.getTempCByIndex(0);
  power = (float)ESP.getVcc() / (float)1000;
  bIsRaining = !(digitalRead(rainDigital));

  doHttpStuff();
  //deep sleep for 25 minutes
  ESP.deepSleep(1500 * 1000000);
}

void doHttpStuff() {

  //we only push anything if the Wifi Connection worked
  if (WiFi.status() == WL_CONNECTED) {
    pushRainDigitalValue();
    pushTemperature();
    pushPower();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

/**
   Pushing the Temperature Value to the server
*/
void pushTemperature() {
  HTTPClient http;
  http.begin(temperatureUrl + String(temperature));
  int httpCode = http.POST("");
  Serial.println(httpCode);
  Serial.println("Send Temperature" + String(temperature));
  http.end();
}

/**
   Pushing the Power Value to the server
*/
void pushPower() {
  HTTPClient http;
  http.begin(powerUrl + String(power, 4));
  int httpCode = http.POST("");
  Serial.println(httpCode);
  Serial.println("Send Power" + String(power));
  http.end();
}

/**
   Instead of pushing this value to the server, we're sending it directly to Homebridge, as it doesn't really make sense to store historical rain values
*/
void pushRainDigitalValue() {
  HTTPClient http;
  String state = "true";
  if (!bIsRaining) {
    state = "false";
  }

  http.begin(homebridgeRainStateUrl + state);
  int httpCode = http.GET();
  Serial.println(httpCode);
  Serial.println("Send Rain Value" + String(bIsRaining));
  http.end();
}

/**
   We don't need to run it in a loop because we're using the DeepSleep. Each time the ESP is being woken up, the setup method
   will run. (It's like pressing the reset button)
*/
void loop() {}
