# Identificação de dispositivos por WiFi SSID

## Intro

Cada dispositivo que monitoriza um paciente, utiliza um Espressif Esp32-S3-N16R8, que tem capacidades WiFi e Bluetooth (BLE).
Como estes ICs tem a capacidade de funcionarem com um AP (Access Point) e como uma STA (Station), podemos conectar a outros dispositivos enquanto temos o nosso próprio AP.

### Implementação

O tamanho máximo de um SSID para um WiFi AP é de 32 bytes, o que implica que se o SSID só conter caracteres UTF8, podemos ter 32 caracteres.
No espirito de facilitar o reconhecimento destes APs pelo programador, cada SSID deverá começar com `DIS<Tipo>`, este identificador é para todos os tipos de dispositivo e segue a seguinte estrutura:

|      **Prefixo**       | Especificador Dispositivo |
| :--------------------: | ------------------------- |
|         _DIS_          | 3 chars                   |
| Sempre presente e fixo |                           |

Em exemplo, o identificador para o dispositivo que monitoriza pacientes seria: **DISPMD**

Também devemos ter em conta que o identificador do dispositivo utiliza um SSID de WiFi para comunicar a sua presença e tipo de funcionamento,
logo, a definição do estado do dispositivo é algo calculado antes do mesmo publicitar a sua existência na rede, de maneira a evitar alterar
um valor que é "esperado" ficar fixo na maioria do tempo.

Todos os SSIDs para este tipo de dispositivo devêm seguir esta estrutura:

|   **Prefixo**   | ID Dispositivo | Sep | ID Paciente | Sep |                   Estado do Dispositivo                    | Sep |                                               Modo Routing                                               |
| :-------------: | :------------: | :-: | :---------: | :-: | :--------------------------------------------------------: | :-: | :------------------------------------------------------------------------------------------------------: |
|    _DISPMD_     |    8 chars     |  -  |   8 chars   |  -  |                          4 chars                           |  -  |                                                 3 chars                                                  |
| Sempre presente |   1º Octeto    |     |  2º Octeto  |     | Funcionamento dos componentes e outros estados de operação |     | Indica o comportamento que o dispositivo ira ter quando receber data par rotear para outros dispositivos |

Em exemplo, o SSID para um dispositivo que monitoriza pacientes seria: **DISPMD**u3ht51bk-alf8993v-good-101

### Multi Node Concerns

As funcionalidades principais de monitorização do estado do paciente correm no primeiro core, e a funcionalidade de nó na mesh correm no segundo core.
