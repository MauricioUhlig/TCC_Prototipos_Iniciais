#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define powerPin D6
#define LED D4

// Update these with values suitable for your network.

//informações da rede WIFI
const char* ssid = "Precioso";                 //SSID da rede WIFI
const char* password =  "naopossopassar";    //senha da rede wifi

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "192.168.1.200";   //server
//const char* mqttUser = "psfhemdn";              //user
//const char* mqttPassword = "oenianQ47UJe";      //password
const int mqttPort = 1883;                     //port


const int interDelay = 200;

bool powerOn = false;


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int Fl_Bomba = 0;
int delayTime = 30000;

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

    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    if(!strcmp(topic,"delayTime")){
      delayTime = ((int)atoi((char *)payload))*1000;
      //Serial.println(delayTime);
      lastMsg = millis();
    }
  
    if(!strcmp(topic,"SubBomba")){
      if(((int)atoi((char *)payload))== 1){
        digitalWrite(LED, LOW);
        Fl_Bomba = 1;
        digitalWrite(powerPin, HIGH);
        // quando a bomba estiver ativada, a leitura dos sensores deve acontecer com maior frequencia
        client.publish("delayTime",String(5).c_str());
      }else{
        digitalWrite(LED, HIGH);
        Fl_Bomba = 0;
        digitalWrite(powerPin, LOW);
        // quando a bomba estiver desativada, a leitura dos sensores volta a acontecer com menor frequencia
        client.publish("delayTime",String(delayTime/1000).c_str());
      }
      client.publish("PubBomba",String(Fl_Bomba).c_str());
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
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
  
  setup_wifi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    //#ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
    //#endif
 
    if (client.connect("ESP8266Client")) {
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
}

void loop() {

if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > delayTime) {
    lastMsg = now;  
    client.publish("PubBomba",String(Fl_Bomba).c_str());
  }
   
  
}
