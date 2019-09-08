#!/usr/bin/env python3

"""A MQTT to InfluxDB Bridge
This script receives MQTT data and saves those to InfluxDB.
"""

import re
from typing import NamedTuple

import time
from datetime import datetime

import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient

INFLUXDB_ADDRESS = 'localhost'
INFLUXDB_USER = 'grafana_user'
INFLUXDB_PASSWORD = 'grafana_pass'
INFLUXDB_DATABASE = 'clima'

MQTT_ADDRESS = 'soldier.cloudmqtt.com'
MQTT_USER = 'psfhemdn'
MQTT_PASSWORD = 'oenianQ47UJe'
MQTT_TOPIC = '#'  # [bme280|mijia]/[temperature|humidity|battery|status]
MQTT_REGEX = '/'
MQTT_CLIENT_ID = 'MQTTInfluxDBBridge'

influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)


def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    #print(msg.topic + ' ' + str(msg.payload))
    _send_sensor_data_to_influxdb(msg.topic, msg.payload.decode('utf-8'))
    #sensor_data = _parse_mqtt_message(msg.topic, msg.payload.decode('utf-8'))
   
    if sensor_data is not None:
        _send_sensor_data_to_influxdb(sensor_data)
        print("send to influx")



def _send_sensor_data_to_influxdb(topic, value):
    json_body = [
        {
            "measurement": "clima",
            "tags": {
                "host": "DHT11",
                "region": "SMJ-ES"
            },
            "time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "fields": {
                "datetime": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                "topic": topic,
                "value": float(value)
            }
        }
    ]
    influxdb_client.write_points(json_body)


def _init_influxdb_database():
    databases = influxdb_client.get_list_database()
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)
    influxdb_client.switch_database(INFLUXDB_DATABASE)


def main():
    _init_influxdb_database()

    mqtt_client = mqtt.Client(MQTT_CLIENT_ID)
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 16684)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()
