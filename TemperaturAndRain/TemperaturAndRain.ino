#define rainDigital D7
#define ONE_WIRE_BUS 2

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define sensorPower 12

#include "config.h"

//By using this, we can read
//Unfortunately this means we cannot read values from A0 anymore (analoge rain sensor data)
ADC_MODE(ADC_VCC);

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor(DATAPOINT_NAME);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
HTTPClient sender;

float temperature;
boolean bIsRaining;

void setup() {
  
  //safe power
  WiFi.mode( WIFI_OFF );
  WiFi.persistent( false );
  WiFi.forceSleepBegin();
  delay( 1 );
  
  Serial.begin(115200);
  pinMode(rainDigital, INPUT);

  digitalWrite(sensorPower, HIGH);  
  delay(10);  
  Serial.println(sensors.getDeviceCount());
  sensors.begin();
  sensors.requestTemperatures();

  temperature = sensors.getTempCByIndex(0);
  bIsRaining = !(digitalRead(rainDigital));

  //shut off rain sensor
  digitalWrite(sensorPower, LOW);  

  Serial.println(temperature);
  Serial.println(bIsRaining);

  WiFi.forceSleepWake();
  delay( 1 );
  WiFi.begin(wifiSsid, wifiKey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Connected!");
  
  doHttpStuff();
  //deep sleep for 30 minutes
  ESP.deepSleep(1800000000);
}

void doHttpStuff() {

  //we only push anything if the Wifi Connection worked
  if (WiFi.status() == WL_CONNECTED) {
    pushRainDigitalValue();
    pushDataToInflux();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

void pushDataToInflux() {
  float vccVolt = ((float)ESP.getVcc())/1024;
  sensor.clearFields(); 
  
  sensor.addField("temperature",  temperature);
  sensor.addField("power", vccVolt);
  
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }else{
    Serial.print("Pushed to InfluxDB.");
  }
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
