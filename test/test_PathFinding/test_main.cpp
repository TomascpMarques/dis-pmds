#include <unity.h>
#include <WString.h>
#include <Arduino.h>

#include <DISNet.h>

void setUp(void)
{
}

void tearDown(void)
{
    // clean stuff up here
}

void test_packet_constructor()
{
    Packet *pkt = new Packet(14, "SuperCool", PacketPriority::Regular);
    TEST_ASSERT_TRUE(pkt);

    const char *destination = pkt->GetDestination();
    const bool comparison = strcmp(destination, "SuperCool") == 0;

    TEST_ASSERT_TRUE(comparison);
}

void test_add_packet_payload()
{
    Packet *pkt = new Packet(14, "SuperCool", PacketPriority::Regular);
    TEST_ASSERT_TRUE(pkt);

    String buffer = "bpm:38.2;spo:0.98";

    int wrote = pkt->payload.SetPayload(buffer);
    TEST_ASSERT_TRUE(wrote == 17);

    const String payload = pkt->payload.GetPayload();
    const int comparison = strcmp(buffer.c_str(), payload.c_str()) == 0;

    TEST_ASSERT_TRUE(comparison);
}

void test_add_node_to_path()
{
    Packet *pkt = new Packet(14, "SuperCool", PacketPriority::Regular);
    TEST_ASSERT_TRUE(pkt);

    NodeConnectionInfo_t node = {
        "",
        134,
    };

    int result = pkt->AddNodeToPath(&node);
    TEST_ASSERT_TRUE(result == 1);

    result = pkt->AddNodeToPath(&node);
    TEST_ASSERT_TRUE(result == 2);
}

void test_get_path_node()
{
    Packet *pkt = new Packet(14, "SuperCool", PacketPriority::Regular);
    TEST_ASSERT_TRUE(pkt);

    NodeConnectionInfo_t node = {
        "AAAA",
        1,
    };

    NodeConnectionInfo_t node1 = {
        "BBBB",
        2,
    };

    int result = pkt->AddNodeToPath(&node);
    TEST_ASSERT_TRUE(result == 1);

    result = pkt->AddNodeToPath(&node1);
    TEST_ASSERT_TRUE(result == 2);

    const NodeConnectionInfo_t *resNode = pkt->GetNodeFromPath(0);
    TEST_ASSERT_TRUE(resNode->rssi == 1);

    const NodeConnectionInfo_t *second = pkt->GetNodeFromPath(3);
    TEST_ASSERT_TRUE(!second);
}

void test_payload_class()
{
    Payload *pyl = new Payload("Supper");
    TEST_ASSERT_TRUE(pyl);

    const uint size = pyl->GetLength();
    TEST_ASSERT_TRUE(size == 6);

    const String payload = pyl->GetPayload();
    TEST_ASSERT_TRUE(payload);

    const bool comparison = strcmp(payload.c_str(), "Supper") == 0;
    TEST_ASSERT_TRUE(comparison);
}

void test_packet_queue()
{
    Packet *pkt = new Packet(14, "I", PacketPriority::Severe);

    Packet *pktt = new Packet(14, "II", PacketPriority::High);
    Packet *pktti = new Packet(14, "II.I", PacketPriority::High);
    Packet *pkttii = new Packet(14, "II.II", PacketPriority::High);

    Packet *pkttt = new Packet(14, "III", PacketPriority::Regular);

    PacketQueue *queue = new PacketQueue();

    PacketQueue::QueueResult inserted = queue->InsertPacket(pkt);
    TEST_ASSERT_TRUE(inserted == PacketQueue::QueueResult::SUC_OK);

    queue->InsertPacket(pktt);
    queue->InsertPacket(pkttt);

    queue->InsertPacket(pktti);
    queue->InsertPacket(pkttii);

    PacketQueue::QueueResult res = queue->InsertPacket(pkttii);
    TEST_ASSERT_TRUE(res == PacketQueue::QueueResult::ERR_FailedAttemptToInsertPacket);

    Packet *v1 = queue->queue[0];
    TEST_ASSERT_TRUE(strcmp(v1->GetDestination(), "I") == 0);

    Packet *v2 = queue->queue[3];
    TEST_ASSERT_TRUE(strcmp(v2->GetDestination(), "II") == 0);
    Packet *v21 = queue->queue[4];
    TEST_ASSERT_TRUE(strcmp(v21->GetDestination(), "II.I") == 0);
    Packet *v22 = queue->queue[5];
    TEST_ASSERT_TRUE(strcmp(v22->GetDestination(), "II.II") == 0);

    Packet *v3 = queue->queue[6];
    TEST_ASSERT_TRUE(strcmp(v3->GetDestination(), "III") == 0);

    delete queue;
}

void test_balance_queue()
{
    Packet *pkt = new Packet(14, "I", PacketPriority::Severe);
    Packet *pktt = new Packet(14, "II", PacketPriority::Severe);
    Packet *pkttt = new Packet(14, "III", PacketPriority::High);
    Packet *pktttt = new Packet(14, "IIII", PacketPriority::None);

    PacketQueue *queue = new PacketQueue();

    TEST_ASSERT_TRUE(!queue->queue[0]);
    TEST_ASSERT_TRUE(!queue->queue[2]);
    TEST_ASSERT_TRUE(!queue->queue[4]);

    queue->queue[0] = pkt;
    queue->queue[2] = pktt;
    queue->queue[4] = pkttt;
    queue->queue[10] = pktttt;

    TEST_ASSERT_TRUE(queue->queue[0]);
    TEST_ASSERT_TRUE(queue->queue[2]);
    TEST_ASSERT_TRUE(queue->queue[4]);

    queue->SortQueueByPriority();

    TEST_ASSERT_TRUE(!queue->queue[2]);
    TEST_ASSERT_TRUE(!queue->queue[4]);
    // AND
    TEST_ASSERT_TRUE(queue->queue[0]);
    TEST_ASSERT_TRUE(queue->queue[1]);
    TEST_ASSERT_TRUE(queue->queue[3]);
    TEST_ASSERT_TRUE(queue->queue[9]);
}

void setup()
{
    Serial.begin(115200);

    UNITY_BEGIN();

    /* RUN_TEST(test_packet_constructor);
    RUN_TEST(test_add_packet_payload);
    RUN_TEST(test_add_node_to_path);
    RUN_TEST(test_get_path_node);
    RUN_TEST(test_payload_class);
    RUN_TEST(test_packet_queue); */
    RUN_TEST(test_balance_queue);

    /*
    CustomClassBase c {1, 2.2, "3.3"};
    CustomClassBase cc;

    MsgPack::Packer packer;
    packer.serialize(c);

    MsgPack::Unpacker unpacker;
    unpacker.feed(packer.data(), packer.size());
    unpacker.deserialize(cc);

    if (c != cc) Serial.println("failed: custom base class");
 */

    UNITY_END();
}

void loop()
{
}