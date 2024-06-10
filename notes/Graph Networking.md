# Networking

Cada ESP32 consegue comunicar com os outros esp32, através de BLE. O problema agora é:

- Encontrar outros dispositivos iguais a este;
- Encontrar uma rota até ao um nó que tenha a informação que queremos ou o servidor principal;
- Implementar uma queue para os pedidos/routing e respostas dos mesmos;
- Incluir latência entre os nós nos calculos;
- Need to handle node disconnects, change the best path according to it;
- Some nodes might connect to the next node instead of the server if the node is closer than the server;
- Need BLE characteristics for inter device value exchange;

TIPS for finding the next node:

1. Only consider the nodes after the ones already traveled to avoid repetition;
