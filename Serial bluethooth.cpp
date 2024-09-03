//SERVIDOR BLUETHOOTH - FAZ A COMUNICAÇÃO VIA BLUETHOOTH APENAS

#include <BluetoothSerial.h>  // Biblioteca para comunicação Bluetooth

#define PIN 18  // Definição do pino que será usado para controle

BluetoothSerial SerialBT;  // Criação do objeto para comunicação Bluetooth

void setup() {
  Serial.begin(115200);          // Inicializa a comunicação serial com o computador
  SerialBT.begin("ESP32_K");     // Inicializa o Bluetooth com o nome "ESP32_K"
  
  pinMode(PIN, OUTPUT);          // Define o pino como saída (para controlar o LED)
}

void loop() {
  if (SerialBT.available()) {    // Verifica se há dados disponíveis no Bluetooth
    String command = SerialBT.readStringUntil('\n');  // Lê o comando enviado pelo Bluetooth
    command.trim();              // Remove espaços em branco extras no início e no final do comando

    // Verifica o comando recebido e executa a ação correspondente
    if (command == "1") {  // Se o comando for "1"
      digitalWrite(PIN, HIGH);          // Liga o pino (LED aceso)
      SerialBT.println("PINO 18 LIGADO");  // Envia uma confirmação pelo Bluetooth
      
    } else if (command == "0") {  // Se o comando for "0"
      digitalWrite(PIN, LOW);           // Desliga o pino (LED apagado)
      SerialBT.println("PINO 18 DESLIGADO");  // Envia uma confirmação pelo Bluetooth
    } else {  // Se o comando não for reconhecido
      SerialBT.println("COMANDO INVALIDO");  // Envia uma mensagem de erro pelo Bluetooth
    }
  }
}
