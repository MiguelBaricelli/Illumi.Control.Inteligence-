#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid     = "Rede";
const char* password = "rede123";

const char* mqttServer = "broker.hivemq.com";
const int   mqttPort   = 1883;
const char* clientId   = "nodemcu_ilum_001";

const char* topicLuminosidade = "iluminacao/luminosidade";
const char* topicPresenca     = "iluminacao/presenca";
const char* topicLed          = "iluminacao/led";

const int pinoPIR = D5;
const int pinoLED = D2;

const int limiarLuminosidade = 500;

WiFiClient espClient;
PubSubClient client(espClient);

void conectarWifi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP atribuído: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");

    if (client.connect(clientId)) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou. Código de erro: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinoPIR, INPUT);
  pinMode(pinoLED, OUTPUT);
  digitalWrite(pinoLED, LOW);

  conectarWifi();

  client.setServer(mqttServer, mqttPort);

  conectarMQTT();
}

void loop() {
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop();

  int valorLDR  = analogRead(A0);
  int estadoPIR = digitalRead(pinoPIR);

  bool ambienteEscuro    = (valorLDR < limiarLuminosidade);
  bool presencaDetectada = (estadoPIR == HIGH);

  if (ambienteEscuro && presencaDetectada) {
    digitalWrite(pinoLED, HIGH);
    Serial.println("[AÇÃO] LED LIGADO – presença detectada em ambiente escuro.");
  } else {
    digitalWrite(pinoLED, LOW);
    Serial.println("[AÇÃO] LED APAGADO.");
  }

  String msgLum      = String(valorLDR);
  String msgPresenca = presencaDetectada ? "1" : "0";
  String msgLed      = (ambienteEscuro && presencaDetectada) ? "ON" : "OFF";

  client.publish(topicLuminosidade, msgLum.c_str());
  client.publish(topicPresenca,     msgPresenca.c_str());
  client.publish(topicLed,          msgLed.c_str());

  Serial.print("[MQTT] Luminosidade: "); Serial.println(msgLum);
  Serial.print("[MQTT] Presença: ");     Serial.println(msgPresenca);
  Serial.print("[MQTT] LED: ");          Serial.println(msgLed);

  delay(2000);
}