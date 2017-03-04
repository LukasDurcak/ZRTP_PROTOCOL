#include "CommitMessage.h"

CommitMessage::CommitMessage(){
    setMessageType((uint8_t *) "Commit  ");

    // Length of DH commit message is always 29 words.
    setMessageLength(29);

    // Caused syscalle error if no set to 0.
    memset(mac, 0, MAC_LENGTH);
}

CommitMessage::~CommitMessage(){

}

void CommitMessage::initializeMessageData(){

    // Copy packet data at the beginning.
    initializePacketData(dataToSend);

    // Copy commit data from commit head to sending data.
    initializeMessageHead(dataToSend);

    // We start copy after packet head and message head.
    uint16_t p = PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH;

    // hash image.
    memcpy(dataToSend + p, hashImageH2, HASH_LENGTH_SHA256);
    p += HASH_LENGTH_SHA256;

    // zid
    memcpy(dataToSend + p, zid, ZID_LENGTH);
    p += ZID_LENGTH;

    // agreed agorithms
    memcpy(dataToSend + p, agreedHashAlgorithm, WORD_LENGTH);
    p += WORD_LENGTH; 
    memcpy(dataToSend + p, agreedCipherAlgorithm, WORD_LENGTH);
    p += WORD_LENGTH;
    memcpy(dataToSend + p, agreedAuthTagAlgorithm, WORD_LENGTH);
    p += WORD_LENGTH;
    memcpy(dataToSend + p, agreedKeyAgreementType, WORD_LENGTH);
    p += WORD_LENGTH;
    memcpy(dataToSend + p, agreedSasType, WORD_LENGTH);
    p += WORD_LENGTH;

    // hvi
    memcpy(dataToSend + p,  hvi, HVI_LENGTH);
    p += HVI_LENGTH;

    // mac
    memcpy(dataToSend + p, mac, MAC_LENGTH);
    p += MAC_LENGTH;

    memcpy(dataToSend + 128, &crc, sizeof(crc));
}

void CommitMessage::parseCommitMessage(CommitMessage *_messageToFill, uint8_t *_messageData){

    // We do reverse procedure as in initialize data.
    uint16_t p = PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH;

    _messageToFill->setHashImageH2(_messageData + p);
    p += HASH_LENGTH_SHA256;

    _messageToFill->setZid(_messageData + p);
    p += ZID_LENGTH;

    _messageToFill->setAgreedHashAlgorithm(_messageData + p);
    p += WORD_LENGTH;
    _messageToFill->setAgreedCipherAlgorithm(_messageData + p);
    p += WORD_LENGTH;
    _messageToFill->setAgreedAuthTagAlgorithm(_messageData + p);
    p += WORD_LENGTH;
    _messageToFill->setAgreedKeyAgreementType(_messageData + p);
    p += WORD_LENGTH;
    _messageToFill->setAgreedSasType(_messageData + p);
    p += WORD_LENGTH;

    _messageToFill->setHvi(_messageData + p);
    p += HVI_LENGTH;

    _messageToFill->setMac(_messageData + p);

    _messageToFill->initializeMessageData();
}

void CommitMessage::setMac(uint8_t *_mac){

    memcpy(mac, _mac, MAC_LENGTH);

    // Copy mac to dataTo send (WORD_LENGTH is crc length).
    memcpy(dataToSend + (getWholePacketLength() - WORD_LENGTH - MAC_LENGTH), _mac, MAC_LENGTH);
}

