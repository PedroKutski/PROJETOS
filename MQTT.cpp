//PROTOCOLO DE COMUNICAÇÃO DE QUALQUER REDE COM BROKER PÚBLICO
//neste codigo é possivel controlar por meio de aplicativo o led do ESP32 pin 2 
#include <WiFi.h>
#include <PubSubClient.h>

// Define o pino do LED
const int ledPin = 2;

// Credenciais Wi-Fi e MQTT - Responsavel por fazer a conexão wifi
const char* ssid = "INTELBRAS - JOAO"; //Nome da rede
const char* password = "jps510123"; //Senha da rede
const char* mqttServer = "broker.hivemq.com"; //servidor publico

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";  
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];  // Concatena o payload na string
  }

  // Verifica se a mensagem pertence ao topico "on"
  if (String(topic) == "on") { //TOPICO
    if (msg == "L") { //payload
      digitalWrite(ledPin, HIGH);  // Liga o LED
    }
  }
  // Verifica se a mensagem pertence ao topico "off"
  else if (String(topic) == "off") { //TOPICO
    if (msg == "D") { //payload
      digitalWrite(ledPin, LOW);   // Desliga o LED
    }
  }
  //aqui voce pode continuar o seu codigo adicionando novos topicos 
}

void reconnect() { //Conexao
  while (!client.connected()) {
    String clientId = "pedrok7824987" + String(random(0xffff), HEX);  // id do cliente, pode ser modificado
    if (client.connect(clientId.c_str())) {
      client.subscribe("on");  // topico "on"
      client.subscribe("off"); // topico "off"
    } else {
      delay(5000);
    }
  }
}


void setup() {
  pinMode(ledPin, OUTPUT);
  setupWifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(500);
}
