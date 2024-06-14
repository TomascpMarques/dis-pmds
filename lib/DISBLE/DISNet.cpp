
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

Node::Node(char id[32 + 1], BLEAdvertisedDevice *device)
{
    strcpy(this->id, id);
    this->deviceId = device->getAddress().toString().c_str();
}

Node::Node(BLEAdvertisedDevice *device)
{
    Node(RandomCharSequenceLen32(), device);
}

Node::Node(String deviceId)
{
    strcpy(this->id, RandomCharSequenceLen32());
    this->deviceId = deviceId;
}

char *Node::GetId()
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
