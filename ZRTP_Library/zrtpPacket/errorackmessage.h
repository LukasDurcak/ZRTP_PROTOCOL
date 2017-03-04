#ifndef ERRORACKMESSAGE_H
#define ERRORACKMESSAGE_H

#include "zrtpPacket.h"

#define ERRORACK_PACKET_SIZE 28

class ErrorAckMessage : public ZrtpPacket{

    uint8_t dataToSend[ERRORACK_PACKET_SIZE];

public:

    /**
     * @brief ErrorAckMessage constructor for error Ack message.
     */
    ErrorAckMessage();

    ~ErrorAckMessage();

    /**
     * @brief getErrorAckData getter for errorAck data.
     * @return data.
     */
    uint8_t * getErrorAckData(){return dataToSend;}

    virtual void initializeMessageData();
};

#endif // ERRORACKMESSAGE_H
