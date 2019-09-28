# TCC_Prototipos_Iniciais

## Instalando Grafana via docker
´´´
$ docker run -d --name=grafana -p 3000:3000 grafana/grafana
´´´

## Instalando InfluxDB via docker
´´´
$ docker run -p 8086:8086 -v $PWD:/var/lib/influxdb influxdb
´´´
### Conectando ao influxdb
Antes de conectar, deve s alterar o nome do container docker do influxdb usando *docker renama **container_name** influxdb*
´´´
$ docker exec -it influxdb influx
´´´
Criando um banco de dados e criando o usuário para o grafana
´´´
> create database monitoring
> use monitoring
> Create user grafana with password '<password>' with all priileges
´´´

## Configurando a integraçao InfluxDB - Grafana
* Acesse o grafana pelo navegador pelo http://<ip do servidor>:3000, usuario e senha: admin.
* Configure a nova conexão de banco de dados.
* Importe o json de modelo que se encontra nesse repositório
 


