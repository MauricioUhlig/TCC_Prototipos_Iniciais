#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

#define pino_sinal_analogico A0
#define powerPin D5
 
#define DHTPIN D4 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
// Update these with values suitable for your network.

//informações da rede WIFI
const char* ssid = "Precioso";                 //SSID da rede WIFI
const char* password =  "naopossopassar";    //senha da rede wifi

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "192.168.1.200";   //server
const char* mqttUser = "psfhemdn";              //user
const char* mqttPassword = "oenianQ47UJe";      //password
const int mqttPort = 1883;                     //port


const int interDelay = 200;

float valor_analogico;
float percentual_umidade_solo;
bool powerOn = false;
 
DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int delayTime = 30000;
int Fl_Bomba = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void Subscribe(){
  client.subscribe("delayTime");
  client.subscribe("SubBomba");
}
void callback(char* topic, byte* payload, unsigned int length) {
 /*
  Serial.print(topic);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
 */
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    if(!strcmp(topic,"delayTime")){
      delayTime = ((int)atoi((char *)payload))*1000;
      //Serial.println(delayTime);
      lastMsg = millis();
      client.publish("Delay", String(delayTime/1000).c_str());
    }
    if(!strcmp(topic,"SubBomba")){
      if(((int)atoi((char *)payload))== 1){
        digitalWrite(LED_BUILTIN, LOW);
        Fl_Bomba = 1;
      }else{
        digitalWrite(LED_BUILTIN, HIGH);
        Fl_Bomba = 0;
      }
      client.publish("PubBomba",String(Fl_Bomba).c_str());
      //Serial.println(atoi((char *)payload));
    }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      Subscribe();
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pino_sinal_analogico, INPUT);
  pinMode(powerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  
  setup_wifi();

  sensor_t sensor;

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    //#ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
    //#endif
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      //#ifdef DEBUG
      Serial.println("Conectado");  
      //#endif
 
    } else {
      //#ifdef DEBUG 
      Serial.print("falha estado  ");
      Serial.print(client.state());
      //#endif
      delay(2000);
 
    }
  }
  
  Subscribe();
  dht.begin();
  
}

void loop() {

if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > (delayTime -2000) && !powerOn){
    digitalWrite(powerPin,HIGH);
    //Serial.println("Liga");
    powerOn = true;
  }
  if (now - lastMsg > delayTime) {
    lastMsg = now;
    
    sensors_event_t event;
    
    dht.temperature().getEvent(&event);
    String t = String(event.temperature);
    dht.humidity().getEvent(&event);
    String h = String(event.relative_humidity);
    valor_analogico = analogRead(pino_sinal_analogico);
    percentual_umidade_solo = (((1023-valor_analogico)/1023)*100);
    String h_solo = String(percentual_umidade_solo);
    // testa se retorno é valido, caso contrário algo está errado.
    //if (isnan(t) || isnan(h)) 
    if( isnan(event.temperature) || isnan(event.relative_humidity) )
    {
      Serial.println("Failed to read from DHT");
    } 
    else
    {
      //Serial.print("Publish message: ");
      //Serial.println(t);
      client.publish("Temperatura", t.c_str());
      delay(interDelay);
      //Serial.print("Publish message: ");
      //Serial.println(h);
      client.publish("Humidade", h.c_str());
      delay(interDelay);
      client.publish("Solo", h_solo.c_str());
      delay(interDelay);
    }
    digitalWrite(powerPin,LOW);
   // Serial.println("Desliga");
    powerOn = false;
    client.publish("Delay", String(delayTime/1000).c_str());
    client.publish("PubBomba",String(Fl_Bomba).c_str());
  }
   
  
}
