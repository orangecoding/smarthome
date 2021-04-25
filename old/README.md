## Homekit enabled weather station with ESP8266
<img src="https://github.com/orangecoding/smarthome/blob/main/old/img/logo.png" width="250">  

With this source, a little bit of soldering and some creativity, you'll get a Homekit enabled weather station that will

- measure and store temperature data
- tell Homekit when it is raining
- measure and store power consumption of the ESP

The temperature data as well as the power consumption is forwarded to a nodeJS server in order to provide historical data (as Homekit is only showing the most current data point). This however is optional. If you want to use the server part for historical data, check out the "backend" folder within this repository.

### Architecture
<img src="https://github.com/orangecoding/smarthome/blob/main/old/img/architecture.jpg" width="500">  

To learn more check out my [Blog post at Medium](https://orangecoding.medium.com/building-a-homekit-enabled-weather-station-using-homebridge-and-an-esp8266-87aceaa4b468).
