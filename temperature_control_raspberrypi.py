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
    if temperature > 21.5:
        requests.get(URL + "/disable")
    elif temperature < 20.5:
        requests.get(URL + "/enable")
    time.sleep(60)