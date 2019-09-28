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
 


