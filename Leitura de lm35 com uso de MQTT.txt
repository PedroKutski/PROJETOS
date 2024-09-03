#include <WiFi.h>
#include <PubSubClient.h>

// Define o pino do LM35
const int lm35Pin = 34;  // Pino ADC para o LM35 no ESP32

// Credenciais Wi-Fi e MQTT
const char* ssid = "INTELBRAS - JOAO"; // Nome da rede
const char* password = "jps510123"; // Senha da rede
const char* mqttServer = "broker.hivemq.com"; // Servidor público MQTT

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

// Função para configurar o Wi-Fi
void setupWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Função callback que será chamada quando uma mensagem for recebida
void callback(char* topic, byte* payload, unsigned int length) {
  // Aqui você pode adicionar códigos adicionais para lidar com novos tópicos se necessário
}

// Função para reconectar ao broker MQTT
void reconnect() {
  while (!client.connected()) {
    String clientId = "pk51" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // Inscrição em tópicos, se necessário
    } else {
      delay(5000);
    }
  }
}

// Função para ler a temperatura do LM35
float readLM35() {
  float total = 0;
  int readings = 10;  // Número de leituras para fazer a média

  for (int i = 0; i < readings; i++) {
    total += analogRead(lm35Pin);  // Soma várias leituras
    delay(10);  // Pequeno atraso entre as leituras
  }

  float analogValue = total / readings;  // Calcula a média
  float voltage = analogValue * (3.3 / 4095.0);  // Converte para tensão
  float temperature = voltage * 100;  // Converte para temperatura em °C
  return temperature;
}

void setup() {
  analogReadResolution(12);  // Configura o ADC para 12 bits
  setupWifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    float temperature = readLM35();
    snprintf(msg, 50, "Temperatura: %.2f", temperature);
    client.publish("sensor/temperatura", msg);
  }

  delay(500);
}
