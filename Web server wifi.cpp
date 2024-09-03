//SERVIDOR WEB WIFI - FAZ A COMUNICAÇÃO VIA WIFI APENAS

#include <WiFi.h>  // Biblioteca para comunicação Wi-Fi

#define PIN 2  // Definição do pino que será usado para controle

const char* ssid = "INTELBRAS - JOAO";  // Substitua por seu SSID
const char* password = "jps510123";  // Substitua pela sua senha

WiFiServer server(80);  // Criação do servidor na porta 80

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial com o computador

  pinMode(PIN, OUTPUT);  // Define o pino como saída (para controlar o LED)
  digitalWrite(PIN, LOW);  // Inicializa o pino como desligado

  // Conecta à rede Wi-Fi
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado ao WiFi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.begin();  // Inicia o servidor
}

void loop() {
  WiFiClient client = server.available();  // Verifica se há clientes conectados

  if (client) {
    Serial.println("Novo cliente conectado");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Envia uma resposta padrão ao cliente HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<form action=\"/1\"><input type=\"submit\" value=\"Ligar PIN 18\"></form>");
            client.println("<form action=\"/0\"><input type=\"submit\" value=\"Desligar PIN 18\"></form>");
            client.println("</html>");
            client.println();
            break;
          } else {
            if (currentLine.startsWith("GET /1")) {
              digitalWrite(PIN, HIGH);
              Serial.println("PINO 2 LIGADO");
            } else if (currentLine.startsWith("GET /0")) {
              digitalWrite(PIN, LOW);
              Serial.println("PINO 2 DESLIGADO");
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
