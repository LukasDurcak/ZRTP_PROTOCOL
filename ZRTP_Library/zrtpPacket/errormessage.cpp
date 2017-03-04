#include "errormessage.h"

ErrorMessage::ErrorMessage(zrtpErrorCode _errorCode){

    setMessageType((uint8_t*) "Error   ");
    setErrorCode(_errorCode);
    setMessageLength(4);

    initializeMessageData();
}

ErrorMessage::~ErrorMessage(){

}

void ErrorMessage::initializeMessageData(){

    initializePacketData(dataToSend);
    initializeMessageHead(dataToSend);

    memcpy(dataToSend + 24, &currentErrorCode, WORD_LENGTH);
    memcpy(dataToSend + 28, &crc, sizeof(crc));
}

void ErrorMessage::setErrorCode (zrtpErrorCode _currentErrorCode) {

    currentErrorCode = _currentErrorCode;
    memcpy(dataToSend + 24, &currentErrorCode, WORD_LENGTH);
}
