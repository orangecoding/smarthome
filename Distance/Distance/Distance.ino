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

int wasserMin = 20;
int wasserLow = 17;
int wasserMax = 6;
//this is the distance between the top and the max water level
int correctionValue = 8;
long distance = 0;

void setup(){
  //safe power
  WiFi.mode( WIFI_OFF );
  WiFi.persistent( false );
  WiFi.forceSleepBegin();
  delay( 1 );
  
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay( 50 );
  
  distance = getDistance();
  delay( 10 );
  WiFi.forceSleepWake();
  delay( 1 );
  WiFi.begin(wifiSsid, wifiKey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.println(distance);
  Serial.println(getPercentage());
  
  pushDataToInflux();
  pushDataToHomebridge();
  //deep sleep for 60 minutes
  ESP.deepSleep(3600000000);
 }

 void pushDataToInflux() {
  float vccVolt = ((float)ESP.getVcc())/1024;
  
  sensor.addField("wasserstand",  getPercentage());
  sensor.addField("power", vccVolt);
  
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }else{
    Serial.print("Pushed data to InfluxDB.");
  }
}

void pushDataToHomebridge() {
  HTTPClient http;
  String state = getWarning();

  //wasserstand min alarm
  http.begin(homebridgeWasserstandStateUrl + state);
  int httpCode = http.GET();
  Serial.println(httpCode);
  Serial.println("Send Wasserstand state Value" + String(state));
  http.end();

  //wasserstand state
  float percentage = getPercentage();
  http.begin(homebridgeWasserstandUrl + String(percentage));
  int httpCode2 = http.GET();
  Serial.println(httpCode2);
  Serial.println("Send Wasserstand Value" + String(percentage));
  http.end();
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

float getPercentage(){
  float percentage = (float)distance / (float)wasserMin * (float)100;
  float waterLeftInPercentage =  (float)100 - percentage;
  if(waterLeftInPercentage < 0){
    return 0;  
  }

  return waterLeftInPercentage; 
}

String getWarning(){
  if(distance >= wasserMin){
    return "true"; 
  }else{
    return "false";
  }
}

void loop() {}
