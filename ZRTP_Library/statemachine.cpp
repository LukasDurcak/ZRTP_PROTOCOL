#include "statemachine.h"
#include <unistd.h>

using std::cout;
using std::endl;

void StateMachine::initTimers(){
    T1.capping = 200;
    T1.startTime = 50;
    T1.maxResend = 20;
    T1.timeoutsCounter = 0;

    T2.capping = 1200;
    T2.startTime = 150;
    T2.maxResend = 10;
    T2.timeoutsCounter = 0;
}

StateMachine::StateMachine(ZrtpPoint *_zrtpPoint){

    zrtpPoint = _zrtpPoint;
    currentErrorCode = N_ERROR;
    helloReceived = false;
    commitHandled = false;

    currentStateMap[InitialState]     = &StateMachine::handleInitialState;
    currentStateMap[HelloSent]        = &StateMachine::handleHelloSentState;
    currentStateMap[HelloAckSent]     = &StateMachine::handleHelloAckSentState;
    currentStateMap[HelloAckReceived] = &StateMachine::handleHelloAckReceivedState;
    currentStateMap[CommitSent]       = &StateMachine::handleCommitSentState;
    currentStateMap[WaitForCommit]    = &StateMachine::handleWaitForCommitState;
    currentStateMap[WaitForDH2]       = &StateMachine::handleWaitForDH2state;
    currentStateMap[WaitForConfirm1]  = &StateMachine::handleWaitForConfirm1State;
    currentStateMap[WaitForConfirm2]  = &StateMachine::handleWaitForConfirm2State;
    currentStateMap[WaitForConfAck]   = &StateMachine::handleWaitForConfAckState;
    currentStateMap[WaitForErrorAck]  = &StateMachine::handleWaitForErrorAckState;
    currentStateMap[SecuredState]     = &StateMachine::handleSecuredState;

    setState(InitialState);
    std::cout << std::endl << "## Current state: INITIAL ##" << std::endl;

    initTimers();
}

StateMachine::~StateMachine(){

    delete stateMachineEvent;
}

void StateMachine::resetTimer(){
    T2.capping = 1200;
    T2.startTime = 150;
    T2.maxResend = 10;
    T2.timeoutsCounter = 0;
    T2.actualTime = 0;
}

bool StateMachine::startTimer(ZrtpTimer *_timer){

    _timer->actualTime = _timer->startTime;

    return (zrtpPoint->zrtpPointCallbacks->startTimer(_timer->startTime));
}

bool StateMachine::nextTimer(ZrtpTimer *_timer){

    if (_timer->actualTime <= _timer->capping){
        _timer->actualTime *= 2;
    }

    if (_timer->timeoutsCounter <  _timer->maxResend){
        _timer->timeoutsCounter++;
    }   else {
            return false;
        }

    return (zrtpPoint->zrtpPointCallbacks->startTimer(_timer->startTime));
}

