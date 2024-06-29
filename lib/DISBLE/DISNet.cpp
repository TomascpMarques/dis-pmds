
#include <cinttypes>
#include "DISNet.h"
#include <UUID.h>

Packet::Packet(uint8_t ttl, char destination[36 + 1], PacketPriority priority)
{
    UUID uuidGen;

    this->ttl = ttl;
    strncpy(this->destination, destination, 37);

    // ID generation from UUID
    uuidGen.generate();
    strncpy(this->id, uuidGen.toCharArray(), 37);

    this->payload = Payload();

    this->priority = priority;

    this->path_length = 0;
    *this->path = {};
}

int Packet::AddNodeToPath(NodeConnectionInfo_t *node)
{
    if (this->path_length + 1 > 4)
    {
        return -1;
    }

    this->path[this->path_length++] = node;
    return this->path_length;
}

void Packet::CycleTTL()
{
    if (this->ttl - 1 < 0)
        return;

    this->ttl -= 1;
}

const char *Packet::GetDestination()
{
    return destination;
}

PacketPriority Packet::GetPacketPriority()
{
    return this->priority;
}

NodeConnectionInfo_t *Packet::GetNodeFromPath(uint index)
{
    if (index >= this->path_length)
        return nullptr;

    return this->path[index];
}

Payload::Payload(String content)
{
    this->payload = content;
    this->length = content.length();
}

int Payload::SetPayload(String content)
{
    this->payload = content;
    this->length = content.length();
    return this->length;
}

uint Payload::GetLength()
{
    return length;
}

String Payload::GetPayload()
{
    return payload;
}

Node::Node(String id, BLEAdvertisedDevice *deviceId)
{
    this->id = id;
    this->deviceId = deviceId->getAddress().toString().c_str();
}

Node::Node(BLEAdvertisedDevice *device)
{
    Node(RandomCharSequenceLen32(), device);
}

Node::Node(String deviceId)
{
    this->id = RandomCharSequenceLen32();
    this->deviceId = deviceId;
}

String Node::GetId()
{
    return id;
}

String Node::GetDeviceId()
{
    return this->deviceId;
}

/// @returns Nothing
/// @brief
void PacketQueue::SortQueueByPriority()
{
    const int MAX_QUEUE_SIZE = PacketPriority::__PRIORITY_COUNT * PACKET_QUEUE_COUNT;

    int empty_spots = 0;
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i)
    {
        if (i % PACKET_QUEUE_COUNT == 0 and !this->queue[i])
        {
            empty_spots = 0;
            if (i == (MAX_QUEUE_SIZE - 1) - 2 and this->queue[i])
            {
                break;
            }
        }

        if (!this->queue[i])
        {
            empty_spots += 1;
            continue;
        }

        if (i > 0 && this->queue[i])
        {
            this->queue[i - (empty_spots--)] = this->queue[i];
            this->queue[i] = nullptr;
            continue;
        }
    }
}

PacketQueue::PacketQueue()
{
    for (int i = 0; i < PacketPriority::__PRIORITY_COUNT * PACKET_QUEUE_COUNT; ++i)
    {
        this->queue[i] = nullptr;
    }
}

PacketQueue::QueueResult PacketQueue::InsertPacket(Packet *packet)
{
    int offset = packet->GetPacketPriority();
    if (offset > PacketPriority::__PRIORITY_COUNT || offset < 0)
        return QueueResult::ERR_PacketPriorityOutOfBounds;

    int i = -1;
    for (int j = 0; j < PACKET_QUEUE_COUNT; ++j)
        if (!queue[(PACKET_QUEUE_COUNT * offset) + j])
        {
            i = j;
            break;
        }

    if (i < 0)
        return QueueResult::ERR_FailedAttemptToInsertPacket;

    queue[PACKET_QUEUE_COUNT * offset + i] = packet;
    return QueueResult::SUC_OK;
}

bool ScalablePacketQueue::__T_PriorityHasPacket(PacketPriority priority)
{
    return this->queue[priority];
}

ScalablePacketQueue::ScalablePacketQueue()
{
    this->metaData = ScalablePacketQueue::PacketQueueMeta();
    *this->queue = {};

    for (int i = 0; i < PacketPriority::__PRIORITY_COUNT; ++i)
        this->queue[i] = nullptr;
}

void ScalablePacketQueue::PushPacket(Packet *packet)
{
    N *new_node = N::Default();
    new_node->self = packet;

    if (!this->queue[packet->GetPacketPriority()])
    {
        this->queue[packet->GetPacketPriority()] = new_node;
        this->metaData.IncrementPacketCount(packet->GetPacketPriority());
        return;
    }

    N *node = this->queue[packet->GetPacketPriority()];
    while (node->next)
        node = node->next;

    new_node->previous = node;
    node->next = new_node;

    this->metaData.IncrementPacketCount(packet->GetPacketPriority());
    return;
}

Packet *ScalablePacketQueue::PopPacket(PacketPriority priority)
{
    if (!this->queue[priority])
        return nullptr;

    N *node = this->queue[priority];

    if (node->next)
    {
        this->queue[priority] = node->next;
        this->queue[priority]->previous = nullptr;
    }
    else
    {
        this->queue[priority] = nullptr;
    }

    Packet *p = node->self;
    free(node);

    this->metaData.IncrementPacketCount(priority);
    return p;
}

NetworkNode::NetworkNode()
{
    this->packetQueue = new PacketQueue();
}

BLENetworkNode::BLENetworkNode(BLEServer *s /* , Node *n */)
{
    // this->node = n;

    // BLE client and server registration / scanner referencing
    this->pClient = BLEDevice::createClient();
    this->pServerReference = s;
    this->pBleScanner = BLEDevice::getScan();

    // Criação da identificação do serviço de comunicação BLE
    this->pCommunicationService =
        this->pServerReference->createService(
            this->communicationServiceUUID
            // ---
        );

    // Criação da caracteristica do serviço de comunicação.
    uint32_t comsServiceRxCharacteristics =
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY;

    // Criação da caracteristica de receção de informação.
    this->pRXCharacteristic =
        this->pCommunicationService->createCharacteristic(
            this->rx_CharacteristicUUID,
            comsServiceRxCharacteristics
            // --------------------------------------
        );

    // Criação da caracteristica do serviço de comunicação.
    uint32_t comsServiceTxCharacteristics =
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY;

    // Criação da caracteristica de receção de informação.
    this->pRXCharacteristic =
        this->pCommunicationService->createCharacteristic(
            this->tx_CharacteristicUUID,
            comsServiceTxCharacteristics
            // --------------------------------------
        );
}

void BLENetworkNode::SetAdvertisedDeviceCallbacks(BLEAdvertisedDeviceCallbacks *pCallbacks)
{
    this->pAdvertisedDeviceCallbacks = pCallbacks;
}
