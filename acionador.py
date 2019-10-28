from influxdb import InfluxDBClient
import json
import time
import paho.mqtt.publish as publish

INFLUXDB_ADDRESS = '192.168.1.200'
INFLUXDB_USER = 'grafana'
INFLUXDB_PASSWORD = 'grafana_pass'
INFLUXDB_DATABASE = 'monitoring'
MQTT_ADDRESS = '192.168.1.200'

                            
influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)

                          
def comando_loop():
    sleepTime = 60 
    def comando():  
        nonlocal sleepTime
        results = []
        results.append(influxdb_client.query('select mean(value) as value from Temperatura where time > now()- 5m'))
        results.append(influxdb_client.query('select mean(value) as value from Humidade where time > now()- 5m'))
        results.append(influxdb_client.query('select mean(value) as value from solo where time > now()- 5m'))

        results_pesos = []
        results_pesos.append(influxdb_client.query('select * from peso order by time desc limit 1'))
        pesos = []

        results_condicionais = []
        results_condicionais.append(influxdb_client.query('select * from condicionais order by time desc limit 1'))

        for result in results_pesos:
            for point in result.get_points():
                temperatura_peso = point['temperatura']
                humidade_peso = point['humidade']
                solo_peso = point['solo']
        
        for result in results_condicionais:
            for point in result.get_points():
                liga = point['liga']
                desliga = point['desliga']

        results_points = []
        for result in results:
            for point in result.get_points():
                results_points.append(point['value'])

        partial = results_points[0]* float(temperatura_peso) + (100-results_points[1])*float(humidade_peso) + (100 - results_points[2])*float(solo_peso)
        
        if(partial > float(liga)):
            print("liga")
            msgs = [{'topic':"SubBomba", 'payload':"1"},
                    ("delayTime", "5", 0, False)]
            publish.multiple(msgs, hostname= MQTT_ADDRESS)
            sleepTime = 5

        if(partial < float(desliga)):
            print("desliga")
            msgs = [{'topic':"SubBomba", 'payload':"0"},
                    ("delayTime", "60", 0, False)]
            publish.multiple(msgs, hostname= MQTT_ADDRESS)
            sleepTime = 60
    
    while True:
        comando()
        time.sleep(sleepTime)

def _init_influxdb_database():
    databases = influxdb_client.get_list_database()
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)
    influxdb_client.switch_database(INFLUXDB_DATABASE)
                            
def main():                 
    _init_influxdb_database()

if __name__ == '__main__':    
    main()
    comando_loop()
        


