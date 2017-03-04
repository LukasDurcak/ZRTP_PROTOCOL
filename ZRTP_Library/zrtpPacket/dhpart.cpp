#include "dhpart.h"

DHPart::DHPart(){

    // Default
    setNegotiatedKeySize(DH3K_PUBLIC_KEY_LENGTH);
    // Length is 117 when DH_3072 is our key size
    setMessageLength(117);

    memset(rs1ID, 0, SHARED_SECRET_LENGTH);
    memset(rs2ID, 0, SHARED_SECRET_LENGTH);
    memset(pbxSecretID, 0, SHARED_SECRET_LENGTH);
    memset(auxSecretID, 0, SHARED_SECRET_LENGTH);

    memset(mac, 0, MAC_LENGTH);
}

DHPart::~DHPart(){

}

void DHPart::initializeMessageData(){

    // Copy packet data at the beginning.
    initializePacketData(dataToSend);

    // Copy dhpart data from dhPart head to sending data.
    initializeMessageHead(dataToSend);
    uint16_t p = PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH;

    // Hash image H1.
    memcpy(dataToSend + p, hashImageH1, HASH_LENGTH_SHA256);
    p += HASH_LENGTH_SHA256;

    // Shared secrets rs1, rs2, aux, pbx
    memcpy(dataToSend + p, rs1ID, SHARED_SECRET_LENGTH);
    p += SHARED_SECRET_LENGTH;
    memcpy(dataToSend + p, rs2ID, SHARED_SECRET_LENGTH);
    p += SHARED_SECRET_LENGTH;
    memcpy(dataToSend + p, auxSecretID, SHARED_SECRET_LENGTH);
    p += SHARED_SECRET_LENGTH;
    memcpy(dataToSend + 80, pbxSecretID, SHARED_SECRET_LENGTH);
    p += SHARED_SECRET_LENGTH;

    // Public Value
    memcpy(dataToSend + p, publicValue, getNegotiatedKeySize());
    p += getNegotiatedKeySize();

    // Mac.
    memcpy(dataToSend + p, mac, MAC_LENGTH);
    p += MAC_LENGTH;

    memcpy(dataToSend + p, &crc, sizeof(crc));
}

void DHPart::parseDhMessage(DHPart *_messageToFill, uint8_t *_messageData){

    // Reverse procedure as in initialize.
    uint16_t p = PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH;
    _messageToFill->setHashImageH1(_messageData + p);
    p += HASH_LENGTH_SHA256;

    _messageToFill->setRs1ID(_messageData + p);
    p += SHARED_SECRET_LENGTH;
    _messageToFill->setRs2ID(_messageData + p);
    p += SHARED_SECRET_LENGTH;
    _messageToFill->setAuxSecretID(_messageData + p);
    p += SHARED_SECRET_LENGTH;
    _messageToFill->setPbxSecretID(_messageData + p);
    p += SHARED_SECRET_LENGTH;

    _messageToFill->setPublicValue(_messageData + p);
    p += getNegotiatedKeySize();

    _messageToFill->setMac(_messageData + p);
    p += MAC_LENGTH;

    _messageToFill->initializeMessageData();
}

void DHPart::setMac(uint8_t *_mac){
    memcpy(mac, _mac, MAC_LENGTH);

    // Copy mac to dataTo send (WORD_LENGTH is crc length).
    memcpy(dataToSend + (getWholePacketLength() - WORD_LENGTH - MAC_LENGTH), mac, MAC_LENGTH);
}


