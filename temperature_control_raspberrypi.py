import os
import sys
import Adafruit_DHT
import time
import requests

DHT_SENSOR = Adafruit_DHT.DHT22
DHT_PIN = 4

URL = "http://esp8266.local/command"

while True:
    humidity, temperature = Adafruit_DHT.read(DHT_SENSOR, DHT_PIN)
    if humidity is not None and temperature is not None:
        humidity = round(humidity, 1)
        temperature = round(temperature, 1)
        if temperature > 21.5:
            requests.get(URL + "/disable")
        elif temperature < 20.5:
            requests.get(URL + "/enable")
    else:
        print('Failed to get reading. Try again!')
    time.sleep(60)
