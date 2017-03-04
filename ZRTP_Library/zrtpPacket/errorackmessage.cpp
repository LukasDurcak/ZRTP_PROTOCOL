#include "errorackmessage.h"

ErrorAckMessage::ErrorAckMessage(){

    setMessageType((uint8_t *) "ErrorACK");
    setMessageLength(3);

    initializeMessageData();
}

ErrorAckMessage::~ErrorAckMessage(){

}

void ErrorAckMessage::initializeMessageData(){

    initializePacketData(dataToSend);
    initializeMessageHead(dataToSend);
    memcpy(dataToSend + 24, &crc, sizeof(crc));
}
