#include "conf2ackmessage.h"

Conf2AckMessage::Conf2AckMessage(){
    setMessageType((uint8_t *) "Conf2ACK");
    setMessageLength(3);

    initializeMessageData();
}

Conf2AckMessage::~Conf2AckMessage(){

}

void Conf2AckMessage::initializeMessageData(){

    // Copy packet head
    initializePacketData(dataToSend);

    // Copy message head
    initializeMessageHead(dataToSend);

    memcpy(dataToSend + PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH, &crc, sizeof(crc));
}
