#ifndef ZRTPOINT_H
#define ZRTPOINT_H

#include "zrtpPacket/messages.h"
#include "callbacks.h"
#include "statemachine.h"
#include "events.h"
#include "userInfo.h"
#include <fstream>
#include <iostream>
#include <assert.h>
#include <inttypes.h>

#include "sha256.h"
#include "entropy.h"
#include "ctr_drbg.h"
#include "dhm.h"
#include "aes.h"

#define CACHED_SECRET_LENGTH 32 // s0, s1, s2, s3
#define KEY_MATERIAL_LENGTH 32 // mackeyI, mackeyR
#define DERIVATED_KEY_LENGTH 16
#define SALT_LENGTH 14
#define KDF_CONTEXT_LENGTH 56

class StateMachine;

enum role {
    INITIATOR,
    RESPONDER
};

struct srtpKeyMaterial{
    uint8_t srtpKeyI [DERIVATED_KEY_LENGTH]; // Aes key length in Bytes
    uint8_t srtpSaltI[SALT_LENGTH]; // Salt length in Bytes
    uint8_t srtpKeyR [DERIVATED_KEY_LENGTH];
    uint8_t srtpSaltR[SALT_LENGTH];
    uint8_t macKeyI  [KEY_MATERIAL_LENGTH];
    uint8_t macKeyR  [KEY_MATERIAL_LENGTH];
    uint8_t zrtpKeyI [DERIVATED_KEY_LENGTH];
    uint8_t zrtpKeyR [DERIVATED_KEY_LENGTH];
};

using namespace std;

/**
 * @brief The ZRTPpoint class represent comunication node. Can be RESPONDER OR INITIATOR.
 */
class ZrtpPoint{

    friend class StateMachine;

private:

    Callbacks * zrtpPointCallbacks;
    StateMachine* engine;
    ZrtpEvent* zrtpPointEvent;

    uint8_t rs1 = 1;
    uint8_t rs2 = 2;
    uint8_t auxSecret = 3;
    uint8_t pbxSecret = 4;

    uint8_t rs1ID[SHARED_SECRET_LENGTH];
    uint8_t rs2ID[SHARED_SECRET_LENGTH];
    uint8_t auxSecretID[SHARED_SECRET_LENGTH];
    uint8_t pbxSecretID[SHARED_SECRET_LENGTH];

    uint8_t myH0[HASH_LENGTH_SHA256];
    uint8_t myH1[HASH_LENGTH_SHA256];
    uint8_t myH2[HASH_LENGTH_SHA256];
    uint8_t myH3[HASH_LENGTH_SHA256];

    uint8_t peersH0[HASH_LENGTH_SHA256];
    uint8_t peersH1[HASH_LENGTH_SHA256];
    uint8_t peersH2[HASH_LENGTH_SHA256];
    uint8_t peersH3[HASH_LENGTH_SHA256];

    uint8_t  s0 [CACHED_SECRET_LENGTH];
    uint8_t* s1;
    uint8_t* s2;
    uint8_t* s3;

    uint8_t zid[ZID_LENGTH];
    uint8_t hvi[HVI_LENGTH];

    role currentRole;
    srtpKeyMaterial currentSrtpKeyMaterial;
    userInfo currentUserInfo;
    uint16_t negotiatedKeySize;

    // Polar SSL context
    sha256_context sha256Context;
    ctr_drbg_context ctrDrbgContext;
    entropy_context entropyContext;
    dhm_context dhmContext;
    aes_context aesContext;

    // Attributes for polarSSL calculation
    size_t n;
    unsigned char buf[800];

    uint8_t myPublicValue [DH3K_PUBLIC_KEY_LENGTH];
    uint8_t dhResult [DH3K_PUBLIC_KEY_LENGTH];
    uint8_t totalHash [HASH_LENGTH_SHA256];
    uint8_t zrtpSess [HASH_LENGTH_SHA256];
    uint8_t exportedKey [HASH_LENGTH_SHA256];

    // KDF_CONTEXT = (ZIDi || Zidr || total_hash)
    uint8_t kdfContext[KDF_CONTEXT_LENGTH];
    uint8_t sasValue[WORD_LENGTH];

    // Messages
    HelloMessage*    helloMessage;
    HelloMessage*    respondersHello;
    HelloACKmessage* helloAckmessage;
    CommitMessage*   commitMessage;
    DHPart*          dhPart1Message;
    DHPart*          dhPart2Message;
    ConfirmMessage*  confirmMessage1;
    ConfirmMessage*  confirmMessage2;
    Conf2AckMessage* conf2AckMessage;
    ErrorMessage*    errorMessage;
    ErrorAckMessage* errorAckMessage;

public:

    /**
     * @brief ZRTPpoint create sprecific endpoint according to given role.
     * @param _role INITIATOR or RESPONDER.
     */
    ZrtpPoint(role _role, Callbacks *_callbacks);

    ~ZrtpPoint();

    /**
     * @brief processMessage function called when message came.
     * @param data of message
     * @param messageLength length of received data
     */
    void processMessage(uint8_t * data, unsigned int messageLength);

    /**
     * @brief processTimeout this function is called from outside, when Timeout occured.
     */
    void processTimeout();

    /**
     * @brief startEngine start ZRTP key exchange process.
     */
    void startEngine();

    /**
     * @brief setRole setter for role.
     * @param _role INITIATOR or RESPONDER.
     */
    void setRole(role _role) {currentRole = _role;}

    /**
     * @brief getCurrentRole is getter for param currentRole.
     * @return current Role.
     */
    role getCurrentRole() {return currentRole;}

    /**
     * @brief setMyH0 setter for myH0.
     * @param _myH0 value to set.
     */
    void setMyH0(uint8_t* _myH0) {memcpy(myH0, _myH0, HASH_LENGTH_SHA256);}

    /**
     * @brief setMyH1 setter for myH1.
     * @param _myH1 value to set.
     */
    void setMyH1(uint8_t* _myH1) {memcpy(myH1, _myH1, HASH_LENGTH_SHA256);}

    /**
     * @brief setMyH2 setter for myH2.
     * @param _myH2 value to set.
     */
    void setMyH2(uint8_t* _myH2) {memcpy(myH2, _myH2, HASH_LENGTH_SHA256);}

    /**
     * @brief setMyH3 setter for myH3.
     * @param _myH3 value to set.
     */
    void setMyH3(uint8_t* _myH3) {memcpy(myH3, _myH3, HASH_LENGTH_SHA256);}

    /**
     * @brief setPeersHash setter for peers hashes.
     * @param _hash new hash to set.
     * @param peersHash peers hash to be set.
     */
    void setPeersHash(uint8_t* _hash, uint8_t* peersHash) {memcpy(peersHash, _hash, HASH_LENGTH_SHA256);}

    /**
     * @brief calculateHashChain calculate hash chain composed from H0, H1, H2, H3.
     *    Also sets all H0, H1, H2, H3 with calculated values.
     *    myH0 = 256-bit random nonce
     *    myH1 = hash (myH0)
     *    myH2 = hash (myH1)
     *    myH3 = hash (myH2)
     */
    void calculateHashChain();

    /**
     * @brief fillWithRandomWalue fill data with random value.
     *        !!! IF random initialization fail assert() is called !!!
     * @param data which user want to set.
     * @param length of data.
     */
    void fillWithRandomWalue (uint8_t* data, uint32_t length);

    /**
     * @brief setZID calculate random zid and set it.
     */
    void setZID() {fillWithRandomWalue (zid, ZID_LENGTH);}

    /**
     * @brief calculateRandomSecrets random rs1, rs2, pbxSecret, AuxSecret and set to message.
     */
    void calculateRandomSecrets(DHPart* dhMessage);

    /**
     * @brief calculatePublicValue for key negotiation.
     *        !!! IF calculatePublicValue fail, assert() is called (POLARSSL ERROR may occur)!!!
     */
    void calculatePublicValue();

    /**
     * @brief readPublicValue read public value and calculate DhResult, also check if public value is not equal
     *        to 1 or p-1.
     * @param _dhPartMessage message to parse public value from.
     * @return error code if public value is equal to 1 or p - 1;
     */
    zrtpErrorCode readPublicValue(DHPart * _dhPartMessage);

    /**
     * @brief compareHashValues compare two hash values calculated in hash chain.
     * @param _currentHashValue is current value computed from previous.
     * @param _previousHashValue pre-hash value of currentHashValue.
     * @return true if is OK false Otherwise.
     */
    bool compareHashValues(uint8_t * _currentHashValue, uint8_t* _previousHashValue);

    /**
     * @brief calculateTotalHash calculate total hash.
     */
    void calculateTotalHash();

    /**
     * @brief calculateMac calculate mac of message.
     * @param _messageData data of message we want to compute mac from.
     * @param _messageLenght.
     * @param _messageType.
     */
    void calculateMac(uint8_t* _messageData, uint16_t _messageLenght,
                      MESSAGE_TYPE _messageType);

    /**
     * @brief verifyMac calculate and verify mac of message of given type
     * @param _messageData
     * @param _messageLenght
     * @param _messageType
     * @return TRUE if computed MAC is equal of received, FALSE otherwise.
     */
    bool verifyMac(uint8_t* _messageData, uint16_t _messageLenght,
                   MESSAGE_TYPE _messageType);

