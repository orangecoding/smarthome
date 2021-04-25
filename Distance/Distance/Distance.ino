#define echoPin D6 // Echo Pin
#define trigPin D7 // Trigger Pin

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "config.h"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

ADC_MODE(ADC_VCC);

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor(DATAPOINT_NAME);

long distance;
void setup(){
  //safe power
  WiFi.mode( WIFI_OFF );
  WiFi.persistent( false );
  WiFi.forceSleepBegin();
  delay( 1 );
  
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay( 10 );
  
  distance = getDistance();

  WiFi.forceSleepWake();
  delay( 1 );
  WiFi.begin(wifiSsid, wifiKey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Connected!");

  Serial.println(distance);
  
  pushData();
  //deep sleep for 60 minutes
  ESP.deepSleep(3600000000);
 }

 void pushData() {
  float vccVolt = ((float)ESP.getVcc())/1024;
  
  sensor.addField("wasserstand",  distance);
  sensor.addField("power", vccVolt);
  
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }else{
    Serial.print("Pushed to InfluxDB.");
  }
}

long getDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //Calculate the distance (in cm) based on the speed of sound.
  return pulseIn(echoPin, HIGH)/58.2;
}

void loop() {}