void StateMachine::processEvent(ZrtpEvent* _event){

    zrtpPoint->zrtpPointCallbacks->enterCriticalSection();

    stateMachineEvent = _event;

    // If EventType is START , engine send Hello message and start key negotiation.
    if ((stateMachineEvent->eventType == START) && (getCurrentState() == InitialState)){
        (this->*currentStateMap[currentState])();
    }

    if (stateMachineEvent->eventType == MESSAGE && currentState != InitialState){

        // Check if messageData are not NULL
        assert(stateMachineEvent->messageData != NULL);

        // We skip packet head and one word (length and unused zeros) to get to message
        setReceivedMessageType( stateMachineEvent->messageData + PACKET_HEAD_LENGTH + WORD_LENGTH);

        //We skip packet head length which is 12 Bytes and +2 to get to message length field.
        uint16_t receivedMessageLenght = *(uint16_t*) ( stateMachineEvent->messageData + PACKET_HEAD_LENGTH + 2);

        // Check if length in message is equal to received packet length
        if (((receivedMessageLenght * 4) != (stateMachineEvent->messageDataLength) - PACKET_WITHOUT_MESSAGE_LENGTH) &&
             currentState != WaitForErrorAck){

            // If no, send Eroor with mallformed packed
            zrtpPoint->errorMessage = new ErrorMessage(MALFORMED_PACKET);
            setErrorCode(MALFORMED_PACKET);
            sendErroMessage(MALFORMED_PACKET);
            return;
        }

        // Check if received message is error message.
        if (strncmp((const char *) receivedMessageType, (const char *) "Error   ", 8) == 0){

            // Copy received error code.
            memcpy(&currentErrorCode,
                  (uint32_t *) (stateMachineEvent->messageData + PACKET_HEAD_LENGTH + WORD_LENGTH + MESSAGE_TYPE_LENGTH),
                   WORD_LENGTH);

            zrtpPoint->errorAckMessage = new ErrorAckMessage();
            zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->errorAckMessage->getErrorAckData(),
                                                           ERRORACK_PACKET_SIZE);

            std::cerr << std::endl << "## Received Error message with error code: " << std::hex << currentErrorCode << " ##" << std::endl;

            setState(InitialState);
            std::cout << std::endl << "## Current state: INITIAL ##" << std::endl;
            return;
        }
    }

    // If event is STOP, we stop timer here and continue to state where STOP is handled.
    if (stateMachineEvent->eventType == STOP){
        zrtpPoint->zrtpPointCallbacks->stopTimer();
        setState(InitialState );
        std::cout << std::endl << "## Current state: INITIAL ##" << std::endl;
        return;
    }

    (this->*currentStateMap[currentState])();

    zrtpPoint->zrtpPointCallbacks->leaveCriticalSection();

    // This gives signal to aplication that key negotiation has ended
    // (we can start negotiation again for tests purposes).
    if(getCurrentState() == SecuredState){
        zrtpPoint->zrtpPointCallbacks->keyNegotitationEnded();
    }
}

void StateMachine::handleInitialState(){

    if (stateMachineEvent->eventType == START){

        zrtpPoint->helloMessage = new HelloMessage();
        zrtpPoint->prepareHelloMessage();

        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->helloMessage->getHelloData(),
                                                      zrtpPoint->helloMessage->getWholePacketLength());

        setLastSentPacket(zrtpPoint->helloMessage->getHelloData(), zrtpPoint->helloMessage->getWholePacketLength());

        if (!startTimer(&T1)){
            sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
            return;
        }

        setState(HelloSent);

        std::cout << std::endl << std::endl << "## Current state: HELLO SENT ##" << std::endl;
    }
}

