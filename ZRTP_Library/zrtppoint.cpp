#include "zrtppoint.h"

ZrtpPoint::ZrtpPoint(role _role, Callbacks *_callbacks){

    setRole(_role);
    zrtpPointCallbacks = _callbacks;
    engine = new StateMachine(this);

    // Init all polar SSL contexts
    sha256_init(&sha256Context);
    dhm_init( &dhmContext );
    entropy_init( &entropyContext );

    setZID();
    calculateHashChain();

    rs1 = 1;
    rs2 = 2;
    auxSecret = 3;
    pbxSecret = 4;

    s1 = nullptr;
    s2 = nullptr;
    s3 = nullptr;

    // Avoid unitialzed byte error
    memset (&currentUserInfo, 0 , sizeof(currentUserInfo));
    currentUserInfo.protocolVersion.reserve(5);

    currentUserInfo.supportedAuthTagType.reserve(MAX_ALGORITHM_COUNT);
    currentUserInfo.supportedCipherAlhorithm.reserve(MAX_ALGORITHM_COUNT);
    currentUserInfo.supportedHashAlgorithm.reserve(MAX_ALGORITHM_COUNT);
    currentUserInfo.supportedKeyAgreementType.reserve(MAX_ALGORITHM_COUNT);
    currentUserInfo.supportedSasType.reserve(MAX_ALGORITHM_COUNT);

    addSupported((const char*) "1.10", 1);

    addSupported((const char*) "S256", 2);
    addSupported((const char*) "AES1", 3);
    addSupported((const char*) "S512", 2);
    addSupported((const char*) "HS32", 4);
    addSupported((const char*) "DH3k", 5);
    addSupported((const char*) "B32 ", 6);

    memset (hvi,0,HVI_LENGTH);
}

ZrtpPoint::~ZrtpPoint(){

    delete zrtpPointCallbacks;
    delete engine;
    delete zrtpPointEvent;

    delete s1;
    delete s2;
    delete s3;

    sha256_free(&sha256Context);
    ctr_drbg_free(&ctrDrbgContext);
    entropy_free(&entropyContext);
    aes_free(&aesContext);

    currentUserInfo.protocolVersion.clear();
    currentUserInfo.supportedAuthTagType.clear();
    currentUserInfo.supportedCipherAlhorithm.clear();
    currentUserInfo.supportedHashAlgorithm.clear();
    currentUserInfo.supportedKeyAgreementType.clear();
    currentUserInfo.supportedSasType.clear();
}

void ZrtpPoint::processMessage(uint8_t *data, unsigned int messageLength){

    zrtpPointEvent = new ZrtpEvent();

    zrtpPointEvent->eventType = MESSAGE;
    zrtpPointEvent->messageData = data;
    zrtpPointEvent->messageDataLength = messageLength;

    engine->processEvent(zrtpPointEvent);

    delete zrtpPointEvent;
}

void ZrtpPoint::processTimeout(){

    zrtpPointEvent = new ZrtpEvent();

    zrtpPointEvent->eventType = TIME;
    engine->processEvent(zrtpPointEvent);

    delete zrtpPointEvent;
}

void ZrtpPoint::startEngine(){

    zrtpPointEvent = new ZrtpEvent();

    zrtpPointEvent->eventType = START;
    engine->processEvent(zrtpPointEvent);

    delete zrtpPointEvent;
}

void ZrtpPoint::fillWithRandomWalue(uint8_t *data, uint32_t length){

    assert ((ctr_drbg_init( &ctrDrbgContext, entropy_func, &entropyContext, NULL, 0) == 0 ));

    assert ((ctr_drbg_random( &ctrDrbgContext, data, length ) == 0 ));
}

void ZrtpPoint::calculateHashChain(){

    // Fill myH0 with random value.
    fillWithRandomWalue(myH0, HASH_LENGTH_SHA256);

    // Calculate rest of hashes.
    sha256(myH0, HASH_LENGTH_SHA256, myH1, 0);
    sha256(myH1, HASH_LENGTH_SHA256, myH2, 0);
    sha256(myH2, HASH_LENGTH_SHA256, myH3, 0);   
}

