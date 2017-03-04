#ifndef COMMITMESSAGE_H
#define COMMITMESSAGE_H

#include "zrtpPacket.h"

#define HVI_LENGTH 32
#define DH_COMMIT_PACKET_LENGTH 132

/**
 * @brief The CommitMessage class represent Commint message for DH mode.
 */
class CommitMessage : public ZrtpPacket {

    // hashImage = hash(initiator’s DHPart2 message || responder’s Hello message)
    uint8_t hashImageH2 [HASH_LENGTH_SHA256];
    uint8_t zid         [ZID_LENGTH];
    uint8_t hvi         [HVI_LENGTH];
    uint8_t mac         [MAC_LENGTH];

    uint8_t agreedHashAlgorithm [WORD_LENGTH];     // = "SHA-256 Hash";
    uint8_t agreedCipherAlgorithm [WORD_LENGTH];   // = "AES-CM with 128 bit Keys";
    uint8_t agreedAuthTagAlgorithm [WORD_LENGTH] ; // = "HMAC-SHA1 32 bit authentication tag";
    uint8_t agreedKeyAgreementType [WORD_LENGTH];  // = "DH mode with p=3072 prime";
    uint8_t agreedSasType [WORD_LENGTH] ;          // = "Short authentication string using base 32";

    uint8_t dataToSend[DH_COMMIT_PACKET_LENGTH];

public:

    /**
     * @brief CommitMessage constructor for commit message.
     */
    CommitMessage();

    /**
     * @brief ~CommitMessage destructor for commit message.
     */
    ~CommitMessage();

    /**
     * @brief getCommitData getter for commit data.
     * @return commit data.
     */
    uint8_t * getCommitData(){return dataToSend;}

    /**
     * @brief setAgreedHashAlgorithm setter for aggreedHashAlgorithm.
     * @param _agreedHashAlgorithm new algorithm to set.
     */
    void setAgreedHashAlgorithm (uint8_t* _agreedHashAlgorithm)
        {memcpy(agreedHashAlgorithm, _agreedHashAlgorithm, WORD_LENGTH);}

    /**
     * @brief setAgreedCipherAlgorithm setter for agreedCipherAlgorithm.
     * @param _agreedCipherAlgorithm new algorithm to set.
     */
    void setAgreedCipherAlgorithm (uint8_t* _agreedCipherAlgorithm)
        {memcpy(agreedCipherAlgorithm, _agreedCipherAlgorithm, WORD_LENGTH);}

    /**
     * @brief setAgreedAuthTagAlgorithm setter for agreedAuthTagAlgorithm.
     * @param _agreedAuthTagAlgorithm new algorithm to set.
     */
    void setAgreedAuthTagAlgorithm (uint8_t* _agreedAuthTagAlgorithm)
        {memcpy(agreedAuthTagAlgorithm, _agreedAuthTagAlgorithm, WORD_LENGTH);}

    /**
     * @brief setAgreedKeyAgreementType setter for agreedKeyAgreementType.
     * @param _agreedKeyAgreementType new key agreement type to set.
     */
    void setAgreedKeyAgreementType (uint8_t* _agreedKeyAgreementType)
        {memcpy(agreedKeyAgreementType, _agreedKeyAgreementType, WORD_LENGTH);}

    /**
     * @brief setAgreedSasType setter for SAS type.
     * @param _agreedSasType new agreedSasType.
     */
    void setAgreedSasType (uint8_t* _agreedSasType)
        {memcpy(agreedSasType, _agreedSasType, WORD_LENGTH);}

    /**
     * @brief setHashImage setter for hash image.
     * @param _hashImage new hash image to set.
     */
    void setHashImageH2(uint8_t* _hashImage) {memcpy(hashImageH2, _hashImage, HASH_LENGTH_SHA256);}

    /**
     * @brief setZid setter for zid.
     * @param _zid new zid to set.
     */
    void setZid(uint8_t* _zid) {memcpy(zid, _zid, ZID_LENGTH);}

    /**
     * @brief setHvi setter for Hvi.
     * @param _hvi new hvi to set.
     */
    void setHvi(uint8_t* _hvi) {memcpy(hvi, _hvi, HVI_LENGTH);}

    /**
     * @brief setMac setter for Mac.
     * @param _mac new mac to set.
     */
    void setMac(uint8_t* _mac);

    /**
     * @brief getHashImage getter for hash image.
     * @return hash image.
     */
    uint8_t* getHashImageH2() {return hashImageH2;}

    /**
     * @brief getZid getter for zid.
     * @return zid.
     */
    uint8_t* getZid() {return zid;}

    /**
     * @brief getHvi getter for Hvi.
     * @return hvi.
     */
    uint8_t* getHvi() {return hvi;}

    /**
     * @brief getMac getter for mac.
     * @return mac.
     */
    uint8_t* getMac() {return mac;}

    /**
     * @brief getAgreedHashAlgorithm getter for hash algorithm.
     * @return agreedHashAlgorithm.
     */
    uint8_t* getAgreedHashAlgorithm() {return agreedHashAlgorithm;}

    /**
     * @brief getAgreedCipherAlgorithm getter for cipher algorithm.
     * @return agreedCipherAlgorithm
     */
    uint8_t* getAgreedCipherAlgorithm() {return agreedCipherAlgorithm;}

    /**
     * @brief getAgreedAuthTagAlgorithm getter for auth tag algorithm.
     * @return agreedAuthTagAlgorithm.
     */
    uint8_t* getAgreedAuthTagAlgorithm() {return agreedAuthTagAlgorithm;}

    /**
     * @brief getAgreedKeyAgreementType getter for key agreement type,
     * @return agreedKeyAgreementType.
     */
    uint8_t* getAgreedKeyAgreementType() {return agreedKeyAgreementType;}

    /**
     * @brief getAgreedSasType getter for sas type.
     * @return agreedSasType.
     */
    uint8_t* getAgreedSasType() {return agreedSasType;}

    /**
     * @brief parseCommitMessage parser for received commit message.
     * @param _messageToFill message to fill with received data.
     * @param _messageData received data.
     * @return error code if error occured, 0 otherwise.
     */
    void parseCommitMessage(CommitMessage* _messageToFill, uint8_t* _messageData);

    virtual void initializeMessageData();
};

#endif // COMMITMESSAGE_H