    /**
     * @brief calculateHvi compute Hvi value from responer`s Hello message and
     *        initiator DHPart2 message.
     *
     *        hvi == hash(initiator’s DHPart2 message ||responder’s Hello message)
     *        || means concatenation.
     *
     * @param helloMessage is hello message for calculate.
     */
    void calculateHvi(HelloMessage* _helloMessage);

    /**
     * @brief createEncryptPart encrypt part of Confirm message, copy it to message
     *        and set initialization vector.
     */
    void createEncryptPart(ConfirmMessage* _confirmMessage);

    /**
     * @brief calculateConfirmMac calculate confirm_mac of given confirm message.
     * @param _confirmMessage
     */
    void calculateConfirmMac(ConfirmMessage* _confirmMessage);

    /**
     * @brief prepareHelloMessage for send.
     */
    void prepareHelloMessage();

    /**
     * @brief keyDerivationFunction for derive rest of keys from s0.
     * @param valueToFill is value which we want to fill with computed key.
     * @param KI secret key derivation key.
     * @param label identifies the purpose for the derived keying material.
     * @param context field that includes ZIDi, ZIDr, and some optional nonce
              material known to both parties.
     * @param length the negotiated hash length.
     */
    void keyDerivationFunction(uint8_t* valueToFill, uint16_t _valueToFillLength, uint8_t* KI, uint16_t KI_length,
                               uint8_t *label, uint8_t* context, uint16_t contextSize, uint32_t length);

    /**
     * @brief prepareCommitMessage for send.
     */
    void prepareCommitMessage();

    /**
     * @brief prepareDhPart1Message for send.
     */
    void prepareDhPart1Message();

    /**
     * @brief prepareDhPart2Message for send.
     */
    void prepareDhPart2Message();

    /**
     * @brief prepareConfirm1Message for send.
     */
    void prepareConfirm1Message();

    /**
     * @brief prepareConfirm2Message for send.
     */
    void prepareConfirm2Message();

    /**
     * @brief calculateZrtpSessAndExportedKey  function calculate zrtpSess key an exportedKey with KDF function.
     */
    void calculateZrtpSessAndExportedKey();

    /**
     * @brief calculateSas calculate sasHash and sas value.
     */
    void calculateSas();

    /**
     * @brief calculateZrtpKeyMaterial calculate all zrtpKey material :
     *       srtpKeyI, srtpSaltI, srtpKeyR, srtpSaltR, macKeyI, macKeyR
     *       zrtpKeyI, zrtpKeyR
     */
    void calculateZrtpKeyMaterial();

    /**
     * @brief calculateS0 calculate S0 secret and KDF_context
     */
    void calculateS0();

    /**
     * @brief calculateAll calculate all secret material after DHresult calculation it call these function:
     *      - calculateTotalHash()
     *      - calculateS0()
     *      - calculateZrtpSessAndExportedKey()
     *      - calculateSas()
     *      - calculateZrtpKeyMaterial()
     */
    void calculateAll();

    /**
     * @brief renderSAS function use negotiated SAS type block and render SAS to user
     */
    uint8_t *renderSAS();

    /**
     * @brief decryptConfirmMessage decrypt confirm message
     * @param _confirmMsg message to decrypt.
     */
    void decryptConfirmMessage(ConfirmMessage * _confirmMsg);

    /**
     * @brief verifyConfirmMac.
     * @param _confirmMsg confirm message we want to verify.
     * @return true if confirm mac is correct, false otherwise.
     */
    bool verifyConfirmMac(ConfirmMessage* _confirmMsg);

    /**
     * @brief addSupported add version or supported algoritm to user info.
     * @param valueToAdd name of algoritm or version.
     * @param typeOfValue 1 - version
     *                    2 - hash algorithm
     *                    3 - cipher algorithm
     *                    4 - auth tag type
     *                    5 - key agreement type
     *                    6 - sas type
     */
    void addSupported(const char* valueToAdd, uint8_t typeOfValue);

    /**
     * @brief searchVersion search protocol version in supported algorithm.
     * @param _version to search.
     * @return true if found, false otherwise.
     */
    bool searchVersion(uint8_t* _version);

    /**
     * @brief findHighestVersion find highest version which is less than actual.
     * @param _highestVersion current higest version.
     * @return second highest version.
     */
    void findHighestVersion();

    /**
     * @brief algorithmNegotiation key algorithm negotiation.
     * @return Key_ALGORITHM NOT SUPPORTED error if no algorithm found.
     */
    zrtpErrorCode algorithmNegotiation();

    /**
     * @brief writeOutKeys write out negotiated keys and sas;
     */
    void writeOutKeys();

    /**
     * @brief writeToFile write keys to file.
     * @param _file to write.
     */
    void writeToFile(fstream &_file);
};

#endif // ZRTPOINT_H