void ZrtpPoint::calculateMac(uint8_t* _messageData, uint16_t _messageLenght,
                             MESSAGE_TYPE _messageType){

    /*
    Key will be either H0, H1 or H2 according to messageType
    We calculate this Mac only in Hello, Commit and DhPart1 / DhPart2 message.
    */
    uint8_t key[HASH_LENGTH_SHA256];

    // Set key according to message.
    switch(_messageType){
        case HELLO_MESSAGE: memcpy(key, myH2, HASH_LENGTH_SHA256); break;
        case COMMIT_MESSAGE: memcpy(key, myH1, HASH_LENGTH_SHA256); break;
        case DHPART1_MESSAGE: memcpy(key, myH0, HASH_LENGTH_SHA256); break;
        case DHPART2_MESSAGE: memcpy(key, myH0, HASH_LENGTH_SHA256); break;
    default: break;
    }

    uint8_t tempMac [HASH_LENGTH_SHA256];

    //_messageLenght - MAC_LENGTH = we must deduct MAC field.
    sha256_hmac(key, HASH_LENGTH_SHA256, (_messageData + PACKET_HEAD_LENGTH),
                _messageLenght - MAC_LENGTH, tempMac, 0);

    // Set mac to of given message
    switch(_messageType){
        case HELLO_MESSAGE: helloMessage->setMac(tempMac); break;
        case COMMIT_MESSAGE: commitMessage->setMac(tempMac); break;
        case DHPART1_MESSAGE: dhPart1Message->setMac(tempMac); break;
        case DHPART2_MESSAGE: dhPart2Message->setMac(tempMac); break;
    default: break;
    }
}

bool ZrtpPoint::verifyMac(uint8_t *_messageData, uint16_t _messageLenght, MESSAGE_TYPE _messageType){

    uint8_t key [HASH_LENGTH_SHA256];

    /*
    Key will be either peers H0, H1 or H2 according to messageType
    We calculate this Mac only in Hello, Commit and DhPart1 / DhPart2 message.
    */
    switch(_messageType){
        case HELLO_MESSAGE: memcpy(key, peersH2, HASH_LENGTH_SHA256); break;
        case COMMIT_MESSAGE: memcpy(key, peersH1, HASH_LENGTH_SHA256); break;
        case DHPART1_MESSAGE: memcpy(key, peersH0, HASH_LENGTH_SHA256); break;
        case DHPART2_MESSAGE: memcpy(key, peersH0, HASH_LENGTH_SHA256); break;
    default: break;
    }

    uint8_t calculatedMac [HASH_LENGTH_SHA256];
    uint8_t macToCompare [MAC_LENGTH];

    //_messageLenght - MAC_LENGTH = we must deduct MAC field.
    sha256_hmac(key, HASH_LENGTH_SHA256, (_messageData + PACKET_HEAD_LENGTH),
                _messageLenght - MAC_LENGTH , calculatedMac, 0);

    switch(_messageType){
        case HELLO_MESSAGE: memcpy(macToCompare, respondersHello->getMac(), MAC_LENGTH); break;
        case COMMIT_MESSAGE: memcpy(macToCompare, commitMessage->getMac(), MAC_LENGTH); break;
        case DHPART1_MESSAGE: memcpy(macToCompare, dhPart1Message->getMac(), MAC_LENGTH); break;
        case DHPART2_MESSAGE: memcpy(macToCompare, dhPart2Message->getMac(), MAC_LENGTH); break;
    default: break;
    }

    if (memcmp(calculatedMac, macToCompare, MAC_LENGTH) != 0){
        return false;
    }   else{
            return true;
        }
}

void ZrtpPoint::calculateRandomSecrets(DHPart* dhMessage){

    uint8_t tempRs1[HASH_LENGTH_SHA256];
    uint8_t tempRs2[HASH_LENGTH_SHA256];
    uint8_t tempAux[HASH_LENGTH_SHA256];
    uint8_t tempPbx[HASH_LENGTH_SHA256];

    fillWithRandomWalue(tempRs1, HASH_LENGTH_SHA256);
    fillWithRandomWalue(tempRs2, HASH_LENGTH_SHA256);
    fillWithRandomWalue(tempAux, HASH_LENGTH_SHA256);
    fillWithRandomWalue(tempPbx, HASH_LENGTH_SHA256);

    dhMessage->setRs1ID(tempRs1);
    dhMessage->setRs1ID(tempRs2);
    dhMessage->setRs1ID(tempAux);
    dhMessage->setRs1ID(tempPbx);
}

void ZrtpPoint::calculatePublicValue(){   

    assert (ctr_drbg_init( &ctrDrbgContext, entropy_func, &entropyContext, NULL, 0) == 0 );

    assert (mpi_read_string(&dhmContext.P ,16, POLARSSL_DHM_RFC3526_MODP_3072_P) == 0);

    assert (mpi_read_string(&dhmContext.G ,16, POLARSSL_DHM_RFC3526_MODP_3072_G) == 0);

    assert ((dhm_make_params( &dhmContext, (int) mpi_size( &dhmContext.P ), buf, &n,
                              ctr_drbg_random, &ctrDrbgContext )) == 0);

    //Write public value to myPublicValue.
    assert (mpi_write_binary(&dhmContext.GX, myPublicValue, sizeof(myPublicValue)) == 0);
}

