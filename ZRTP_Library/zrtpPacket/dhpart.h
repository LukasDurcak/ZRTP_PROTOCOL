#ifndef DHPART1_H
#define DHPART1_H

#include "zrtpPacket.h"

// rs1ID, rs2ID, auxSecretID, pbxSecret
#define SHARED_SECRET_LENGTH 8

// public keys lengths
#define DH3K_PUBLIC_KEY_LENGTH 384
#define DH2K_PUBLIC_KEY_LENGTH 256
#define EC25_PUBLIC_KEY_LENGTH 64
#define EC38_PUBLIC_KEY_LENGTH 96
#define EC52_PUBLIC_KEY_LENGTH 132

// Maximum size of DH_Packet, when RFC3526 is used.
#define DH3K_PACKET_SIZE 484

class DHPart : public ZrtpPacket {

    uint8_t hashImageH1 [HASH_LENGTH_SHA256];

    uint8_t rs1ID [SHARED_SECRET_LENGTH];
    uint8_t rs2ID [SHARED_SECRET_LENGTH];
    uint8_t auxSecretID [SHARED_SECRET_LENGTH];
    uint8_t pbxSecretID [SHARED_SECRET_LENGTH];
    uint8_t publicValue [DH3K_PUBLIC_KEY_LENGTH];
    uint8_t mac [MAC_LENGTH];

    uint32_t negotiatiedKeySize = 0;
    uint8_t dataToSend[DH3K_PACKET_SIZE];

public:

    /**
     * @brief DHPart constructor for dhpart 1 or 2 message.
     */
    DHPart();

    /**
     * @brief DHPart destructor for dhpart 1 or 2 message.
     */
    ~DHPart();

    /**
     * @brief getDHData is getter for parameter dataTosend
     * @return data to send for DHpart message
     */
    uint8_t* getDHData() {return dataToSend;}

    /**
     * @brief setHashImageH1 is setter for parameter hashImageH1.
     * @param _hashImageH1 new hash to set.
     */
    void setHashImageH1(uint8_t* _hashImageH1) {memcpy(hashImageH1,_hashImageH1, HASH_LENGTH_SHA256);}

    /**
     * @brief setRs1ID setter for rs1ID.
     * @param _rs1ID data to set.
     */
    void setRs1ID(uint8_t* _rs1ID) {memcpy(rs1ID, _rs1ID, SHARED_SECRET_LENGTH);}

    /**
     * @brief setRs2ID setter for rs2ID.
     * @param _rs2ID data to set.
     */
    void setRs2ID(uint8_t* _rs2ID) {memcpy(rs2ID, _rs2ID, SHARED_SECRET_LENGTH);}

    /**
     * @brief setAuxSecretID setter for auxSecretID.
     * @param _auxsecret new auxsecret to set.
     */
    void setAuxSecretID(uint8_t* _auxsecret) {memcpy(auxSecretID, _auxsecret, SHARED_SECRET_LENGTH);}

    /**
     * @brief setPbxSecretID setter for pbxSecretID.
     * @param _pbxsecret new pbx secret to set.
     */
    void setPbxSecretID(uint8_t* _pbx) {memcpy(pbxSecretID, _pbx, SHARED_SECRET_LENGTH);}

    /**
     * @brief setPublicValue setter fo public value.
     * @param _publicValue new value to set.
     */
    void setPublicValue(uint8_t* _publicValue)
        {memcpy(publicValue, _publicValue, getNegotiatedKeySize());}

    /**
     * @brief setMac setter for mac.
     * @param _mac new mac to set.
     */
    void setMac(uint8_t* _mac);

    /**
     * @brief getHashImageH1 getter for parameter hashImageH1.
     * @return hashImageH1.
     */
    uint8_t*getHashImageH1() {return hashImageH1;}

    /**
     * @brief getRs1ID getter for parameter rs1ID.
     * @return rs1ID.
     */
    uint8_t* getRs1ID() {return rs1ID;}

    /**
     * @brief getRs2ID getter for parameter rs1ID.
     * @return rs2ID.
     */
    uint8_t* getRs2ID() {return rs2ID;}

    /**
     * @brief getAuxSecret getter for parameter auxSecret.
     * @return auxSecret.
     */
    uint8_t* getAuxSecret(){return auxSecretID;}

    /**
     * @brief getPbxSecret getter for parameter auxSecret.
     * @return auxSecret
     */
    uint8_t* getPbxSecret(){return pbxSecretID;}

    /**
     * @brief getPublicValue getter for publicValue.
     * @return publicValue.
     */
    uint8_t* getPublicValue() {return publicValue;}

    /**
     * @brief getMac getter for parameter mac.
     * @return mac of dhPart message.
     */
    uint8_t* getMac() {return mac;}

    /**
     * @brief setNegotiatedKeySize is setter for key size.
     *        key size is set after hello and hello ack exchange, when the best common algorithm is chosen.
     * @param _keySize size of key.
     */
    void setNegotiatedKeySize(uint32_t _keySize) {negotiatiedKeySize = _keySize;}

    /**
     * @brief getNegotiatedKeySize getter for key size.
     */
    uint32_t getNegotiatedKeySize() {return negotiatiedKeySize;}

    /**
     * @brief parseDhMessage parser for Dh message.
     * @param _messageToFill message to fill with parsed data.
     * @param messageData data to parse.
     * @return error code or 0 if succesfull
     */
    void parseDhMessage(DHPart* _messageToFill, uint8_t* _messageData);

    virtual void initializeMessageData();
};

#endif // DHPART1_H
