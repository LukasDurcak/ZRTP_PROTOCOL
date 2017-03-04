#ifndef CONF2ACKMESSAGE_H
#define CONF2ACKMESSAGE_H

#include "zrtpPacket.h"

// Conf2Ack packet is always 28
#define CONF2ACK_PAKET_SIZE 28

/**
 * @brief The conf2AckMessage class represent conf2AckMessage
 */
class Conf2AckMessage : public ZrtpPacket{

    uint8_t dataToSend[CONF2ACK_PAKET_SIZE];

public:

    /**
     * @brief conf2AckMessage constructor for class conf2AckMessage.
     */
    Conf2AckMessage();

    /**
     * @brief conf2AckMessage destructor for class conf2AckMessage.
     */
    ~Conf2AckMessage();

    /**
     * @brief getConf2AckData getter for conf2AckData.
     * @return dataToSend.
     */
    uint8_t * getConf2AckData() {return dataToSend;}

    virtual void initializeMessageData();
};

#endif // CONF2ACKMESSAGE_H
