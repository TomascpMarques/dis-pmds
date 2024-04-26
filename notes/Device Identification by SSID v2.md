# Identificação de dispositivos por WiFi SSID

## Intro

Cada dispositivo que monitoriza um paciente, utiliza um Espressif Esp32-S3-N16R8, que tem capacidades WiFi e Bluetooth (BLE).
Como estes ICs tem a capacidade de funcionarem com um AP (Access Point) e como uma STA (Station), podemos conectar a outros dispositivos enquanto temos o nosso próprio AP.

### Implementação - v2

Embora o ssid tenha no máximo 32 caracteres disponiveis, a implementação anterior deixa a desejar em termos
de segurança e fiabilidade na veracidade da identificação dos dispositivos.
Na versão anterior, dados como o ID do dispositivo, ID do paciente, estado do dispositivo e modo de routing de dados eram incluidos no ssid.

Esta versão tenta remover a representação direta desses dados, em favor de um SSID mais fiável e secreto à maquina.

A nova técnica involve a utilização do algoritmo AES128 para validação do SSID, como um ID confiavel. Sendo a informação outrora demonstrada no SSID, necessita-se agora que esta informação seja lida de alguma outra maneira, pós conexão ou pedido ao dispositivo alvo de avaliação.

A maneira desejada para esta versão da implementação será através da leitura de caracteristicas expostas por serviços BLE. Que, no caso de não existir cobertura BLE, tentaria-se transmitir os dados por WiFi utilizando algum protocolo a especificar.

### Pipeline para gerar um SSID

A criação de um SSID passa pelos seguintes passos:

1. Criação de uma string com:
    2. Tipo do dispositivo;
    2. ID de 12 caracteres do dispositivo;
1. Aplicação do algoritmo AES128.

Como exemplo temos os seguintes dados do dispositivo:

**Dados do Dispositivo - A1:**

```json
{
    "tipo": "PMD",
    "id": "8e6902b386ec",
}
```

Estes dados seriam transformados na seguinte string:

```md
PMD8e6902b386ec
```

Que se tornaria na hashed string:

```md
f95c75bb711e4cdb3949d73bef5ca2ef
```