void StateMachine::handleHelloSentState(){

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Hello   ",
        MESSAGE_TYPE_LENGTH) == 0){

        // Save responders hello for further calculation.
        // Check error code : MALFORMED PACKET, EQUAL ZID.
        zrtpPoint->respondersHello = new HelloMessage();
        if ((currentErrorCode = zrtpPoint->helloMessage->parseHelloMessage(zrtpPoint->respondersHello,
                                stateMachineEvent->messageData)) != N_ERROR) {

            sendErroMessage(currentErrorCode);
            return;
        }

        // Check if protocol version is not lower than 1.10 (lowest supported).
        if ((currentErrorCode = zrtpPoint->respondersHello->checkProtocolVersion()) != 0){
            sendErroMessage(currentErrorCode);
            return;
        }

        // If hello message with higher version received, continue sending our lower version
        if (memcmp(zrtpPoint->respondersHello->getProtocolVersion(),
                   zrtpPoint->helloMessage->getProtocolVersion(), WORD_LENGTH) > 0){

            if (nextTimer(&T1)){
                zrtpPoint->zrtpPointCallbacks->sendData(lastSentMessageData, lastSentMessageLength);
            }   else {
                  sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
                  return;
                }
        }

        // If hello message with lower version received, sent hello with version that is lower than recent sent hello.
        if (memcmp(zrtpPoint->respondersHello->getProtocolVersion(),
                   zrtpPoint->helloMessage->getProtocolVersion(), WORD_LENGTH) < 0){

            // Create new hello with lower version.
            //uint8_t * highestVersion = zrtpPoint->findHighestVersion();
            //zrtpPoint->helloMessage->setProtocolVersion(highestVersion);
            //delete[] highestVersion;
            zrtpPoint->findHighestVersion();
            zrtpPoint->calculateMac(zrtpPoint->helloMessage->getHelloData(),
                                    zrtpPoint->helloMessage->getMessageLength(), HELLO_MESSAGE);
            setLastSentPacket(zrtpPoint->helloMessage->getHelloData(), zrtpPoint->helloMessage->getWholePacketLength());



            if (nextTimer(&T1)){
             zrtpPoint->zrtpPointCallbacks->sendData(lastSentMessageData, lastSentMessageLength);
            }   else {
                  sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
                  return;
                }
        }

        // If versions are equal, send helloAck message.
        if (memcmp(zrtpPoint->respondersHello->getProtocolVersion(),
                   zrtpPoint->helloMessage->getProtocolVersion(), WORD_LENGTH) == 0){

            zrtpPoint->zrtpPointCallbacks->stopTimer();

            // Store hash from hello message.
            zrtpPoint->setPeersHash(zrtpPoint->respondersHello->getHashImageH3(), zrtpPoint->peersH3);

            helloReceived = true;
            zrtpPoint->helloAckmessage = new HelloACKmessage();
            zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->helloAckmessage->getHelloAckData(),
                                                           HELLOACK_PACKET_SIZE);

            setState(HelloAckSent);
            std::cout << std::endl << std::endl << "## Current state: HELLO ACK SENT ##" << std::endl;
        }
    }

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "HelloACK",
        MESSAGE_TYPE_LENGTH) == 0){

        zrtpPoint->zrtpPointCallbacks->stopTimer();
        setState(HelloAckReceived);

        std::cout << std::endl << std::endl << "## Current state: HelloAck received ##" << std::endl;
    }

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Commit  ",
        MESSAGE_TYPE_LENGTH) == 0){

        // Check if we received hello message
        if(!helloReceived){
            sendErroMessage(HELLO_COMPONENTS_MISMATCH);
            return;
        }

        zrtpPoint->zrtpPointCallbacks->stopTimer();

        zrtpPoint->commitMessage = new CommitMessage();
        zrtpPoint->commitMessage->parseCommitMessage(zrtpPoint->commitMessage, stateMachineEvent->messageData);
        zrtpPoint->setPeersHash(zrtpPoint->commitMessage->getHashImageH2(), zrtpPoint->peersH2);

        // Check hello message, we have key from commit message.
        if (!(zrtpPoint->verifyMac(zrtpPoint->respondersHello->getHelloData(), zrtpPoint->respondersHello->getMessageLength(),
                                    HELLO_MESSAGE))) {

            sendErroMessage(MALFORMED_PACKET);
            return;
        }

        // Compare H2 and H3
        if (!(zrtpPoint->compareHashValues(zrtpPoint->peersH3, zrtpPoint->peersH2))){
                std::cerr << "Hash chain error !" << std::endl;
        }

        zrtpPoint->prepareDhPart1Message();
        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->dhPart1Message->getDHData(),
                                                zrtpPoint->dhPart1Message->getWholePacketLength());

        setState(WaitForDH2);
        std::cout << std::endl << std::endl << "## Current state: WAIT FOR DHPART 2 ##" << std::endl;
    }

    if (stateMachineEvent->eventType == TIME){
        if(nextTimer(&T1)){
            zrtpPoint->zrtpPointCallbacks->sendData(lastSentMessageData, lastSentMessageLength);
        }   else {
               sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
            }
    }
}

