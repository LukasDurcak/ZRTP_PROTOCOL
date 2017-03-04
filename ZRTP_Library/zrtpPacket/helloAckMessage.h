#ifndef HELLOACKMESSAGE_H
#define HELLOACKMESSAGE_H

#include "zrtpPacket.h"

#define HELLOACK_PACKET_SIZE 28

/**
 * @brief The HelloACKmessage class represent HelloACK message.
 */
class HelloACKmessage : public ZrtpPacket{

    uint8_t dataToSend[HELLOACK_PACKET_SIZE];

public:

    /**
     * @brief HelloACKmessage constructor for class helloAck message.
     */
    HelloACKmessage();

    /**
     * @brief ~HelloAckmessage destructor for class helloAck message.
     */
    ~HelloACKmessage();

    /**
     * @brief getHelloAckData getter for helloAck message data.
     * @return data.
     */
    uint8_t * getHelloAckData() {return dataToSend;}

    /**
     * @brief parseHelloAck parse and check received hello ack message
     * @return error code if something went wrong, 0 otherwise.
     */
    zrtpErrorCode parseHelloAck(uint8_t* _message);

    virtual void initializeMessageData();
};

#endif // HELLOACKMESSAGE_H
