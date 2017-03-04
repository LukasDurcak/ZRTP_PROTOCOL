#ifndef HELLOMESSAGE_H
#define HELLOMESSAGE_H

#include "zrtpPacket.h"

#define PROTOCOL_VERSION_LENGTH 4
#define CLIENT_IDENTIFIER_LENGTH 16

// Maximum size for hello message
#define HELLO_PACKET_SIZE 244

// Maximum value for hc, cc, ac, kc, sc
#define MAXIMUM_COUNT_OF_ALGORITHMS 7

/*
 This structure represent algorithm counts in hello message,
 which all have 4bit size.
 */
struct counts{
    unsigned int sc : 4; // sas count
    unsigned int kc : 4; // key agreement count
    unsigned int ac : 4; // auth tag count
    unsigned int cc : 4; // cipher count
    unsigned int hc : 4; // hash count
};

/**
 * @brief The HelloMessage class represent Hello message.
 */
class HelloMessage : public ZrtpPacket{

    uint8_t protocolVersion[PROTOCOL_VERSION_LENGTH];
    uint8_t clientId[CLIENT_IDENTIFIER_LENGTH];
    uint8_t hashImageH3[HASH_LENGTH_SHA256];
    uint8_t zid[ZID_LENGTH];

    counts currentCounts;

    // Represent one word which hold flags and count
    uint32_t flagsAndCount;

    uint8_t hashAlgorithms    [MAXIMUM_COUNT_OF_ALGORITHMS * WORD_LENGTH]; // = "SHA-256 Hash";
    uint8_t cipherAlgorithms  [MAXIMUM_COUNT_OF_ALGORITHMS * WORD_LENGTH]; // = "AES-CM with 128 bit Keys";
    uint8_t authTagAlgorithms [MAXIMUM_COUNT_OF_ALGORITHMS * WORD_LENGTH]; // = "HMAC-SHA1 32 bit authentication tag";
    uint8_t keyAgreementTypes [MAXIMUM_COUNT_OF_ALGORITHMS * WORD_LENGTH]; // = "DH mode with p=3072 prime";
    uint8_t sasTypes          [MAXIMUM_COUNT_OF_ALGORITHMS * WORD_LENGTH]; // = "Short authentication string using base 32";

    uint8_t mac[MAC_LENGTH];

    uint8_t dataToSend[HELLO_PACKET_SIZE];

public:

    /**
     * @brief HelloMessage constructor for Hello message class
     */
    HelloMessage();

    /**
     * @brief ~HelloMessage destructor for Hello message class.
     */
    ~HelloMessage();

    /**
     * @brief setProtocolVersion setter for protocol version. This function also copy new value to dataToSend.
     * @param _protocolVersion new protocol version
     */
    void setProtocolVersion(uint8_t* _protocolVersion);

    /**
     * @brief setClientID setter for client Identifier.
     * @param _clientID new clientID.
     */
    void setClientID (uint8_t* _clientID)
        {memcpy(clientId, _clientID, CLIENT_IDENTIFIER_LENGTH);}

    /**
     * @brief setHashImage setter for parameter hashImage.
     * @param _hashImage new hash image.
     */
    void setHashImageH3(uint8_t* _hashImage)
        {memcpy(hashImageH3, _hashImage, HASH_LENGTH_SHA256);}

    /**
     * @brief setZID setter for user´s ZID.
     * @param _zid to set.
     */
    void setZID(uint8_t* _zid) {memcpy(zid, _zid, ZID_LENGTH);}

    /**
     * @brief setMac is setter for parameter Mac. This function also copy new value to dataToSend.
     * @param _mac is mac to be set.
     */
    void setMac(uint8_t* _mac);

    /**
     * @brief getMac getter for hello mac.
     * @return mac.
     */
    uint8_t* getMac() {return mac;}

    /**
     * @brief setFlagsAndcounts setter for parameter flags and counts.
     * @param _flagsAndCounts to set.
     */
    void setFlagsAndcounts(uint32_t _flagsAndCounts) {flagsAndCount = _flagsAndCounts;}