void StateMachine::handleHelloAckSentState(){

    if ( stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "HelloACK",
         MESSAGE_TYPE_LENGTH) == 0){

        // Stop hello retransmision
        setState(HelloAckReceived);
        zrtpPoint->zrtpPointCallbacks->stopTimer();

        std::cout << std::endl << std::endl << "## Current state: HELLO ACK RECEIVED ##" << std::endl;

        if (zrtpPoint->getCurrentRole() == INITIATOR){
            commitHandled = true;

            // Prepare Dhpart2 message and calculate Hvi
            zrtpPoint->dhPart2Message = new DHPart();
            zrtpPoint->prepareDhPart2Message();
            zrtpPoint->calculateHvi(zrtpPoint->respondersHello);

            zrtpPoint->commitMessage = new CommitMessage();

            // Check if we support key algorithm
            if ((currentErrorCode = zrtpPoint->algorithmNegotiation()) != N_ERROR){
                sendErroMessage(currentErrorCode);
                return;
            }

            zrtpPoint->prepareCommitMessage();
            zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->commitMessage->getCommitData(),
                                                           DH_COMMIT_PACKET_LENGTH);

            setLastSentPacket(zrtpPoint->commitMessage->getCommitData(), DH_COMMIT_PACKET_LENGTH);

            if (startTimer(&T2) == false){
                sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
                return;
            }

            setState(CommitSent);
            std::cout << std::endl << std::endl << "## Current state: COMMIT SENT ##" << std::endl;

        }   else {
                commitHandled = true;

                // Calculate secret to speed up processing
                zrtpPoint->dhPart1Message = new DHPart();
                zrtpPoint->prepareDhPart1Message();
                setState(WaitForCommit);
                std::cout << std::endl << std::endl << "## Current state: WAIT FOR COMMIT ##" << std::endl;
        }
    }

    if (stateMachineEvent->eventType == TIME){
        if(nextTimer(&T1)){
            zrtpPoint->zrtpPointCallbacks->sendData(lastSentMessageData, lastSentMessageLength);
        }   else {
               sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
            }
    }
}

void StateMachine::handleHelloAckReceivedState(){

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Hello   ",
        MESSAGE_TYPE_LENGTH) == 0){

        if (zrtpPoint->getCurrentRole() == INITIATOR && commitHandled == false){

            // Prepare Dhpart2 message and calculate Hvi
            zrtpPoint->dhPart2Message = new DHPart();
            zrtpPoint->prepareDhPart2Message();
            zrtpPoint->calculateHvi(zrtpPoint->respondersHello);

            zrtpPoint->commitMessage = new CommitMessage();

            // Check if we support key algorithm
            if ((currentErrorCode = zrtpPoint->algorithmNegotiation()) != N_ERROR){
                sendErroMessage(currentErrorCode);
                return;
            }

            zrtpPoint->prepareCommitMessage();
            zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->commitMessage->getCommitData(),
                                                           DH_COMMIT_PACKET_LENGTH);

            setLastSentPacket(zrtpPoint->commitMessage->getCommitData(), DH_COMMIT_PACKET_LENGTH);

            if (startTimer(&T2) == false){
                sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
                return;
            }

            setState(CommitSent);
            std::cout << std::endl << std::endl << "## State: COMMIT SENT ##" << std::endl;
        }
            else {
                zrtpPoint->dhPart1Message = new DHPart();
                zrtpPoint->prepareDhPart1Message();
                setState(WaitForCommit);
                std::cout << std::endl << std::endl << "## Current state:: WAIT FOR COMMIT ##" << std::endl;
            }
    }
}

void StateMachine::handleWaitForCommitState(){

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Commit  ",
                 MESSAGE_TYPE_LENGTH) == 0){

        zrtpPoint->commitMessage = new CommitMessage();
        zrtpPoint->commitMessage->parseCommitMessage(zrtpPoint->commitMessage, stateMachineEvent->messageData);
        zrtpPoint->setPeersHash(zrtpPoint->commitMessage->getHashImageH2(), zrtpPoint->peersH2);

        // Compare hello mac we have H2
        if (!(zrtpPoint->verifyMac(zrtpPoint->respondersHello->getHelloData(),
                                   zrtpPoint->respondersHello->getMessageLength(),
                                   HELLO_MESSAGE))){

            sendErroMessage(MALFORMED_PACKET);
            return;
        }

        // Compare H2 and H3
        if (!(zrtpPoint->compareHashValues(zrtpPoint->peersH3, zrtpPoint->peersH2))){
                std::cerr << "Hash chain error !" << std::endl;
        }

        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->dhPart1Message->getDHData(), zrtpPoint->dhPart1Message->getWholePacketLength());
        setState(WaitForDH2);
        std::cout << std::endl << std::endl << "## Current state: WAIT FOR DHPART 2 ##" << std::endl;
    }
}

