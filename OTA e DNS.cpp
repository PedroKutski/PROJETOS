//OTA E DNS PELA REDE WIFI WEB SERVER, O OTA FAZ A ATUALIZAÇÃO DA ESP32 E O DNS FACILITA O MEIO DE COMUNICAÇÃO WIFI

#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <ESPmDNS.h>

// Configurações da rede WiFi
const char* ssid = "INTELBRAS - JOAO";
const char* password = "jps510123";

// Cria um servidor web na porta 80
WebServer server(80);

// Página HTML para upload do firmware
const char* serverIndex = 
"<form method='POST' action='/update' enctype='multipart/form-data'>"
"<input type='file' name='firmware'>"
"<input type='submit' value='Atualizar Firmware'>"
"</form>";

// Função para lidar com a página de upload
void handleRoot() {
  server.send(200, "text/html", serverIndex);
}

// Função para lidar com o upload do firmware
void handleUpdate() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Atualizando: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // Inicia a atualização com um tamanho desconhecido
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { // Finaliza a atualização
      Serial.printf("Sucesso na atualização: %u bytes\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}

// Função para reiniciar o ESP32 após a atualização
void handleUpdateFinished() {
  server.send(200, "text/plain", (Update.hasError()) ? "Falha na atualização!" : "Atualização bem-sucedida, reiniciando...");
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  // Iniciar mDNS
  if (!MDNS.begin("meuesp")) {
    Serial.println("Erro ao iniciar mDNS");
    return;
  }
  Serial.println("mDNS iniciado: http://meuesp.local");

  // Roteamento das páginas
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_POST, handleUpdateFinished, handleUpdate);

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();
}
