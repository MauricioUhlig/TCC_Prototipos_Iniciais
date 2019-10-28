# Colocando pra funcionar

## Instalando Grafana via docker
```
$ docker run -d --name=grafana -p 3000:3000 grafana/grafana
```

## Instalando InfluxDB via docker
```
$ docker run -d --name=influxdb -p 8086:8086 -v $PWD:/var/lib/influxdb influxdb
```
### Conectando ao influxdb

```
$ docker exec -it influxdb influx
```
Criando um banco de dados e criando o usuário para o grafana
```
> create database monitoring
> use monitoring
> Create user grafana with password '<password>' with all privileges
```

## Configurando a integraçao InfluxDB - Grafana
* Acesse o grafana pelo navegador pelo http://[ip do servidor]:3000, usuario e senha: admin.
* Configure a nova conexão de banco de dados.
* Importe o json de modelo que se encontra nesse repositório
 
 ## Proparando o broker mqtt
 ### Criado pastas para uso do mosquitto
 ```
$ sudo mkdir -p /srv/mqtt/log
$ sudo mkdir -p /srv/mqtt/data/
$ sudo mkdir -p /srv/mqtt/data/
 ```
 ### Instalando Mosquitto via docker
  ```
 $ docker run -d --name=mosquitto --restart on-failure -p 1883:1883 -p 9001:9001 -v /srv/mqtt/log:/mqtt/log -v /srv/mqtt/log:/mqtt/log -v /srv/mqtt/data/:/mqtt/data/ eclipse-mosquitto
   ```
 
## Iniciando os containers automaticamente 
 ```
 $ docker update --restart=always grafana influxdb mosquitto
  ```
  
## Sincronizador
### Preparando Python
Criando o Dockerfile
```
$ mkdir docker_sync && cd docker_sync && touch Dockerfile && nano Dockerfile
```
Preencha o arquivo com o seguinte:
```
FROM python:2-alpine
RUN pip install paho-mqtt influxdb
CMD python /home/sync.py
EXPOSE 1883
EXPOSE 8086
```
Execute para gerar a imagem: 
```
$ docker build -t python-sync .
```
Colocando a imagem para rodar 
```
$ docker run -d -v /home/ubuntu/scripts:/home --name=sync --link influxdb:influxdb --link mosquitto:mosquitto --restart=always python-sync
```

## Acionador
### Preparando Python
Criando o Dockerfile
```
$ mkdir docker_start && cd docker_start && touch Dockerfile && nano Dockerfile
```
Preencha o arquivo com o seguinte:
```
FROM python-sync:latest
CMD python /home/start.py
EXPOSE 1883
EXPOSE 8086
```
Execute para gerar a imagem: 
```
$ docker build -t python-start .
```
Colocando a imagem para rodar 
```
$ docker run -d -v /home/ubuntu/scripts:/home --name=start --link influxdb:influxdb --link mosquitto:mosquitto --restart=always python-start
```