void ZrtpPoint::calculateHvi(HelloMessage *_helloMessage){

    uint8_t* hashData = new uint8_t[respondersHello->getMessageLength() + dhPart2Message->getMessageLength()];

   /*
   Hello data and Dhpar2 data contains whole packet, we must skip packet which includes
   head, sequence number, magic cookie and source identifier.
   */

   // Copy hello message data.
   memcpy(hashData, (_helloMessage->getHelloData() + PACKET_HEAD_LENGTH),
          _helloMessage->getMessageLength());

   // After hello, we copy dhPart data.
   memcpy(hashData + _helloMessage->getMessageLength(), dhPart2Message->getDHData() + PACKET_HEAD_LENGTH,
          dhPart2Message->getMessageLength());

   sha256(hashData, _helloMessage->getMessageLength() + dhPart2Message->getMessageLength(), hvi, 0);

   delete[] hashData;
}

void ZrtpPoint::createEncryptPart(ConfirmMessage *_confirmMessage){

    _confirmMessage->initializeEncryptedPart();
    aes_init(&aesContext);

    // Set key according to role.
    (currentRole == INITIATOR) ? (aes_setkey_enc(&aesContext, currentSrtpKeyMaterial.zrtpKeyI, 128)) :
                                 (aes_setkey_enc(&aesContext, currentSrtpKeyMaterial.zrtpKeyR, 128));

    uint8_t output [40];
    uint8_t tempIV [16];

    memset(output, 0, 40);
    memset(tempIV,0,16);

    size_t n = 2;

    _confirmMessage->setInitializationVector(tempIV);

    // Encoding
    aes_crypt_cfb128(&aesContext, AES_ENCRYPT, 40, &n, tempIV, _confirmMessage->getEncryptedPart(), output);

    _confirmMessage->setEncryptedData(output);
}

void ZrtpPoint::calculateConfirmMac(ConfirmMessage* _confirmMessage){

    uint8_t tempConfirmMac [HASH_LENGTH_SHA256];

    if (currentRole == INITIATOR) {
        sha256_hmac(currentSrtpKeyMaterial.macKeyI, KEY_MATERIAL_LENGTH, _confirmMessage->getEncryptedPart(),
                    ENCRYPTED_PART_LENGTH, tempConfirmMac, 0);
    }   else {
           sha256_hmac(currentSrtpKeyMaterial.macKeyR, KEY_MATERIAL_LENGTH, _confirmMessage->getEncryptedPart(),
                       ENCRYPTED_PART_LENGTH, tempConfirmMac, 0);
        }

    _confirmMessage->setConfirmMac(tempConfirmMac);
}

bool ZrtpPoint::verifyConfirmMac(ConfirmMessage *_confirmMsg){

    uint8_t tempConfirmMac [HASH_LENGTH_SHA256];

    if (currentRole == RESPONDER) {
        sha256_hmac(currentSrtpKeyMaterial.macKeyI, KEY_MATERIAL_LENGTH, _confirmMsg->getEncryptedPart(),
                    ENCRYPTED_PART_LENGTH, tempConfirmMac, 0);
    }   else {
            sha256_hmac(currentSrtpKeyMaterial.macKeyR, KEY_MATERIAL_LENGTH, _confirmMsg->getEncryptedPart(),
                        ENCRYPTED_PART_LENGTH, tempConfirmMac, 0);
        }

    if(memcmp(tempConfirmMac, _confirmMsg->getConfirmMac(), MAC_LENGTH) != 0){
        return false;
    }   else {
            return true;
        }
}

void ZrtpPoint::keyDerivationFunction(uint8_t *valueToFill, uint16_t _valueToFillLength, uint8_t *KI, uint16_t KI_length,
                                      uint8_t* label, uint8_t* context, uint16_t contextSize, uint32_t length){

    // _counter has fixed value of 1, because we compute mac only one.
    uint32_t counter = 0x00000001;
    // delimiter - 0x00 is a delimiter required by NIST.
    uint8_t delimiter = 0x00;

    uint32_t hashedDataLength = sizeof(counter) + strlen((char*)label) + sizeof(delimiter) + contextSize +
                                sizeof (length);

    uint8_t* dataToHash = new uint8_t[hashedDataLength];

    // copy all parameters to data.
    uint16_t p = 0;
    memcpy(dataToHash, &counter, sizeof(counter));
    p += sizeof(counter);

    memcpy(dataToHash + p, label, strlen((char*)label));
    p += strlen((char*)label);

    memcpy(dataToHash + p, &delimiter, sizeof (delimiter));
    p += sizeof(delimiter);

    memcpy(dataToHash + p, context, contextSize);
    p += contextSize;

    memcpy(dataToHash + p, &length, sizeof(length));

    uint8_t* tempHash = new uint8_t[HASH_LENGTH_SHA256];

    // Calculate hash from copied data.
    sha256_hmac(KI, KI_length, dataToHash, hashedDataLength, tempHash, 0);

    // Copy calculated data to our value.
    memcpy(valueToFill, tempHash, _valueToFillLength);

    delete[] dataToHash;
    delete[] tempHash;
}

