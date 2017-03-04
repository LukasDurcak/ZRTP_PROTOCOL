#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)

  #include <windows.h>

  inline void delay( unsigned long ms )
    {
    Sleep( ms );
    }

#else  /* presume POSIX */

  #include <unistd.h>

  inline void delay( unsigned long ms )
    {
    usleep( ms * 1000 );
    }

#endif

#include "zrtppoint.h"
#include "events.h"
#include <map>
#include <ctime>

class StateMachine;
class ZrtpPoint;

// Enum that represet ZRTP states.
typedef enum _ZrtpStates{
    InitialState,
    HelloSent,
    HelloAckSent,
    HelloAckReceived,
    WaitForHelloAck,
    CommitSent,
    WaitForCommit,
    WaitForDH2,
    WaitForConfirm1,
    WaitForConfirm2,
    WaitForConfAck,
    WaitForErrorAck,
    SecuredState
} ZrtpStates;

// Struct that represent Zrtp Timers
typedef struct _ZrtpTimer{
     uint16_t startTime;
     uint16_t actualTime;
     uint16_t capping;
     uint16_t maxResend;
     uint16_t timeoutsCounter;
} ZrtpTimer;

typedef void (StateMachine::* handler)(void);
typedef std::map<ZrtpStates, handler> stateMap;

class StateMachine{

private:

    ZrtpPoint* zrtpPoint;
    ZrtpEvent* stateMachineEvent;
    ZrtpStates currentState;
    stateMap currentStateMap;

    ZrtpTimer T1;
    ZrtpTimer T2;

    zrtpErrorCode currentErrorCode;
    uint8_t receivedMessageType[8];

    // Store resending packet
    uint8_t lastSentMessageData[500];
    uint16_t lastSentMessageLength;

    bool helloReceived;
    bool commitHandled;

public:

    /**
     * @brief StateMachine constructor for Statemachine class.
     * @param _zrtpPoint is Zrtp point, which will be controled by this State machine.
     */
    StateMachine(ZrtpPoint*_zrtpPoint); 

    /**
     * @brief ~StateMachine() is destructor for this class.
     */
    ~StateMachine();

    /**
     * @brief initTimers set values to Timer T1 and T2.
     */
    void initTimers();

    /**
     * @brief processEvent is ,,basic" function in this class. It`s responsible for
     *        all events, which come from outside.
     * @param _event we want to handle.
     */
    void processEvent(ZrtpEvent* _event);

    /**
     * @brief handleInitialState
     */
    void handleInitialState();

    /**
     * @brief handleHelloSentState
     */
    void handleHelloSentState();

    /**
     * @brief handleHelloAckSentState
     */
    void handleHelloAckSentState();

    /**
     * @brief handleHelloAckReceivedState
     */
    void handleHelloAckReceivedState();

    /**
     * @brief handleCommitSentState
     */
    void handleCommitSentState();

    /**
     * @brief handleWaitForCommitState
     */
    void handleWaitForCommitState();

    /**
     * @brief handleWaitForDH2state
     */
    void handleWaitForDH2state();

    /**
     * @brief handleWaitForConfirm1State
     */
    void handleWaitForConfirm1State();

    /**
     * @brief handleWaitForConfirm2State
     */
    void handleWaitForConfirm2State();

    /**
     * @brief handleWaitForConfAckState
     */
    void handleWaitForConfAckState();

    /**
     * @brief handleWaitForErrorAckState
     */
    void handleWaitForErrorAckState();

    /**
     * @brief handleSecuredState
     */
    void handleSecuredState();

    /**
     * @brief setState setter for state.
     * @param _state to set.
     */
    void setState(ZrtpStates _state){currentState = _state;}

    /**
     * @brief getCurrentState getter for actual state.
     * @return actual state.
     */
    ZrtpStates getCurrentState(){return currentState;}

    /**
     * @brief setErrorCode setter for error code.
     * @param _errorCode to set.
     */
    void setErrorCode(zrtpErrorCode _errorCode) {currentErrorCode = _errorCode;}

    /**
     * @brief getErrorCode getter for error code.
     * @return error code.
     */
    uint32_t getErrorCode(){return currentErrorCode;}

    /**
     * @brief setReceivedMessageType setter for received message.
     * @param _messageType to set.
     */
    void setReceivedMessageType(uint8_t* _messageType) {memcpy(receivedMessageType, _messageType,
                                                        MESSAGE_TYPE_LENGTH);}

    /**
     * @brief getReceivedMessageType getter for received message type.
     * @return type of received message.
     */
    uint8_t* getReceivedMessageType(){return receivedMessageType;}

    /**
     * @brief sendErroMessage send error message with occured error code and put engine to WaitForErrorAck state.
     * @param _errorCode.
     */
    void sendErroMessage(zrtpErrorCode _errorCode);

    /**
     * @brief startTimer start given timer.
     * @param _timer to be start.
     * @return  true if timer started, false otherwise.
     */
    bool startTimer(ZrtpTimer* _timer);

    /**
     * @brief nextTimer start next timer value.
     * @param _timer to be start.
     * @return true if timer count is > 0, false otherwise.
     */
    bool nextTimer(ZrtpTimer* _timer);

    /**
     * @brief resetTimers reset timer 2 values to initial.
     */
    void resetTimer();

    /**
     * @brief setLastSentPacket setter for last message data, also set length.
     * @param _data of last sent message.
     * @param _lengt of last sent message.
     */
    void setLastSentPacket(uint8_t* _data, uint16_t _length) {memcpy(lastSentMessageData, _data, _length);
                                                                     lastSentMessageLength = _length;}
};

#endif // STATEMACHINE_H