    /**
     * @brief addHashAlgorithm add new hash algorithm to list.
     * @param _hashAlgorithm to add.
     */
    void addHashAlgorithm(uint8_t* _hashAlgorithm);

    /**
     * @brief addCipherAlgorithm add new cipher algorithm to list.
     * @param _cipherAlgorithm to add.
     */
    void addCipherAlgorithm(uint8_t* _cipherAlgorithm);

    /**
     * @brief addAuthTagType add new auth type to list.
     * @param _authTagType to add
     */
    void addAuthTagType(uint8_t* _authTagType);

    /**
     * @brief addKeyAgreementType add new key agreement type to list.
     * @param _keyAgreementType to add.
     */
    void addKeyAgreementType(uint8_t* _keyAgreementType);

    /**
     * @brief addSasType add new SAS type to list.
     * @param _sasType to add.
     */
    void addSasType(uint8_t* _sasType);

    /**
     * @brief setSignatureFlaf sets Signature-capable flag (S).
     */
    void setSignatureFlaf() {flagsAndCount ^= (1 << 30);}

    /**
     * @brief setMitmFlag sets MiTM flag (M).
     */
    void setMitmFlag() {flagsAndCount ^= (1 << 29);}

    /**
     * @brief setPassiveFlag ets Passive flag (P).
     */
    void setPassiveFlag() {flagsAndCount ^= (1 << 28);}

    /**
     * @brief getProtocolVersion getter for protocol version.
     * @return protocol version.
     */
    uint8_t * getProtocolVersion () {return protocolVersion;}

    /**
     * @brief getClientIdentifier getter for client ID.
     * @return  client identifier.
     */
    uint8_t * getClientIdentifier() {return clientId;}

    /**
     * @brief getHashImageH3 getter for hashImage H3.
     * @return hashImageH3.
     */
    uint8_t * getHashImageH3() {return hashImageH3;}

    /**
     * @brief getZID getter for parameter Zid.
     * @return zid.
     */
    uint8_t * getZID() {return zid;}

    /**
     * @brief getHelloData getter for hello data.
     * @return data to send.
     */
    uint8_t* getHelloData() {return dataToSend;}

    /**
     * @brief getHashAlgorithms getter for hash algorithms.
     * @return hash algorithms.
     */
    uint8_t* getHashAlgorithms(){return hashAlgorithms;}

    /**
     * @brief getCipherAlgorithms getter for cipher algorithms.
     * @return cipher algorithms.
     */
    uint8_t* getCipherAlgorithms(){return cipherAlgorithms;}

    /**
     * @brief getAuthTagTypes getter for auth tagTypes.
     * @return auth tag types.
     */
    uint8_t* getAuthTagTypes() {return authTagAlgorithms;}

    /**
     * @brief getKeyAgreementTypes getter for key agreement types.
     * @return key agreement types.
     */
    uint8_t* getKeyAgreementTypes() {return keyAgreementTypes;}

    /**
     * @brief getSasTypes getter for sas types.
     * @return sas types.
     */
    uint8_t* getSasTypes() {return sasTypes;}

    /**
     * @brief initFlagsAndCounts copy flags and current counts to one variable, so they
     *        will have one word size.
     */
    void initFlagsAndCounts();

    /**
     * @brief parseHelloMessage parse and check received hello message and fill new message with correct data.
     * @param _messageToFill message to fill.
     * @param _messageData received data.
     * @param length of received message
     * @return EQUAL_ZID, MALFORMED_PACKET or NOT SUPORTET VERSION error if occured or N_ERROR if parse is OK.
     */
    zrtpErrorCode parseHelloMessage(HelloMessage* _messageToFill, uint8_t* _messageData);

    /**
     * @brief checkProtocolVersion check if version is not less than 1.10.
     * @return Version not supported or No_error.
     */
    zrtpErrorCode checkProtocolVersion();

    /**
     * @brief getHelloCounts getter for helloCounts.
     * @return structure with algorithm counts stored in hello message.
     */
    counts getHelloCounts() {return currentCounts;}

    virtual void initializeMessageData();
};

#endif // HELLOMESSAGE_H