void ZrtpPoint::calculateTotalHash(){

    uint16_t helloMessageLength = 0;

    // We must use responder`s hello for total hash calculation
    (currentRole == INITIATOR) ? (helloMessageLength = respondersHello->getMessageLength()) :
                                 (helloMessageLength = helloMessage->getMessageLength());

    uint16_t hashedDataLength = (helloMessageLength + commitMessage->getMessageLength() + dhPart1Message->getMessageLength() +
                                 dhPart2Message->getMessageLength());

    uint8_t* dataToHash = new uint8_t[hashedDataLength];
    uint16_t copiedLength = 0;

    // Copy all messages data to hash data.
    if (currentRole == INITIATOR) {
        memcpy(dataToHash, (respondersHello->getHelloData() + PACKET_HEAD_LENGTH), helloMessageLength );
    }   else {
            memcpy(dataToHash, (helloMessage->getHelloData() + PACKET_HEAD_LENGTH), helloMessageLength);
        }

    copiedLength += helloMessageLength;
    memcpy(dataToHash + copiedLength, (commitMessage->getCommitData() + PACKET_HEAD_LENGTH), commitMessage->getMessageLength());
    copiedLength += commitMessage->getMessageLength();
    memcpy(dataToHash + copiedLength, (dhPart1Message->getDHData() + PACKET_HEAD_LENGTH), dhPart1Message->getMessageLength());
    copiedLength += dhPart1Message->getMessageLength();
    memcpy(dataToHash + copiedLength, (dhPart2Message->getDHData() + PACKET_HEAD_LENGTH), dhPart2Message->getMessageLength());

    sha256(dataToHash, hashedDataLength, totalHash, 0);

    delete[] dataToHash;
}

void ZrtpPoint::prepareHelloMessage(){

    // We set in hello message version and protocol that user support
    helloMessage->setProtocolVersion((uint8_t *) currentUserInfo.protocolVersion[0]);

    // Copy all supported algorithms.
    for (uint16_t i = 0; i < currentUserInfo.supportedAuthTagType.size(); i++){
           helloMessage->addAuthTagType((uint8_t*) currentUserInfo.supportedAuthTagType[i]);
    }

    for (uint16_t i = 0; i < currentUserInfo.supportedCipherAlhorithm.size(); i++){
        helloMessage->addCipherAlgorithm((uint8_t*) currentUserInfo.supportedCipherAlhorithm[i]);
    }

    for (uint16_t i = 0; i < currentUserInfo.supportedHashAlgorithm.size(); i++ ){
        helloMessage->addHashAlgorithm((uint8_t*) currentUserInfo.supportedHashAlgorithm[i]);
    }

    for (uint16_t i = 0; i < currentUserInfo.supportedKeyAgreementType.size(); i++ ){
        helloMessage->addKeyAgreementType((uint8_t*) currentUserInfo.supportedKeyAgreementType[i]);
    }

    for (uint16_t i = 0; i < currentUserInfo.supportedSasType.size(); i++ ) {
        helloMessage->addSasType((uint8_t*) currentUserInfo.supportedSasType[i]);
    }

    helloMessage->setZID((uint8_t*) zid);
    helloMessage->setHashImageH3(myH3);

    helloMessage->initializeMessageData();

    // When hello is initialized we calculate mac of hello message.
    calculateMac(helloMessage->getHelloData(), helloMessage->getMessageLength(),
                 HELLO_MESSAGE);
}

void ZrtpPoint::prepareCommitMessage(){

    commitMessage->setHashImageH2(myH2);
    commitMessage->setZid(zid);

    // We set negotiated algorithms. In this implementation are algoritms set default, because both endpoint
    // because we support basic set of funcition.
    commitMessage->setAgreedHashAlgorithm((uint8_t *) currentUserInfo.supportedHashAlgorithm[0]);
    commitMessage->setAgreedCipherAlgorithm((uint8_t *) currentUserInfo.supportedCipherAlhorithm[0]);
    commitMessage->setAgreedAuthTagAlgorithm((uint8_t *) currentUserInfo.supportedAuthTagType[0]);
    //commitMessage->setAgreedKeyAgreementType((uint8_t *) currentUserInfo.supportedKeyAgreementType[0]);
    commitMessage->setAgreedSasType((uint8_t *) currentUserInfo.supportedSasType[0]);
    commitMessage->setHvi(hvi);

    commitMessage->initializeMessageData();

    calculateMac(commitMessage->getCommitData(), commitMessage->getMessageLength(), COMMIT_MESSAGE);
}

