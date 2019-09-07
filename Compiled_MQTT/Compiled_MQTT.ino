
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>


#define DHTPIN D5 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
// Update these with values suitable for your network.

//informações da rede WIFI
const char* ssid = "Precioso";                 //SSID da rede WIFI
const char* password =  "naopossopassar";    //senha da rede wifi
String text =  "naopossopassar";
//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "soldier.cloudmqtt.com";   //server
const char* mqttUser = "psfhemdn";              //user
const char* mqttPassword = "oenianQ47UJe";      //password
const int mqttPort = 16684;                     //port
const char* mqttTopicSub ="ledplaca/L13";            //tópico que sera assinado

DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
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
  //pinMode(D4, WAKEUP_PULLUP);     // Initialize the BUILTIN_LED pin as an output
  //digitalWrite(D0,LOW);
  Serial.begin(115200);
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
   //client.publish("leituraPotenciometro",const_cast<char*>(text.c_str()));
    text = text +"1";
  //subscreve no tópico
  client.subscribe(mqttTopicSub);
  //deepSleep();
 
//  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  dht.begin();
   // ESP.deepSleep(3e6);
    //delay(250);
    //ESP.deepSleep(3e6);
}

void loop() {

if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > delayTime) {
    lastMsg = now;
    
    sensors_event_t event;
    
    dht.temperature().getEvent(&event);
    String t = String(event.temperature);
    dht.humidity().getEvent(&event);
    String h = String(event.relative_humidity);
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
      //Serial.print("Publish message: ");
      //Serial.println(h);
      client.publish("Humidade", h.c_str());
    }
  }
  
  
}
