import os
import sys
import Adafruit_DHT
import time
import requests
import logging

DHT_SENSOR = Adafruit_DHT.DHT22
DHT_PIN = 4
URL = "http://esp8266.local/command"
TEMPERATURE_THRESHOLD = 21

# Logger setup
logger = logging.getLogger()
logger.setLevel(logging.INFO)

# create a file handler
handler = logging.FileHandler('/tmp/temp_log.log')
handler.setLevel(logging.INFO)

# create a logging format
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
handler.setFormatter(formatter)

# add the handlers to the logger
logger.addHandler(handler)


def get_dht22_reading(sensor, pin):
    humidity, temperature = None, None
    while humidity is None or temperature is None:
        try:
            humidity, temperature = Adafruit_DHT.read_retry(sensor, pin)
        except Exception as e:
            logger.error(f'Error reading sensor: {e}')
            return humidity, temperature
    humidity = round(humidity, 1)
    temperature = round(temperature, 1)
    return humidity, temperature


def stabilize_temperature(temperature, threshold, time_window):
    temperature_history = []
    temperature_history.append(temperature)
    while len(temperature_history) < time_window:
        try:
            humidity, temperature = get_dht22_reading(DHT_SENSOR, DHT_PIN)
            temperature_history.append(temperature)
        except Exception as e:
            logger.error(f'Error reading sensor: {e}')
            return

    # check if last n temperature readings are below threshold
    if all(temp < threshold for temp in temperature_history):
        try:
            response = requests.get(URL + "/enable")
            if response.status_code == 200:
                logger.info(f'Temperature is below threshold for {time_window} seconds. Device turned on.')
            else:
                logger.error(f'Error sending request: {response.status_code}')
        except Exception as e:
            logger.error(f'Error sending request: {e}')

    temperature_history.clear()

while True:
    try:
        humidity, temperature = get_dht22_reading(DHT_SENSOR, DHT_PIN)
        logger.info(f'Temperature: {temperature}, Humidity: {humidity}')
        if temperature < TEMPERATURE_THRESHOLD:
            stabilize_temperature(temperature, TEMPERATURE_THRESHOLD, 10)
    except Exception as e:
        logger.error(f'Error getting reading: {e}')
    time.sleep(240)
