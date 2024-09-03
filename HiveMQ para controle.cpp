#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

//---- WiFi settings
const char* ssid = "INTELBRAS - JOAO";  // Nome da rede WiFi
const char* password = "jps510123";     // Senha da rede WiFi

//---- MQTT Broker settings
const char* mqtt_server = "xxxx"; // Seu server MQTT do HiveMQ
const char* mqtt_username = "xxxx"; // Nome de usuário MQTT
const char* mqtt_password = "xxxx"; // Senha MQTT
const int mqtt_port = 8883; // Porta do broker MQTT

WiFiClientSecure espClient; // Cliente seguro para WiFi
PubSubClient client(espClient); // Cliente MQTT usando o cliente seguro

#define MSG_BUFFER_SIZE (50) // Tamanho do buffer de mensagem
char msg[MSG_BUFFER_SIZE];  // Buffer para armazenar mensagens

const int gpio2 = 2; // Pino para o controle do LED
const char* led_topic = "ledControl"; // Tópico MQTT para controlar o LED

static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Declaração das funções
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial

  pinMode(gpio2, OUTPUT); // Configura o pino do LED como saída
  digitalWrite(gpio2, LOW); // Garante que o LED está apagado

  WiFi.mode(WIFI_STA); // Configura o modo WiFi como estação
  WiFi.begin(ssid, password); // Conecta-se à rede WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Espera até estar conectado
  }

  espClient.setCACert(root_ca); // Configura o certificado CA
  client.setServer(mqtt_server, mqtt_port); // Configura o servidor MQTT
  client.setCallback(callback); // Define a função de callback para mensagens MQTT
}

void loop() {
  if (!client.connected()) {
    reconnect(); // Tenta reconectar ao MQTT se desconectado
  }
  client.loop(); // Mantém a comunicação MQTT
}

// Função para reconectar ao broker MQTT
void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      client.subscribe(led_topic); // Inscreve-se no tópico do LED
    } else {
      delay(5000); // Espera antes de tentar novamente
    }
  }
}

// Função chamada quando uma mensagem MQTT é recebida
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i]; // Converte o payload para string
  }

  if (String(topic) == led_topic) { // Verifica se a mensagem é para o tópico do LED
    if (incommingMessage == "ON") {
      digitalWrite(gpio2, HIGH);  // Liga o LED
    } else if (incommingMessage == "OFF") {
      digitalWrite(gpio2, LOW);   // Desliga o LED
    }
  }
}