void ZrtpPoint::prepareDhPart1Message(){

    dhPart1Message->setMessageType((uint8_t*) "DHPart1 ");

    dhPart1Message->setHashImageH1(myH1);

    // This function set rs1, rs2, aux, pbx to random, because we dont have any shared secret.
    calculateRandomSecrets(dhPart1Message);
    calculatePublicValue();

    dhPart1Message->setPublicValue(myPublicValue);
    dhPart1Message->initializeMessageData();

    // Calculate MAC at the end.
    calculateMac(dhPart1Message->getDHData(), dhPart1Message->getMessageLength(),
                 DHPART1_MESSAGE);
}

void ZrtpPoint::prepareDhPart2Message(){

    dhPart2Message->setMessageType((uint8_t*) "DHPart2 ");

    dhPart2Message->setHashImageH1(myH1);

    calculateRandomSecrets(dhPart2Message);
    calculatePublicValue();

    dhPart2Message->setPublicValue(myPublicValue);
    dhPart2Message->initializeMessageData();

    calculateMac(dhPart2Message->getDHData(), dhPart2Message->getMessageLength(),
                 DHPART2_MESSAGE);
}

void ZrtpPoint::prepareConfirm1Message() {

    confirmMessage1->setMessageType((uint8_t *) "Confirm1");
    confirmMessage1->setHashImageH0(myH0);

    // Encrypt part of confirm.

    createEncryptPart(confirmMessage1);
    confirmMessage1->initializeMessageData();

    // Calculate confirm mac at the end, this protect encrypted part.
    calculateConfirmMac(confirmMessage1);
    confirmMessage1->initializeMessageData();

}

void ZrtpPoint::prepareConfirm2Message() {

    confirmMessage2->setMessageType((uint8_t *) "Confirm2");
    confirmMessage2->setHashImageH0(myH0);

    createEncryptPart(confirmMessage2);
    confirmMessage1->initializeMessageData();

    calculateConfirmMac(confirmMessage2);
    confirmMessage2->initializeMessageData();
}


bool ZrtpPoint::compareHashValues(uint8_t *_currentHashValue, uint8_t *_previousHashValue){
    uint8_t* tempHash = new uint8_t [HASH_LENGTH_SHA256];

    sha256(_previousHashValue, HASH_LENGTH_SHA256, tempHash, 0);

    if ((memcmp(_currentHashValue, tempHash, HASH_LENGTH_SHA256) == 0)) {
        delete[] tempHash;
        return true;
    }   else {
            delete[] tempHash;
            return false;
        }
}

zrtpErrorCode ZrtpPoint::readPublicValue(DHPart *_dhPartMessage){

    zrtpErrorCode tempError = N_ERROR;

    mpi tempMpi;
    mpi_init(&tempMpi);

    // testMpi will be initialized to P-1
    assert (mpi_sub_int(&tempMpi, &dhmContext.P, 1) == 0);
    // read received public value
    assert (mpi_read_binary(&dhmContext.GY, _dhPartMessage->getPublicValue(), DH3K_PUBLIC_KEY_LENGTH) == 0);

    // Compare if is not equal to 1 or p - 1
    if (mpi_cmp_int(&dhmContext.GY, 1) == 0 || (mpi_cmp_mpi(&tempMpi, &dhmContext.P) == 0)){
        return tempError = DH_ERROR_BAD_PUBLIC_VALUE;
    }

    // Calculate key.
    assert (dhm_calc_secret( &dhmContext, buf, &n, ctr_drbg_random, &ctrDrbgContext ) == 0);
    assert (mpi_write_binary(&dhmContext.K, dhResult, sizeof(dhResult)) == 0);

    mpi_free(&tempMpi);
    return tempError;
}