void StateMachine::handleCommitSentState(){

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "DHPart1 ",
        MESSAGE_TYPE_LENGTH) == 0){

        zrtpPoint->zrtpPointCallbacks->stopTimer();

        // Store received Dhpar1
        zrtpPoint->dhPart1Message = new DHPart();
        zrtpPoint->dhPart1Message->setMessageType((uint8_t*) "DHPart1 ");
        zrtpPoint->dhPart1Message->parseDhMessage(zrtpPoint->dhPart1Message, stateMachineEvent->messageData);

        // Copy H1 from Dhpart
        zrtpPoint->setPeersHash(zrtpPoint->dhPart1Message->getHashImageH1(), zrtpPoint->peersH1);
        zrtpPoint->readPublicValue(zrtpPoint->dhPart1Message);
        zrtpPoint->calculateAll();

        // Calculate H2 from H1 and verify Hello of responder
        sha256(zrtpPoint->peersH1, 32, zrtpPoint->peersH2, 0);
        if (!(zrtpPoint->verifyMac(zrtpPoint->respondersHello->getHelloData(),
                                   zrtpPoint->respondersHello->getMessageLength(),
                                   HELLO_MESSAGE))){

            sendErroMessage(MALFORMED_PACKET);
            return;
        }

        // Check hash chain of calculated H2 and H3 from responders hello
        if (!(zrtpPoint->compareHashValues(zrtpPoint->peersH3, zrtpPoint->peersH2))){
            std::cerr << "Hash chain error !" << std::endl;
        }

        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->dhPart2Message->getDHData(),
                                                zrtpPoint->dhPart2Message->getWholePacketLength());

        setLastSentPacket(zrtpPoint->dhPart2Message->getDHData(), zrtpPoint->dhPart2Message->getWholePacketLength());

        setState(WaitForConfirm1);
        // We must wait in this state because peers computation in waitForConfirm1 state take too much time.
        #if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
            delay(570);
        #else  /* presume POSIX */
             delay(70);
       #endif

        if (startTimer(&T2) == false){
            sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
            return;
        }

        std::cout << std::endl << std::endl << "## Current state: WAIT FOR CONFIRM 1 ##" << std::endl;
    }

    // We can not accept commit, we support DH mode only
    if (strncmp((const char *) getReceivedMessageType(), (const char *) "Confirm1",
                 MESSAGE_TYPE_LENGTH) == 0){

        zrtpPoint->zrtpPointCallbacks->stopTimer();
        sendErroMessage(DH_MODE_REQUIRED);
        return;
    }

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Commit  ",
        MESSAGE_TYPE_LENGTH) == 0){

        // If our hvi value is lower than responder, we continue as a initiator.
        // stateMachineEvent->messageData + 88 = position of HVI in DH mode.
        if(memcmp(zrtpPoint->commitMessage->getHvi(), stateMachineEvent->messageData + 88, 32) < 0){
            // stay INITIATOR, commit is resend by timout.
            return;
        }   else {
                zrtpPoint->setRole(RESPONDER);
                setState(WaitForCommit);
            }
    }

    if (stateMachineEvent->eventType == TIME){
        if (nextTimer(&T2)){
            zrtpPoint->zrtpPointCallbacks->sendData(lastSentMessageData, lastSentMessageLength);
        }   else{
                resetTimer();
                sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
                return;
            }
    }
}

