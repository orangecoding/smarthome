## ESP code for the Homekit enabled weather station with ESP8266

This code needs to be flashed to your ESP. It will collect data from the temperature and rain sensor every 25 minutes
and send them either to Homebridge or to your server (in order to build up a history).
The ESP is going into deep sleep for the time when it is not actively reading any values to save power.

### Wiring
<img src="https://github.com/orangecoding/weatherstation/blob/main/img/wiring.jpg" width="500">  
