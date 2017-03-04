#include "helloAckMessage.h"

HelloACKmessage::HelloACKmessage(){
    setMessageType((uint8_t *) "HelloACK");

    // Length of HelloAck message is always 3 words.
    setMessageLength(3);
    initializeMessageData();
}

HelloACKmessage::~HelloACKmessage(){

}

void HelloACKmessage::initializeMessageData(){

    initializePacketData(dataToSend);
    initializeMessageHead(dataToSend);

    memcpy(dataToSend + PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH, &crc, sizeof(crc));
}


