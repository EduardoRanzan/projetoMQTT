# 📟 Projeto NodeMCU + MQTT
Este projeto foi desenvolvido para a disciplina de sistemas embarcados de Sistemas da Informação de 2025

## Exemplo do funcionamento do projeto na prática
![Gif de funcionamento do projeto](/img/funcionamento.gif)
## Forma na qual enviamos as informações (mqqt-explorer)
![mqtt-explorer](/img/ex-server.png)

Download aqui -> https://mqtt-explorer.com/

---
### 🔌 Bibliotecas Utilizadas
ESP8266WiFi.h – Conecta o NodeMCU à rede Wi-Fi.

PubSubClient.h – Faz a comunicação com o broker MQTT.

ArduinoJson.h – Faz o parsing e geração de mensagens JSON.

---
### 📶 Conexão Wi-Fi
As credenciais estão definidas nas variáveis:

```cpp
const char *ssid = "sua-rede-wifi";
const char *password = "senha";
```

A função setup_wifi() tenta conectar à rede e imprime o IP local ao conectar com sucesso.

---
### 🌐 Broker MQTT
Configuração do broker:

```cpp
const char *mqtt_server = "";
const int mqtt_port = 1883;
```

---

### 🔁 Tópicos MQTT

#### Publicação (pub)
Após receber e tratar um comando, o ESP publica o estado do LED correspondente nos tópicos:

- casa/cozinha/led/status
- casa/quarto1/led/status
- casa/quarto2/led/status
- casa/quarto3/led/status
- casa/varanda/led/status

#### Subscrição (sub)

Sequindo o que ensina o protocolo mqtt o ESP escuta mensagens nestes tópicos para controlar os LEDs:

- casa/cozinha/led/set
- casa/quarto1/led/set
- casa/quarto2/led/set
- casa/quarto3/led/set
- casa/varanda/led/set

### 💡 Controle dos LEDs

Os LEDs estão conectados aos pinos:
```ini
LED_PINS = {D0, D1, D2, D3, D4};
``` 

Esses pinos são configurados como saída no setup() e iniciam desligados (LOW).


### 📩 Callback MQTT
A função callback() é chamada sempre que uma mensagem MQTT é recebida.

Espera um JSON assim:
```json
{
  "estado": "ON"
}
```
Ou:
```json
{
  "estado": "OFF"
}
```

Se for "ON", o LED correspondente é ligado.

Se for "OFF", ele é desligado.

A resposta (o mesmo JSON) é publicada no tópico de status.

### 🔄 Reconexão Automática
A função reconnect() garante que o ESP reconecte ao broker MQTT caso a conexão seja perdida. Também reinscreve em todos os tópicos após reconectar.

### 🛠️ setup()
- Inicializa comunicação serial.
- Configura os pinos dos LEDs.
- Conecta ao Wi-Fi.
- Configura o broker MQTT.
- Define a função de callback.

### 🔁 loop()
- Verifica se o cliente MQTT está conectado.
- Se não estiver, chama reconnect().
- Mantém a conexão com client.loop().