void StateMachine::handleWaitForDH2state(){

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "DHPart2 ",
                 MESSAGE_TYPE_LENGTH) == 0){

        // Store responders DhPart2 message and calculate Hvi from our Hello and received DHpar2
        zrtpPoint->dhPart2Message = new DHPart();
        zrtpPoint->dhPart2Message->setMessageType((uint8_t*) "DHPart2 ");
        zrtpPoint->dhPart2Message->parseDhMessage(zrtpPoint->dhPart2Message, stateMachineEvent->messageData);

        zrtpPoint->setPeersHash(zrtpPoint->dhPart2Message->getHashImageH1(), zrtpPoint->peersH1);

        // Compare calculated Hvi with Hvi in commit message
        zrtpPoint->calculateHvi(zrtpPoint->helloMessage);

        // Send error if Hvi is not equal
        if(memcmp(zrtpPoint->hvi, zrtpPoint->commitMessage->getHvi(), HVI_LENGTH) != 0){
           sendErroMessage(DH_ERROR_HASHED_DATA);
           return;
        }

        // We can verify MAC of commit, we have H1
        if((zrtpPoint->verifyMac(zrtpPoint->commitMessage->getCommitData(),zrtpPoint->commitMessage->getMessageLength(),
                                 COMMIT_MESSAGE)) == false){

            sendErroMessage(MALFORMED_PACKET);
            return;
        }

        // Check hash chain of H1 and H2
        if (!zrtpPoint->compareHashValues(zrtpPoint->commitMessage->getHashImageH2(), zrtpPoint->dhPart2Message->getHashImageH1())){
            std::cerr << "Hash chain error !" << std::endl;
        }

        zrtpPoint->readPublicValue(zrtpPoint->dhPart2Message);
        zrtpPoint->calculateAll();

        zrtpPoint->confirmMessage1 = new ConfirmMessage();
        zrtpPoint->prepareConfirm1Message();
        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->confirmMessage1->getConfirmData(),
                                                      CONFIRM_PACKET_LENGTH);

        setState(WaitForConfirm2);
        std::cout << std::endl << std::endl << "## Current state: WAIT FOR CONFIRM 2 ##" << std::endl;
    }
}

void StateMachine::handleWaitForConfirm1State(){

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Confirm1",
                 MESSAGE_TYPE_LENGTH) == 0){

        // STOP retransmision of dhpart2
        zrtpPoint->zrtpPointCallbacks->stopTimer();
        resetTimer();

        zrtpPoint->confirmMessage1 = new ConfirmMessage();
        zrtpPoint->confirmMessage1->parseConfirmMessage(zrtpPoint->confirmMessage1, stateMachineEvent->messageData);

        // Check confirm mac
        if (!(zrtpPoint->verifyConfirmMac(zrtpPoint->confirmMessage1))){
            sendErroMessage(AUTH_ERROR);
            return;
        }
        zrtpPoint->decryptConfirmMessage(zrtpPoint->confirmMessage1);

        // Save h0 from confirm
        zrtpPoint->setPeersHash(zrtpPoint->confirmMessage1->getHashPreimageH0(), zrtpPoint->peersH0);

        if (!(zrtpPoint->compareHashValues(zrtpPoint->peersH1, zrtpPoint->peersH0))){
                std::cerr << "Hash chain error !" << std::endl;
        }

        // We can verify DH message with H0 from confirm
        if (!(zrtpPoint->verifyMac(zrtpPoint->dhPart1Message->getDHData(),
                                   zrtpPoint->dhPart1Message->getMessageLength(),
                                   DHPART1_MESSAGE))) {

            sendErroMessage(MALFORMED_PACKET);
            return;
        }

        // Compare my h0 and responders h0 = nonce reused
        if ((memcmp(zrtpPoint->peersH0, zrtpPoint->myH0, 32) == 0)){
           sendErroMessage(NONCE_REUSE);
           return;
        }

        zrtpPoint->confirmMessage2 = new ConfirmMessage();
        zrtpPoint->prepareConfirm2Message();

        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->confirmMessage2->getConfirmData(),
                                                CONFIRM_PACKET_LENGTH);

        setLastSentPacket(zrtpPoint->confirmMessage2->getConfirmData(), CONFIRM_PACKET_LENGTH);

        if (startTimer(&T2) == false){
            sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
            return;
        }

        setState(WaitForConfAck);
        std::cout << std::endl << std::endl << "## Current state: WAIT FOR CONFACK ##" << std::endl;
    }


    if (stateMachineEvent->eventType == TIME){
        if (nextTimer(&T2)){
            zrtpPoint->zrtpPointCallbacks->sendData(lastSentMessageData, lastSentMessageLength);
        }   else{
                resetTimer();
                sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
                return;
            }
    }
}

