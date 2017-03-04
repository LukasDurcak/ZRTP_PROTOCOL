#ifndef ZRTPMESSAGE_H
#define ZRTPMESSAGE_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include "errorCodes.h"

// One word length in this imlementation is 4 bytes
#define WORD_LENGTH 4
#define MESSAGE_TYPE_LENGTH 8

// Include sizeof sequence number, magic cookie and source identifier.
// We need this to skip packet head and get pointer directly to message data.
#define PACKET_HEAD_LENGTH 12

// Include length of preamble, length and message type.
#define MESSAGE_HEAD_LENGTH 12

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

/**
 * @brief The ZrtpMessage class is class for message head.
 */
class ZrtpMessageHeader {

    const uint16_t preamble = 0x505a;

    uint16_t messageLength;

    uint8_t messageType[MESSAGE_TYPE_LENGTH];

public:

    /**
     * @brief ZrtpMessageHeader constructor for this class.
     */
    ZrtpMessageHeader();

    /**
     * @brief ~ZrtpMessageHeader() destructor.
     */
    ~ZrtpMessageHeader();

    /**
     * @brief setMessageLength setter for length.
     * @param _length length to set.
     */
    void setMessageLength(const uint16_t _length) {messageLength = _length;}

    /**
     * @brief setMessageType setter for param message Type.
     * @param _messageType is message type.
     */
    void setMessageType (uint8_t * _message) { memcpy(messageType, _message, MESSAGE_TYPE_LENGTH);}

    /**
     * @brief getMessageType is getter for param message type.
     * @return message type.
     */
    uint8_t * getMessageType() {return messageType;}

    /**
     * @brief getMessageLength is getter for param length.
     * @return length of message.
     */
    uint16_t getMessageLength() {return messageLength;}

    /**
     * @brief initializeMessageHead copy message head to data.
     * @param _data
     */
    void initializeMessageHead(uint8_t* _data);
};


#endif // ZRTPMESSAGE_H