void ZrtpPoint::calculateS0(){

    uint32_t lenS1 = 0;
    uint32_t lenS2 = 0;
    uint32_t lenS3 = 0;
    uint32_t counter = 0x00000001;
    const char * text = "ZRTP-HMAC-KDF";

    // Check previous shared secret.
    if (s1 == nullptr) {
        lenS1 = 0x00000000;
    }

    if (s2 == nullptr) {
        lenS2 = 0x00000000;
    }

    if (s3 == nullptr) {
        lenS3 = 0x00000000;
    }

    uint32_t hashDataLength = sizeof(counter) + DH3K_PUBLIC_KEY_LENGTH + strlen(text) + (2 * ZID_LENGTH) + sizeof(totalHash) +
                              sizeof(lenS1) + lenS1 + sizeof(lenS2) + lenS2 + sizeof(lenS3) + lenS3;

    uint8_t* dataToHash = new uint8_t[(hashDataLength)];

    // Initialize data for hash
    uint32_t p = 0;
    memcpy(dataToHash, &counter, sizeof(counter));
    p += sizeof(counter);

    // Copy dhResult
    memcpy(dataToHash + p, dhResult, sizeof(dhResult));
    p += sizeof(dhResult);
    memcpy(dataToHash + p, text, strlen(text));
    p += strlen(text);

    // Next items in hash data should be ZidI and Zidr
    if (currentRole == INITIATOR) {
        (memcpy(dataToHash + p, helloMessage->getZID(), ZID_LENGTH));
         p += ZID_LENGTH;
        (memcpy(dataToHash + p, respondersHello->getZID(), ZID_LENGTH));
         p += ZID_LENGTH;
    }   else {
            (memcpy(dataToHash + p, respondersHello->getZID(), ZID_LENGTH));
            p += ZID_LENGTH;
            (memcpy(dataToHash + p, helloMessage->getZID(), ZID_LENGTH));
            p += ZID_LENGTH;
        }

    memcpy(dataToHash + p, totalHash, HASH_LENGTH_SHA256);
    p += HASH_LENGTH_SHA256;
    memcpy(dataToHash + p, &lenS1, sizeof(lenS1));
    p += sizeof(lenS1);

    if(s1 != nullptr){
        memcpy(dataToHash + p, s1, lenS1);
        p += lenS1;
    }

    memcpy(dataToHash + p, &lenS2, sizeof(lenS2));
    p += sizeof(lenS2);
    if(s2 != nullptr) {
        memcpy(dataToHash + p, s2, lenS2);
        p += lenS2;
   }

    memcpy(dataToHash + p, &lenS3, sizeof(lenS3));
    p += sizeof(lenS3);
    if (s3 != nullptr) {
        memcpy(dataToHash + p, s3, lenS3);
        p += lenS3;
    }

    sha256(dataToHash, hashDataLength, s0, 0);

    // Set Kdf Context
    if (currentRole == INITIATOR) {
        (memcpy(kdfContext, helloMessage->getZID(), ZID_LENGTH));
        (memcpy(kdfContext + ZID_LENGTH, respondersHello->getZID(), ZID_LENGTH));
    }   else {
            (memcpy(kdfContext, respondersHello->getZID(), ZID_LENGTH));
            (memcpy(kdfContext + ZID_LENGTH, helloMessage->getZID(), ZID_LENGTH));
        }
    memcpy(kdfContext + (2 * ZID_LENGTH), totalHash, sizeof(totalHash));

    memset(dhResult, 0, sizeof(dhResult));
    memset(myPublicValue, 0, sizeof(myPublicValue));

    dhm_free(&dhmContext);
    memset(myPublicValue, 0, sizeof(myPublicValue));
    memset(totalHash, 0, sizeof(totalHash));

    delete[] dataToHash;
}

void ZrtpPoint::calculateZrtpSessAndExportedKey(){

    keyDerivationFunction(zrtpSess, HASH_LENGTH_SHA256, s0, CACHED_SECRET_LENGTH, (uint8_t*) "ZRTP Session Key",
                          kdfContext, KDF_CONTEXT_LENGTH,256);
    keyDerivationFunction(exportedKey, HASH_LENGTH_SHA256, s0, CACHED_SECRET_LENGTH,(uint8_t*)  "Exported key",
                          kdfContext, KDF_CONTEXT_LENGTH, 256);
}

void ZrtpPoint::calculateSas(){

    uint8_t sashash [HASH_LENGTH_SHA256];
    memset(sashash, 0, HASH_LENGTH_SHA256);

    keyDerivationFunction(sashash, HASH_LENGTH_SHA256, s0, CACHED_SECRET_LENGTH, (uint8_t*)  "SAS",
                          kdfContext, KDF_CONTEXT_LENGTH, 256);

    memcpy(sasValue, sashash, WORD_LENGTH);
}

void ZrtpPoint::calculateZrtpKeyMaterial(){

    keyDerivationFunction(currentSrtpKeyMaterial.srtpKeyI, DERIVATED_KEY_LENGTH, s0, CACHED_SECRET_LENGTH,(uint8_t*) "Initiator SRTP master key",
                          kdfContext, KDF_CONTEXT_LENGTH, 128);

    keyDerivationFunction(currentSrtpKeyMaterial.srtpSaltI, SALT_LENGTH, s0, CACHED_SECRET_LENGTH, (uint8_t*) "Initiator SRTP master salt",
                          kdfContext, KDF_CONTEXT_LENGTH, 112);

    keyDerivationFunction(currentSrtpKeyMaterial.srtpKeyR, DERIVATED_KEY_LENGTH, s0, CACHED_SECRET_LENGTH, (uint8_t*)  "Responder SRTP master key",
                          kdfContext, KDF_CONTEXT_LENGTH, 128);

    keyDerivationFunction(currentSrtpKeyMaterial.srtpSaltR, SALT_LENGTH, s0, CACHED_SECRET_LENGTH, (uint8_t*) "Responder SRTP master salt",
                          kdfContext, KDF_CONTEXT_LENGTH, 112);

    keyDerivationFunction(currentSrtpKeyMaterial.macKeyI, KEY_MATERIAL_LENGTH, s0, CACHED_SECRET_LENGTH, (uint8_t*) "Initiator HMAC key",
                          kdfContext, KDF_CONTEXT_LENGTH, 256);

    keyDerivationFunction(currentSrtpKeyMaterial.macKeyR, KEY_MATERIAL_LENGTH, s0, CACHED_SECRET_LENGTH, (uint8_t*) "Responder HMAC key",
                          kdfContext, KDF_CONTEXT_LENGTH, 256);

    keyDerivationFunction(currentSrtpKeyMaterial.zrtpKeyI, DERIVATED_KEY_LENGTH, s0, CACHED_SECRET_LENGTH, (uint8_t*) "Initiator ZRTP key",
                          kdfContext, KDF_CONTEXT_LENGTH, 128);

    keyDerivationFunction(currentSrtpKeyMaterial.zrtpKeyR, DERIVATED_KEY_LENGTH, s0, CACHED_SECRET_LENGTH, (uint8_t*) "Responder ZRTP key",
                          kdfContext, KDF_CONTEXT_LENGTH, 128);
}