void StateMachine::handleWaitForConfirm2State(){

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Confirm2",
                 MESSAGE_TYPE_LENGTH) == 0){

        zrtpPoint->confirmMessage2 = new ConfirmMessage();
        zrtpPoint->confirmMessage2->parseConfirmMessage(zrtpPoint->confirmMessage2, stateMachineEvent->messageData);

        if (!(zrtpPoint->verifyConfirmMac(zrtpPoint->confirmMessage2))){
            sendErroMessage(AUTH_ERROR);
            return;
        }

        zrtpPoint->decryptConfirmMessage(zrtpPoint->confirmMessage2);
        zrtpPoint->setPeersHash(zrtpPoint->confirmMessage2->getHashPreimageH0(), zrtpPoint->peersH0);

        if (!(zrtpPoint->compareHashValues(zrtpPoint->peersH1, zrtpPoint->peersH0))){
                std::cerr << "Hash chain error !" << std::endl;
        }

        if(!(zrtpPoint->verifyMac(zrtpPoint->dhPart2Message->getDHData(),
                                  zrtpPoint->dhPart2Message->getMessageLength(),
                                  DHPART2_MESSAGE))){

            sendErroMessage(MALFORMED_PACKET);
            return;
        }

        // Compare my h0 and responders h0 = nonce reused
        if ((memcmp(zrtpPoint->peersH0, zrtpPoint->myH0, 32) == 0)){
           sendErroMessage(NONCE_REUSE);
           return;
        }

        zrtpPoint->conf2AckMessage = new Conf2AckMessage();
        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->conf2AckMessage->getConf2AckData(),
                                                CONF2ACK_PAKET_SIZE);

        setState(SecuredState);        
        std::cout  << std::endl << std::endl << "## Current state: SECURED STATE ##" << std::endl;
        zrtpPoint->writeOutKeys();
    }
}

void StateMachine::handleWaitForConfAckState(){

     if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "Conf2ACK",
         MESSAGE_TYPE_LENGTH) == 0){

         zrtpPoint->zrtpPointCallbacks->stopTimer();
         resetTimer();

         setState(SecuredState);         
         std::cout << std::endl << "## Current state: SECURED STATE ##" << std::endl;
         zrtpPoint->writeOutKeys();
     }

     if (stateMachineEvent->eventType == TIME){
         if (nextTimer(&T2)){
             zrtpPoint->zrtpPointCallbacks->sendData(lastSentMessageData, lastSentMessageLength);
         }   else{
                 resetTimer();
                 sendErroMessage(PROTOCOL_TIMEOUT_ERROR);
                 return;
             }
     }
}

void StateMachine::handleWaitForErrorAckState(){

   if (stateMachineEvent->eventType == TIME){
       if(nextTimer(&T2)){
           zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->errorMessage->getErrorData(),
                                                          ERROR_MESSAGE_LENGTH);
       }   else {
               zrtpPoint->errorMessage->setErrorCode(PROTOCOL_TIMEOUT_ERROR);
               zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->errorMessage->getErrorData(),
                                              ERROR_MESSAGE_LENGTH);;
               setState(InitialState);
               std::cout << std::endl << "## Current state: INITIAL ##" << std::endl;
               return;
       }
   }

    if (stateMachineEvent->eventType == MESSAGE && strncmp((const char *) getReceivedMessageType(), (const char *) "ErrorACK",
        MESSAGE_TYPE_LENGTH) == 0){

            zrtpPoint->zrtpPointCallbacks->stopTimer();
            setState(InitialState);
            std::cout << std::endl << "## Current state: INITIAL ##" << std::endl;
            return;
     }
}

void StateMachine::handleSecuredState(){

}

void StateMachine::sendErroMessage(zrtpErrorCode _errorCode){

        currentErrorCode = _errorCode;
        zrtpPoint->errorMessage = new ErrorMessage(currentErrorCode);

        // Set to 0 (causes conditional jump error)
        memset(receivedMessageType, 0, 8);

        zrtpPoint->zrtpPointCallbacks->sendData(zrtpPoint->errorMessage->getErrorData(),
                                                  ERROR_MESSAGE_LENGTH);

        setState(WaitForErrorAck);

        if (startTimer(&T2) == false){
            std::cerr << "ERROR timer failed while error sending" << endl;
        }

    std::cout << std::endl << "## Current state: WAIT FOR ERROR ACK ##" << std::endl;
}

