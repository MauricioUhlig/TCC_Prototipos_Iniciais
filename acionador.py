
from influxdb import InfluxDBClient
import json

import math

def sigmoid(x):
    return (float(1 / float(1 + math.exp(-x))))

INFLUXDB_ADDRESS = '192.168.1.200'
INFLUXDB_USER = 'grafana'
INFLUXDB_PASSWORD = 'grafana_pass'
INFLUXDB_DATABASE = 'monitoring'

                            
influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)
                            
       
def _init_influxdb_database():
    databases = influxdb_client.get_list_database()
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)
    influxdb_client.switch_database(INFLUXDB_DATABASE)
                            
def main():                 
    _init_influxdb_database()

if __name__ == '__main__':  
    print('MQTT to InfluxDB bridge')
    main()
    
    results = []
    results.append(influxdb_client.query('select mean(value) as value from Temperatura where time > now()- 5m'))
    results.append(influxdb_client.query('select mean(value) as value from Humidade where time > now()- 5m'))
    results.append(influxdb_client.query('select mean(value) as value from solo where time > now()- 5m'))
    
    # print(results)
    results_pesos = []
    results_pesos.append(influxdb_client.query('select * from peso order by time desc limit 1'))
    print(results_pesos)
    pesos = []


    temperatura_peso = 0
    for result in results_pesos:
        for point in result.get_points():
            temperatura_peso = point['temperatura']
            humidade_peso = point['humidade']
            solo_peso = point['solo']

    results_points = []
    for result in results:
        for point in result.get_points():
            results_points.append(point['value'])

   
    partial = results_points[0]* float(temperatura_peso) + (100-results_points[1])*float(humidade_peso) + (100 - results_points[2])*float(solo_peso)
    print(partial)

    if(partial > 1800):
        print("liga")

    if(partial < 1200):
        print("desliga")
 
    # json_body = [       
    #         {               
    #             "measurement":"peso",
    #             "tags": {   
    #                 "host": "HIGROMETO"
    #             },          
    #             "fields": { 
    #                 "temperatura": "20",
    #                 "humidade": "35",
    #                 "solo": "70"
    #             }           
    #         }               
    #     ]                   
    # influxdb_client.write_points(json_body)
    # print(json_body)       
