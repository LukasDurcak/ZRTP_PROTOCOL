#include "confirmmessage.h"

ConfirmMessage::ConfirmMessage() {
    // Confirm length is 19 when signature is not suported.
    setMessageLength(19);

    setCacheExpirationInterval(0);

    memset(unusedAndSignature, 0, sizeof(unusedAndSignature));
    flagocet = 0;
}

ConfirmMessage::~ConfirmMessage(){

}

void ConfirmMessage::initializeEncryptedPart(){

    uint16_t p = 0;

    // Hash image
    memcpy(encryptedPart, hashPreimageH0, HASH_LENGTH_SHA256);
    p += HASH_LENGTH_SHA256;

    // Unused bytes
    memcpy(encryptedPart + p, unusedAndSignature, sizeof(unusedAndSignature));
    p += sizeof(unusedAndSignature);

    // Flagocet
    memcpy(encryptedPart + p, &flagocet, sizeof(flagocet));
    p += sizeof(flagocet);

    // Cache expiration interval
    memcpy(encryptedPart + p, &cacheExpirationInterval, WORD_LENGTH);
}

void ConfirmMessage::initializeMessageData(){

    // Copy packet data at the beginning.
    initializePacketData(dataToSend);

    // Copy hello data from hello head to sending data.
    initializeMessageHead(dataToSend);
    uint16_t p = PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH;

    // Confirm mac.
    memcpy(dataToSend + p, confirm_mac, MAC_LENGTH);
    p += MAC_LENGTH;

    // CFB initialization vector.
    memcpy(dataToSend + p, initializationVector, CFB_INITIALIZATION_VECTOR_LENGTH);
    p += CFB_INITIALIZATION_VECTOR_LENGTH;

    // Encrypted part.
    memcpy(dataToSend + p, encryptedPart, ENCRYPTED_PART_LENGTH);
    p += ENCRYPTED_PART_LENGTH;

    // Crc
    memcpy(dataToSend + p, &crc, sizeof(crc));
}

void ConfirmMessage::parseConfirmMessage(ConfirmMessage *_messageToFill, uint8_t *_messageData){

    // Reverse procedure as in initialize()
    uint16_t p = PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH;
    _messageToFill->setConfirmMac(_messageData + p);
    p += MAC_LENGTH;

    _messageToFill->setInitializationVector(_messageData + p);
    p += CFB_INITIALIZATION_VECTOR_LENGTH;

    _messageToFill->setEncryptedData(_messageData + p);
    p += ENCRYPTED_PART_LENGTH;

    _messageToFill->initializeMessageData();
}

void ConfirmMessage::setEncryptedData(uint8_t *_encryptedData){
    // Store encrypted part
    memcpy(encryptedPart, _encryptedData, ENCRYPTED_PART_LENGTH);

    // Copy encrypted part to data to send.
    memcpy(dataToSend + PACKET_HEAD_LENGTH + MESSAGE_HEAD_LENGTH + MAC_LENGTH + CFB_INITIALIZATION_VECTOR_LENGTH,
           encryptedPart, ENCRYPTED_PART_LENGTH);

    // Set H0 from encrypted part
    setHashImageH0(_encryptedData);

    memset(unusedAndSignature, 0, sizeof(unusedAndSignature));

    // Set flagocet
    flagocet = *(uint8_t*)(_encryptedData + 35);
}