void ZrtpPoint::calculateAll(){

    calculateTotalHash();
    calculateS0();
    calculateZrtpSessAndExportedKey();
    calculateSas();
    calculateZrtpKeyMaterial();

    memset(s0, 0, CACHED_SECRET_LENGTH);
    memset(kdfContext, 0, KDF_CONTEXT_LENGTH);
}

uint8_t* ZrtpPoint::renderSAS(){

    uint32_t bits;
    memcpy(&bits, sasValue, WORD_LENGTH);

    int i, n, shift;
    uint8_t * result = new uint8_t[WORD_LENGTH];

    for (i=0,shift=27; i!=4; ++i,shift-=5){
        n = (bits>>shift) & 31;
        result[i] = "ybndrfg8ejkmcpqxot1uwisza345h769"[n];
    }

    return result;
}

void ZrtpPoint::decryptConfirmMessage(ConfirmMessage *_confirmMsg){

    size_t n = 2;
    uint8_t output[40];

    (currentRole == RESPONDER) ? (aes_setkey_enc(&aesContext, currentSrtpKeyMaterial.zrtpKeyI, 128)) :
                                 (aes_setkey_enc(&aesContext, currentSrtpKeyMaterial.zrtpKeyR, 128));

    //Decoding
    aes_crypt_cfb128(&aesContext, AES_DECRYPT, 40, &n, _confirmMsg->getInitializationVector(),
                     _confirmMsg->getEncryptedPart(), output);

    _confirmMsg->setEncryptedData(output);
    _confirmMsg->initializeMessageData();

}

void ZrtpPoint::addSupported(const char *valueToAdd, uint8_t typeOfValue){

    // Add highet supported version at beginin
    if (typeOfValue == 1){
        if (currentUserInfo.protocolVersion.size() == 0){
            currentUserInfo.protocolVersion.push_back(valueToAdd);
            return;
        }
        float number;
        number = atof(currentUserInfo.protocolVersion[0]);
        if (number < atof(valueToAdd)){
            currentUserInfo.protocolVersion.push_back(currentUserInfo.protocolVersion[0]);
            currentUserInfo.protocolVersion[0] = valueToAdd;
        }   else {
                currentUserInfo.protocolVersion.push_back(valueToAdd);
            }
    }

    switch(typeOfValue){
        case(2) : currentUserInfo.supportedHashAlgorithm.push_back(valueToAdd); break;
        case(3) : currentUserInfo.supportedCipherAlhorithm.push_back(valueToAdd); break;
        case(4) : currentUserInfo.supportedAuthTagType.push_back(valueToAdd); break;
        case(5) : currentUserInfo.supportedKeyAgreementType.push_back(valueToAdd); break;
        case(6) : currentUserInfo.supportedSasType.push_back(valueToAdd); break;
    default : break;
    }
}

bool ZrtpPoint::searchVersion(uint8_t *_version){

    bool found = false;

    for (unsigned int i = 0; i < currentUserInfo.protocolVersion.size(); i++){
        if(memcmp(_version, currentUserInfo.protocolVersion[i], 4) == 0){
            found = true;
            break;
        }
    }

    return found;
}

void ZrtpPoint::findHighestVersion(){

    uint8_t minVersion[4];
    memcpy(minVersion, (const char*) "1.10", 4);

    for(unsigned int i = 0; i < currentUserInfo.protocolVersion.size(); i++){
        if (((memcmp(currentUserInfo.protocolVersion[i], minVersion, 3) >  0) ||
             (memcmp(currentUserInfo.protocolVersion[i], minVersion, 3) == 0)) &&
             (memcmp(currentUserInfo.protocolVersion[i], helloMessage->getProtocolVersion(), 3)!= 0)){
        }

        memcpy(minVersion, currentUserInfo.protocolVersion[i], WORD_LENGTH);
        helloMessage->setProtocolVersion(minVersion);
    }
}

