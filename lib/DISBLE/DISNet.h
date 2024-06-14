#pragma once

/* ---- BLE Libraries ---- */
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEUUID.h>
#include <BLE2902.h>
/* ---- BLE Libraries ---- */

/* ---- Private Libs ---- */
#include <MsgPack.h>
#include <DIS.h>
/* ---- Private Libs ---- */

class Node
{
private:
    // The address must be unique
    char id[32 + 1];
    String deviceId;

public:
    // ---- Constructors START ----
    Node(char id[32 + 1], BLEAdvertisedDevice *deviceId);
    Node(BLEAdvertisedDevice *deviceId);
    Node(String id);
    // ---- Constructors END ----

    // ---- Getters START ----
    char *GetId();
    String GetDeviceId();
    // ---- Getters END ----
};

class NetworkNode
{
    // uint8_t CalculateNodeWeight(std::string destination, uint8_t tolerance);
private:
public:
    ~NetworkNode(){};
    NetworkNode();
};

struct NodeConnectionInfo_t
{
    MsgPack::str_t nodeAddress;
    int rssi;

    MSGPACK_DEFINE(rssi, nodeAddress);
};

struct Payload
{
    MsgPack::str_t payload;
    uint length;

    ~Payload(){};
    Payload(String content);
    Payload() : payload(""), length(0){};

    // ---- Setters START ----
    int SetPayload(String content);
    // ---- Setters END ----

    // ---- Getters START ----
    uint GetLength();
    String GetPayload();
    // ---- Getters END ----

    MSGPACK_DEFINE(length, payload);
};

enum PacketPriority
{
    Severe,
    High,
    Regular,
    None,
    __PRIORITY_COUNT
};

class Packet
{
private:
    uint8_t ttl;
    PacketPriority priority;
    char id[36 + 1];
    char destination[36 + 1];

    uint8_t path_length;
    NodeConnectionInfo_t *path[4];

public:
    Payload payload;

    // ---- Constructor ----
    Packet(uint8_t ttl, char destination[36 + 1], PacketPriority priority);
    Packet() : ttl(14), priority(PacketPriority::Regular), destination("emtpy"), id("empty"){};

    // ---- Getters START ----
    const char *GetDestination();
    PacketPriority GetPacketPriority();
    NodeConnectionInfo_t *GetNodeFromPath(uint index);
    // ---- Getters END ----

    // ---- Behavior START ----
    void CycleTTL();
    int AddNodeToPath(NodeConnectionInfo_t *node);
    // ---- Behavior END ----
};

static const int PACKET_QUEUE_COUNT = 3;

class PacketQueue
{
public:
    // --- Class inner types START ---
    typedef Packet *Queue[PacketPriority::__PRIORITY_COUNT * PACKET_QUEUE_COUNT];
    enum QueueResult
    {
        ERR_PacketPriorityOutOfBounds,
        ERR_FailedAttemptToInsertPacket,
        SUC_OK,
    };
    // --- Class inner types END ---

    Queue queue;

    // --- Construtores START ---
    PacketQueue();
    // --- Construtores END ---

    // ---- Behavior START ----
    PacketQueue::QueueResult InsertPacket(Packet *packet);
    void SortQueueByPriority();
    // ---- Behavior END ----
};