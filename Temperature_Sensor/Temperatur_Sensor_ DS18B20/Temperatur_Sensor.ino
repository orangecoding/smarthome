#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include "config.h"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"
ADC_MODE(ADC_VCC);

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor(DATAPOINT_NAME);

#define ONE_WIRE_BUS 0

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>



OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
HTTPClient sender;

float temperature;

void preinit() {
  ESP8266WiFiClass::preinitWiFiOff();
}

void setup() {
  //safe power
  WiFi.mode( WIFI_OFF );
  WiFi.persistent( false );
  WiFi.forceSleepBegin();
  delay( 1 );

  //Sensors read
  Serial.begin(115200);
  sensors.begin();
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  Serial.println(temperature);

  //wifi begin
  WiFi.forceSleepWake();
  delay( 1 );
  WiFi.begin(wifiSsid, wifiKey);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  
  pushDataToInflux();
  //1 hour deep sleep
  ESP.deepSleep(3600000000);
}

void pushDataToInflux() {
  float vccVolt = ((float)ESP.getVcc())/1024;
  sensor.clearFields(); 
  
  sensor.addField("temperature",  sensors.getTempCByIndex(0));
  sensor.addField("power", vccVolt);
  
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

/**
   We don't need to run it in a loop because we're using the DeepSleep. Each time the ESP is being woken up, the setup method
   will run. (It's like pressing the reset button)
*/
void loop() {}