zrtpErrorCode ZrtpPoint::algorithmNegotiation(){

    zrtpErrorCode returnCode = N_ERROR;

    std::vector< const char* > intersection;

    for (uint16_t i = 0; i < (respondersHello->getHelloCounts().kc)* WORD_LENGTH; i += WORD_LENGTH){
        for(uint16_t j = 0; j < currentUserInfo.supportedKeyAgreementType.size(); j++){
            if (memcmp((respondersHello->getKeyAgreementTypes() + i), currentUserInfo.supportedKeyAgreementType.at(j), WORD_LENGTH) == 0){
                intersection.push_back(currentUserInfo.supportedKeyAgreementType[j]);
            }
        }
    }

    if (intersection.empty()){
        returnCode = PUBLIC_KEY_EXCHANGE_NOT_SUPPORTED;
        return returnCode;
    }

    // Algorithm from fastest to slowest: DH2k, EC25, DH3k, EC38, EC52
    for (uint16_t i = 0; i < intersection.size(); i++){
        if (memcmp(intersection.at(i), (const char*) "DH2k", WORD_LENGTH) == 0){
            commitMessage->setAgreedKeyAgreementType((uint8_t*) "DH2k");
            negotiatedKeySize = DH2K_PUBLIC_KEY_LENGTH;
            return returnCode;
        }
    }

    for (uint16_t i = 0; i < intersection.size(); i++){
        if (memcmp(intersection.at(i), (const char*) "EC25", WORD_LENGTH) == 0){
            commitMessage->setAgreedKeyAgreementType((uint8_t*) "EC25");
            negotiatedKeySize = EC25_PUBLIC_KEY_LENGTH;
            return returnCode;
        }
    }

    for (uint16_t i = 0; i < intersection.size(); i++){
        if (memcmp(intersection.at(i), (const char*) "DH3k", WORD_LENGTH) == 0){
            commitMessage->setAgreedKeyAgreementType((uint8_t*) "DH3k");
            negotiatedKeySize = DH3K_PUBLIC_KEY_LENGTH;
            return returnCode;
        }
    }

    for (uint16_t i = 0; i < intersection.size(); i++){
        if (memcmp(intersection.at(i), (const char*) "EC38", WORD_LENGTH) == 0){
            commitMessage->setAgreedKeyAgreementType((uint8_t*) "EC38");
            negotiatedKeySize = EC38_PUBLIC_KEY_LENGTH;
            return returnCode;
        }
    }

    for (uint16_t i = 0; i < intersection.size(); i++){
        if (memcmp(intersection.at(i), (const char*) "EC52", WORD_LENGTH) == 0){
            commitMessage->setAgreedKeyAgreementType((uint8_t*) "EC52");
            negotiatedKeySize = EC52_PUBLIC_KEY_LENGTH;
            return returnCode;
        }
    }

    return returnCode;
}

void ZrtpPoint::writeOutKeys(){

    std::cout << std::endl;
    std::cout << "      SAS to compare: ";
    uint8_t * rendered = renderSAS();
    for(int i = 0; i < WORD_LENGTH; i++){
        std::cout << *(rendered + i);
    }
    std::cout << std::endl;
    delete [] rendered;

    std::cout << std::endl;
    std::cout << std::dec;
}

void ZrtpPoint::writeToFile(fstream &_file){

    _file << std::endl;
    _file << "sas: ";
    uint8_t * rendered = renderSAS();
    for(int i = 0; i < WORD_LENGTH; i++){
        _file << *(rendered + i);
    }
    _file << std::endl;
    delete [] rendered;

    _file << "srtpKeyI:  ";
    for(int i = 0; i < DERIVATED_KEY_LENGTH; i++){
        _file << std::hex <<  (int)*(currentSrtpKeyMaterial.srtpKeyI + i);
    }
    _file << std::endl;

    _file << "srtpSaltI: ";
    for(int i = 0; i < SALT_LENGTH; i++){
        _file << std::hex << (int)*(currentSrtpKeyMaterial.srtpSaltI + i);
    }
    _file << std::endl;

    _file << "srtpKeyR:  ";
    for(int i = 0; i < DERIVATED_KEY_LENGTH; i++){
        _file << std::hex << (int)*(currentSrtpKeyMaterial.srtpKeyR + i);
    }
    _file << std::endl;

    _file << "srtpSaltR: ";
    for(int i = 0; i < SALT_LENGTH; i++){
        _file << std::hex << (int)*(currentSrtpKeyMaterial.srtpSaltR + i);
    }
    _file << std::endl;
    _file << std::dec;
}



