#include "zrtpPacket/hellomessage.h"
#include <bitset>

HelloMessage::HelloMessage(){
    setMessageType((uint8_t *) "Hello   ");
    setClientID((uint8_t*) "DURCAK______2015");

    currentCounts.ac = 0;
    currentCounts.cc = 0;
    currentCounts.hc = 0;
    currentCounts.kc = 0;
    currentCounts.sc = 0;

    flagsAndCount = 0; 
}

HelloMessage::~HelloMessage(){

}

void HelloMessage::initializeMessageData(){

    uint16_t tempLength = 0;

    // Copy packet data at the beginning.
    initializePacketData(dataToSend);

    initFlagsAndCounts();

    // Copy the remaining data to dataTosend
    tempLength += (PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH);

    // Protocol version
    memcpy(dataToSend + tempLength, protocolVersion, sizeof(protocolVersion));
    tempLength += sizeof(protocolVersion);

    // client identifier
    memcpy(dataToSend + tempLength, clientId, CLIENT_IDENTIFIER_LENGTH);
    tempLength += CLIENT_IDENTIFIER_LENGTH;

    // hash image H3
    memcpy(dataToSend + tempLength, hashImageH3, HASH_LENGTH_SHA256);
    tempLength += HASH_LENGTH_SHA256;

    // zid
    memcpy(dataToSend + tempLength, zid, ZID_LENGTH);
    tempLength += ZID_LENGTH;

    // flags and counts.
    memcpy(dataToSend + tempLength, &flagsAndCount, sizeof(flagsAndCount));
    tempLength += sizeof(flagsAndCount);

    // Copy supported algorithms lists.
    for (int i = 0; i < (WORD_LENGTH * currentCounts.hc); i++){
        dataToSend[tempLength] = * (hashAlgorithms + i);
        tempLength++;
    }

    for (int i = 0; i < (WORD_LENGTH * currentCounts.cc); i++){
        dataToSend[tempLength] = * (cipherAlgorithms + i);
        tempLength++;
    }

    for (int i = 0; i < (WORD_LENGTH * currentCounts.ac); i++){
        dataToSend[tempLength] = * (authTagAlgorithms + i);
        tempLength++;
    }

    for (int i = 0; i < (WORD_LENGTH * currentCounts.kc); i++){
        dataToSend[tempLength] = * (keyAgreementTypes + i);
        tempLength++;
    }

    for (int i = 0; i < (WORD_LENGTH * currentCounts.sc); i++){
        dataToSend[tempLength] = * (sasTypes + i);
        tempLength++;
    }

    tempLength += sizeof(mac);
    memcpy(dataToSend + tempLength, &crc, sizeof(crc));
    tempLength += sizeof(crc);

    setMessageLength((tempLength - PACKET_WITHOUT_MESSAGE_LENGTH) / WORD_LENGTH);

    // Copy message header to sending data after length was calculated.
    initializeMessageHead(dataToSend);
}

void HelloMessage::addHashAlgorithm(uint8_t *_hashAlgorithm){
    memcpy(hashAlgorithms + (currentCounts.hc * WORD_LENGTH), _hashAlgorithm,
           WORD_LENGTH);

    currentCounts.hc++;
}

void HelloMessage::addCipherAlgorithm(uint8_t *_cipherAlgorithm){
    memcpy(cipherAlgorithms + (currentCounts.cc * WORD_LENGTH), _cipherAlgorithm,
           WORD_LENGTH);

    currentCounts.cc++;
}

void HelloMessage::addAuthTagType(uint8_t *_authTagType){
    memcpy(authTagAlgorithms + (currentCounts.ac * WORD_LENGTH), _authTagType,
           WORD_LENGTH);

    currentCounts.ac++;
}

void HelloMessage::addKeyAgreementType(uint8_t *_keyAgreementType){
    memcpy(keyAgreementTypes + (currentCounts.kc) * WORD_LENGTH, _keyAgreementType,
           WORD_LENGTH);

    currentCounts.kc++;
}

void HelloMessage::addSasType(uint8_t *_sasType){
    memcpy(sasTypes + (currentCounts.sc) * WORD_LENGTH, _sasType,
           WORD_LENGTH);

    currentCounts.sc++;
}

void HelloMessage::initFlagsAndCounts(){
    uint32_t temp;

    memcpy(&temp, &currentCounts, 4);

    temp <<= 12;
    temp >>= 12;

    flagsAndCount |= temp;
}

