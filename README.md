TemperatureMonitor
Since my new Viessmann heater has been causing problems from the beginning and goes into "Firing Automaton EE" (Feuerungsautomat EE) error, I needed a solution to be able to detect when the heater had failed again. This spontaneously gave me the idea to display the values in Telegram. Gradually, this code was developed. Maybe it can help you - to some extent - with your code.

This Project uses a 2004-i2c-LCD, a ds18b20 temperature sensor, and is all based on an esp8266 (Wemos d1-Mini).


![alt text](https://github.com/petergeher/TemperatureMonitor/blob/main/images/TG_Using1.png?raw=true)
![alt text](https://github.com/petergeher/TemperatureMonitor/blob/main/images/TG_alarm1.jpg?raw=true)

![alt text](https://github.com/petergeher/TemperatureMonitor/blob/main/images/LCD_cold.jpg?raw=true)
![alt text](https://github.com/petergeher/TemperatureMonitor/blob/main/images/LCD_warm.jpg?raw=true)

![alt text](https://github.com/petergeher/TemperatureMonitor/blob/main/images/LCD_alarm1.jpg?raw=true)
![alt text](https://github.com/petergeher/TemperatureMonitor/blob/main/images/LCD_Alarm2.jpg?raw=true)
