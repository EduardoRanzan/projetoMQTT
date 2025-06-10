#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Wi-Fi
const char *ssid = "sua-rede-wifi";
const char *password = "senha";

// Broker MQTT
const char *mqtt_server = "";
const int mqtt_port = 1883;

// Tópicos MQTT
const char *sub_topics[] = {
  "casa/cozinha/led/set",
  "casa/quarto1/led/set",
  "casa/quarto2/led/set",
  "casa/quarto3/led/set",
  "casa/varanda/led/set"
};

const char *pub_topics[] = {
  "casa/cozinha/led/status",
  "casa/quarto1/led/status",
  "casa/quarto2/led/status",
  "casa/quarto3/led/status",
  "casa/varanda/led/status"
};

// Pinos dos LEDs correspondentes
const uint8_t LED_PINS[] = {D0, D1, D2, D3, D4};
const uint8_t NUM_LEDS = sizeof(LED_PINS);

// Cliente Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Conectar Wi-Fi
void setup_wifi() {
  Serial.print("Conectando na rede Wi-Fi: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Wi-Fi conectado! IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  // Converte payload em String
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  // Cria o objeto JSON
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, msg);

  if (error) {
    Serial.printf("Erro ao interpretar JSON em %s: %s\n", topic, error.c_str());
    return;
  }

  // Obtém o comando do JSON
  String cmd = doc["estado"];
  cmd.toUpperCase();  // Garante consistência

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    if (String(topic) == sub_topics[i]) {
      if (cmd == "ON") {
        digitalWrite(LED_PINS[i], HIGH);
        Serial.printf("Recebido 'ON' para %s → LED %d ligado\n", topic, i);
      } else if (cmd == "OFF") {
        digitalWrite(LED_PINS[i], LOW);
        Serial.printf("Recebido 'OFF' para %s → LED %d desligado\n", topic, i);
      } else {
        Serial.printf("Comando desconhecido em %s: %s\n", topic, cmd.c_str());
      }

      // Reenvia o mesmo JSON para o tópico de status
      String output;
      serializeJson(doc, output);
      client.publish(pub_topics[i], output.c_str(), true);
      break;
    }
  }
}


// Conexão com o broker MQTT
void reconnect() {
  while (!client.connected()) {
    String clientId = "NodeMCU-";
    clientId += String(random(0xffff), HEX);
    Serial.print("Tentando conectar ao broker MQTT... ID = ");
    Serial.println(clientId);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker!");

      // Inscrever em todos os tópicos
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        client.subscribe(sub_topics[i]);
        Serial.print("Inscrito no tópico: ");
        Serial.println(sub_topics[i]);
      }
    } else {
      Serial.print("Falha na conexão. Código: ");
      Serial.print(client.state());
      Serial.println(" – tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// Setup
void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("=== Iniciando NodeMCU + MQTT ===");

  // Configura pinos como saída
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// Loop principal
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
