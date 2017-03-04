#include "zrtpPacket/zrtpPacket.h"

ZrtpPacket::ZrtpPacket(){
    messageHeader = new ZrtpMessageHeader();

    srand (time(NULL));
    sequenceNumber = rand() % USHRT_MAX;
    sourceId = rand () % ULONG_MAX;

    memcpy(magicCookie, (const char*) "ZRTP", sizeof(magicCookie));
    crc = 1111;
}

ZrtpPacket::~ZrtpPacket(){
    delete messageHeader;
}

void ZrtpPacket::initializePacketData(uint8_t *_data){

    uint16_t p = 0;

    // PacketHead
    memcpy(_data,  &packetHead, sizeof(packetHead));
    p += sizeof(packetHead);

    // Sequence number
    memcpy(_data + p, &sequenceNumber, sizeof(sequenceNumber));
    p += sizeof(sequenceNumber);

    // Magic cookie
    memcpy(_data + p, &magicCookie, sizeof(magicCookie));
    p += sizeof(magicCookie);

    // Source ID
    memcpy(_data + p , &sourceId, sizeof(sourceId));
}