void HelloMessage::setMac(uint8_t *_mac){
    memcpy(mac, _mac, MAC_LENGTH);

    // Copy mac to dataTo send (WORD_LENGTH is crc length).
    memcpy(dataToSend + (getWholePacketLength() - WORD_LENGTH - MAC_LENGTH), _mac, MAC_LENGTH);
}

void HelloMessage::setProtocolVersion(uint8_t *_protocolVersion){
     memcpy(protocolVersion, _protocolVersion, PROTOCOL_VERSION_LENGTH);
     memcpy(dataToSend + PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH, _protocolVersion, PROTOCOL_VERSION_LENGTH);
}

zrtpErrorCode HelloMessage::parseHelloMessage(HelloMessage *_messageToFill, uint8_t *_messageData){

    zrtpErrorCode tempErrorCode = N_ERROR;

    uint16_t tempLength;
    uint32_t tempFlagsAndCounts;
    uint16_t p = 0;

    // Copy length, we skip packet head and 2Bytes - preamble.
    tempLength = *(uint16_t * )( _messageData + PACKET_HEAD_LENGTH + 2);
    _messageToFill->setMessageLength(tempLength);
    p += PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH;

    // Set and check protocol version
    _messageToFill->setProtocolVersion((_messageData + p));
    p += WORD_LENGTH;
    if ((tempErrorCode = checkProtocolVersion()) != N_ERROR){
        return tempErrorCode;
    }

    // Client ID.
    _messageToFill->setClientID(_messageData + p);
    p += CLIENT_IDENTIFIER_LENGTH;

    // Hash image H3.
    _messageToFill->setHashImageH3(_messageData + p);
    p += HASH_LENGTH_SHA256;

    // ZID,  check Zids if are equal = error code
    _messageToFill->setZID(_messageData + p);
    if(memcmp(this->getZID(), _messageToFill->getZID(), ZID_LENGTH) == 0){
        tempErrorCode = EQUALS_ZID_IN_HELLO;
        return tempErrorCode;
    }
    p += ZID_LENGTH;

    // Flags and count
    tempFlagsAndCounts = *(uint32_t*) (_messageData + p);
    setFlagsAndcounts(tempFlagsAndCounts);

    currentCounts.sc = (tempFlagsAndCounts & 0xf);
    tempFlagsAndCounts >>= 4;
    currentCounts.kc = (tempFlagsAndCounts & 0xf);
    tempFlagsAndCounts >>= 4;
    currentCounts.ac = (tempFlagsAndCounts & 0xf);
    tempFlagsAndCounts >>= 4;
    currentCounts.cc = (tempFlagsAndCounts & 0xf);
    tempFlagsAndCounts >>= 4;
    currentCounts.hc = (tempFlagsAndCounts & 0xf);
    tempFlagsAndCounts >>= 4;

    p += WORD_LENGTH;

    // Copy lists of algorithms.
    for (int i = 0; i < currentCounts.hc ; i++){
       _messageToFill->addHashAlgorithm(_messageData + p);
       p += WORD_LENGTH;
    }

    for (int i = 0; i < currentCounts.cc ; i++){
       _messageToFill->addCipherAlgorithm(_messageData + p);
       p += WORD_LENGTH;
    }

    for (int i = 0; i < currentCounts.ac ; i++){
       _messageToFill->addAuthTagType(_messageData + p);
       p += WORD_LENGTH;
    }

    for (int i = 0; i < currentCounts.kc ; i++){
       _messageToFill->addKeyAgreementType(_messageData + p);
       p += WORD_LENGTH;
    }

    for (int i = 0; i < currentCounts.sc ; i++){
       _messageToFill->addSasType(_messageData + p);
       p += WORD_LENGTH;
    }

    _messageToFill->initializeMessageData();
    _messageToFill->setMac(_messageData + p);

    return tempErrorCode;
}

zrtpErrorCode HelloMessage::checkProtocolVersion(){
    zrtpErrorCode tempError = N_ERROR;
    uint8_t* minVersion = new uint8_t[4];
    memcpy(minVersion, (const char*) "1.10", 4);

    if (memcmp(protocolVersion, minVersion, 4) < 0){
        return tempError = UNSUPORTED_ZRTP_VERSION;
    }

    delete [] minVersion;
    return tempError;
}
