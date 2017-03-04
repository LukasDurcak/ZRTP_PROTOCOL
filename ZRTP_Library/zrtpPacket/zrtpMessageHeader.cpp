#include "zrtpPacket/zrtpMessageHeader.h"

void ZrtpMessageHeader::initializeMessageHead(uint8_t *_data) {

    memcpy(_data + PACKET_HEAD_LENGTH, &preamble, sizeof(preamble));
    memcpy(_data + PACKET_HEAD_LENGTH + sizeof(preamble), &messageLength, sizeof(messageLength));
    memcpy(_data + PACKET_HEAD_LENGTH + sizeof(preamble) + sizeof(messageLength), messageType, sizeof(messageType));
}

ZrtpMessageHeader::ZrtpMessageHeader(){

}

ZrtpMessageHeader::~ZrtpMessageHeader(){

}
