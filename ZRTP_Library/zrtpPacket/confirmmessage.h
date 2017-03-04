#ifndef CONFIRMMESSAGE_H
#define CONFIRMMESSAGE_H

#include "zrtpPacket.h"

#define CFB_INITIALIZATION_VECTOR_LENGTH 16

// Confirm packet lengt is 92, when signature is not suported.
#define CONFIRM_PACKET_LENGTH 92
#define ENCRYPTED_PART_LENGTH 40

class ConfirmMessage : public ZrtpPacket {

    uint8_t confirm_mac[MAC_LENGTH];

    uint8_t initializationVector[CFB_INITIALIZATION_VECTOR_LENGTH];

    uint8_t hashPreimageH0[HASH_LENGTH_SHA256];

    // Represents unused 15 bits set to 0 and signature
    // we do not use signature, so all 24 bits will be set to 0.
    uint8_t unusedAndSignature[3];

    // Represent E V A D flags.
    uint8_t flagocet;

    uint32_t cacheExpirationInterval;

    uint8_t encryptedPart[ENCRYPTED_PART_LENGTH];

    uint8_t dataToSend[CONFIRM_PACKET_LENGTH];

public:

    /**
     * @brief ConfirmMessage constructor of confirm message.
     */
    ConfirmMessage();

    /**
     * @brief ConfirmMessage destructor of confirm message.
     */
    ~ConfirmMessage();

    /**
     * @brief setConfirmMac setter for parameter confirm mac.
     * @param _confirmMac new value to set.
     */
    void setConfirmMac (uint8_t * _confirmMac) {memcpy(confirm_mac, _confirmMac, MAC_LENGTH);}


    /**
     * @brief setInitializationVector setter for initialization vector.
     * @param _initializationVector new value to set.
     */
    void setInitializationVector(uint8_t * _initializationVector) {memcpy(initializationVector, _initializationVector,
                                                                   CFB_INITIALIZATION_VECTOR_LENGTH);}

    /**
     * @brief setHashImageH0 setter for parameter hash image.
     * @param _hashPreimageH0 new value to set.
     */
    void setHashImageH0 (uint8_t* _hashPreimageH0)
        {memcpy(hashPreimageH0, _hashPreimageH0, HASH_LENGTH_SHA256);}

    /**
     * @brief setPbxEnrollmentFlag set PBX enrollment flag to 1.
     */
    void setPbxEnrollmentFlag() {flagocet ^= 8;}

    /**
     * @brief setSasVerifiedFlag set SAS verified flag to 1.
     */
    void setSasVerifiedFlag() {flagocet ^= 4;}

    /**
     * @brief setAllowClearFlag set allow clear flag to 1.
     */
    void setAllowClearFlag() {flagocet ^= 2;}

    /**
     * @brief setDisclosureFlag set disclosure flag to 1.
     */
    void setDisclosureFlag() {flagocet ^= 1;}

    /**
     * @brief initializeEncryptedPart copy correct values to encrypted part.
     */
    void initializeEncryptedPart();

    /**
     * @brief setEncryptedData setter for encrypted data also parse encrypted data and set correct values.
     *        copy new enrypted part to data to send.
     * @param _encryptedData new encrypted data.
     */
    void setEncryptedData(uint8_t* _encryptedData);

    /**
     * @brief setCacheExpirationInterval set cache expiration interval.
     * @param _expirationInterval
     */
    void setCacheExpirationInterval(uint32_t _expirationInterval) {cacheExpirationInterval = _expirationInterval;}

    /**
     * @brief getEncryptedPard getter for encryped part.
     * @return encrypted part.
     */
    uint8_t* getEncryptedPart() {return encryptedPart;}

    /**
     * @brief getConfirmData getter for confirm data.
     * @return confirmData.
     */
    uint8_t* getConfirmData() {return dataToSend;}

    /**
     * @brief getConfirmMac getter for confirm mac.
     * @return confirm_mac.
     */
    uint8_t* getConfirmMac() {return confirm_mac;}

    /**
     * @brief getCachceExpirationInterval getter for cache expiration interval.
     * @return
     */
    uint32_t getCachceExpirationInterval() {return cacheExpirationInterval;}

    /**
     * @brief getInitializationVector getter for initialization vector.
     * @return initialization vector.
     */
    uint8_t* getInitializationVector() {return initializationVector;}

    /**
     * @brief getHashPreimageH0 getter for hash preimage.
     * @return hashPreimage H0.
     */
    uint8_t* getHashPreimageH0() {return hashPreimageH0;}

    /**
     * @brief parseConfirmMessage parser for confirm message.
     * @param _messageToFill message to fill with parsed data.
     * @param _messageData data to parse.
     * @return return error code if something went wrong, 0 otherwise.
     */
    void parseConfirmMessage(ConfirmMessage* _messageToFill, uint8_t* _messageData);

    virtual void initializeMessageData();
};

#endif // CONFIRMMESSAGE_H
