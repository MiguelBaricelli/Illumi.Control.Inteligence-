# Illumi.Control.Inteligence

Sistema de automação residencial para controle inteligente de iluminação baseado em ESP32, conectividade Wi-Fi (TCP/IP) e protocolo MQTT. O projeto permite acionar luzes remotamente por qualquer cliente MQTT, além de detectar presença via sensor PIR para automação local.

---

# Índice

- [i) Descrição do funcionamento e uso](#i-descrição-do-funcionamento-e-uso)
- [ii) Software desenvolvido e documentação do código](#ii-software-desenvolvido-e-documentação-do-código)
- [iii) Descrição do hardware utilizado](#iii-descrição-do-hardware-utilizado)
- [iv) Documentação das interfaces, protocolos e módulos de comunicação](#iv-documentação-das-interfaces-protocolos-e-módulos-de-comunicação)
- [v) Comunicação/controle via internet (TCP/IP) e uso do MQTT](#v-comunicaçãocontrole-via-internet-tcpip-e-uso-do-mqtt)
- [Instalação e configuração passo a passo](#instalação-e-configuração-passo-a-passo)
- [Exemplo de mensagens MQTT](#exemplo-de-mensagens-mqtt)
- [Licença e autores](#licença-e-autores)

---

# i) Descrição do funcionamento e uso

O projeto implementa um **nó de iluminação inteligente** com as seguintes funcionalidades:

- **Acionamento remoto:** através de publicação em tópico MQTT (ex: `illumi/luz/cmd`) com payloads `"ON"` e `"OFF"`.
- **Detecção de presença automática:** um sensor PIR (HC-SR501) liga automaticamente a luz quando detecta movimento, podendo ser combinado com um timer de desligamento.
- **Feedback de estado:** o ESP32 publica periodicamente o status da lâmpada (ligada/desligada) e o estado do sensor.
- **Controle local manual:** (opcional) um botão físico para override.

## Como reproduzir o projeto

1. Monte o hardware conforme esquema elétrico.
2. Instale o software necessário (Arduino IDE ou PlatformIO).
3. Configure as credenciais Wi-Fi e MQTT no arquivo `config.h`.
4. Faça o upload do código para a ESP32.
5. Controle a iluminação usando qualquer cliente MQTT.

---

## Exemplo de uso prático

### Cenário 1

Usuário envia `"ON"` para `illumi/luz/cmd` → ESP32 recebe → aciona relé → luz acende.

### Cenário 2

Sensor PIR detecta movimento → ESP32 publica `"1"` em `illumi/sensor/presenca` → luz acende automaticamente.

### Cenário 3

Integração com Home Assistant ou Node-RED através do broker MQTT.

---

# ii) Software desenvolvido e documentação do código

## Linguagem utilizada

- **C++**
- Framework Arduino para ESP32

---

## Principais funções

### `setupWiFi()`

Conecta o ESP32 à rede Wi-Fi.

### `setupMQTT()`

Configura o cliente MQTT e callback.

### `callback(char* topic, byte* payload, unsigned int length)`

Processa mensagens recebidas.

### `readPIRSensor()`

Lê o estado do sensor PIR.

### `publishStatus()`

Publica estado da luz e sensor.

### `loop()`

Mantém conexões e executa rotinas periódicas.

---

## Diagrama funcional

```text
[ Wi-Fi Router ] <--> [ Broker MQTT ] <--> [ Cliente MQTT ]
        |
        v
      [ ESP32 ]
        |
  +-----+------+
  |            |
[PIR]       [Relé]
                |
             [LED]
```

---

# iii) Descrição do hardware utilizado

## Lista de componentes

| Componente           | Modelo / Especificações | Quantidade | Função                 |
| -------------------- | ----------------------- | ---------- | ---------------------- |
| ESP32                | DevKit V1               | 1          | Controle principal     |
| Sensor PIR           | HC-SR501                | 1          | Detecção de movimento  |
| Relé                 | 1 canal 5V              | 1          | Acionamento da lâmpada |
| Fonte                | 5V 2A                   | 1          | Alimentação            |
| Lâmpada              | LED/Incandescente       | 1          | Carga controlada       |
| Botão                | Push button             | 1          | Controle manual        |
| Protoboard e jumpers | -                       | Diversos   | Montagem               |

---

## Ligações elétricas

```text
ESP32           PIR
3V3 ----------> VCC
GND ----------> GND
D13 <---------- OUT

ESP32           Relé
D12 ----------> IN
VIN ----------> VCC
GND ----------> GND
```

---

## Observações importantes

- O relé deve ser alimentado com 5V.
- A saída do PIR é compatível com 3.3V do ESP32.
- Alguns módulos de relé utilizam lógica invertida.

---

## Peças impressas em 3D

### Caixa protetora

- Medidas: 120x80x50 mm
- Espaço para ESP32, relé e sensor

### Suporte do PIR

- Ajuste angular

### Tampa para botão

- Acabamento externo

---

# iv) Documentação das interfaces, protocolos e módulos de comunicação

# Wi-Fi (TCP/IP)

| Item       | Descrição  |
| ---------- | ---------- |
| Modo       | Station    |
| Protocolo  | IPv4       |
| Porta MQTT | 1883       |
| Segurança  | WPA2       |
| Reconexão  | Automática |

---

# MQTT

## Configurações

| Item          | Valor               |
| ------------- | ------------------- |
| Versão        | MQTT 3.1.1          |
| Broker padrão | `broker.hivemq.com` |
| Porta         | 1883                |

---

## Tópicos MQTT

| Tópico                   | Direção   | Payload          | Descrição                  |
| ------------------------ | --------- | ---------------- | -------------------------- |
| `illumi/luz/cmd`         | Subscribe | `"ON"` / `"OFF"` | Controle da luz            |
| `illumi/luz/status`      | Publish   | `"ON"` / `"OFF"` | Estado da luz              |
| `illumi/sensor/presenca` | Publish   | `"1"` / `"0"`    | Estado do PIR              |
| `illumi/device/info`     | Publish   | JSON             | Informações do dispositivo |

---

## QoS utilizado

| Função   | QoS   |
| -------- | ----- |
| Comandos | QoS 1 |
| Sensores | QoS 0 |

---

# Comunicação Serial

| Configuração | Valor  |
| ------------ | ------ |
| Baud Rate    | 115200 |
| Formato      | 8N1    |

---

# GPIO

## PIR

- Entrada digital
- Pull-down interno

## Relé

- Saída digital
- HIGH = ligado
- LOW = desligado

---

# v) Comunicação/controle via internet (TCP/IP) e uso do MQTT

O projeto utiliza:

- Comunicação Wi-Fi baseada em TCP/IP
- Protocolo MQTT para troca de mensagens
- Controle remoto pela internet

---

## Arquitetura de comunicação

```text
[ Smartphone ]
       |
       v
[ Broker MQTT ]
       |
       v
[ ESP32 ] ---> [ Relé ] ---> [ Lâmpada ]
```

---

# Demonstração dos requisitos

## Controle remoto

Qualquer dispositivo conectado ao broker pode controlar a iluminação.

## MQTT

Toda troca de mensagens é feita via publish/subscribe.

## TCP/IP

A comunicação ocorre pela pilha TCP/IP do Wi-Fi.

---

# Exemplo utilizando HiveMQ

## Broker público

```text
broker.hivemq.com
Porta: 1883
```

---

## Teste remoto

1. Faça upload do código.
2. Configure broker HiveMQ.
3. Conecte pelo celular.
4. Publique `"ON"` em `illumi/luz/cmd`.
5. A lâmpada deverá acender.

---

# Integração com Home Assistant

```yaml
light:
  - platform: mqtt
    name: "Luz Inteligente"
    state_topic: "illumi/luz/status"
    command_topic: "illumi/luz/cmd"
    payload_on: "ON"
    payload_off: "OFF"
    optimistic: false
    qos: 1
```

---

# Instalação e configuração passo a passo

# Pré-requisitos

- Arduino IDE 1.8.19+
- ESP32 Board Package
- Driver CP210x ou CH340

---

# Bibliotecas necessárias

- `PubSubClient`
- `WiFi.h`

---

# Clonar repositório

```bash
git clone https://github.com/MiguelBaricelli/Illumi.Control.Inteligence-.git
```

---

# Configuração do Wi-Fi e MQTT

```cpp
const char* ssid = "SeuWiFi";
const char* password = "SuaSenha";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
```

---

# Upload do código

1. Selecione:
   - ESP32 Dev Module
   - Porta COM correta

2. Clique em Upload.

3. Abra o Serial Monitor em:

```text
115200 baud
```

---

# Verificação de funcionamento

O monitor serial deve mostrar:

```text
Conectado ao Wi-Fi
Conectado ao MQTT
```

---

# Teste MQTT

```bash
mosquitto_pub -h broker.hivemq.com -t "illumi/luz/cmd" -m "ON"
```

---

# Exemplo de mensagens MQTT

| Ação                | Payload | Tópico                   |
| ------------------- | ------- | ------------------------ |
| Ligar luz           | ON      | `illumi/luz/cmd`         |
| Desligar luz        | OFF     | `illumi/luz/cmd`         |
| Status da luz       | ON/OFF  | `illumi/luz/status`      |
| Movimento detectado | 1       | `illumi/sensor/presenca` |
| Sem movimento       | 0       | `illumi/sensor/presenca` |

---

# Possíveis melhorias

- OTA (Over-The-Air)
- Integração Alexa/Google Home
- Sensor de luminosidade LDR
- MQTT com TLS/SSL
- Dashboard Web
- Node-RED
- Aplicativo mobile

---

# Licença e autores

## Desenvolvedor

Miguel Baricelli

## Disciplina

Objetos Inteligentes Conectados – 5º Semestre – Faculdade Mackenzie

---
