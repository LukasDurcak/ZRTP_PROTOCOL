#ifndef ZRTPPACKET_H
#define ZRTPPACKET_H

#include "zrtpMessageHeader.h"

#define HASH_LENGTH_SHA256 32
#define ZID_LENGTH 12
#define MAC_LENGTH 8
#define PACKET_WITHOUT_MESSAGE_LENGTH 16

/**
 * @brief The ZrtpPacket class is base class for all ZRTP messages.
 */
class ZrtpPacket{

    // The 0001 and 12 zero bits at begining of packet.
    const uint16_t packetHead = 0x1000;

    //The Sequence Number is a count that is incremented for each ZRTP packet sent.
    uint16_t sequenceNumber;

    // Magic cookie ZRTP
    uint8_t magicCookie[4];

    // Source Identifier is the SSRC number of the RTP stream to which this ZRTP packet relates.
    uint32_t sourceId;

    // Header of message.
    ZrtpMessageHeader* messageHeader;

protected:

    // The CRC is calculated across the entire ZRTP packet.
    // We Do not calculate crc in this implementation, so it is set to default value in constructor.
    uint32_t crc;

public:

    /**
     * @brief ZrtpPacket constructor for zrtp packet.
     */
    ZrtpPacket();

    /**
     * @brief ~ZrtpPacket destructor for this class.
     */
    virtual ~ZrtpPacket();

    /**
     * @brief getMagicCookie is getter for ZRTP magic cookie.
     * @return magic cookie.
     */
    const uint8_t * getMagicCookie() const {return magicCookie;}

    /**
     * @brief getHead is getter for head of packet.
     * @return head.
     */
    uint16_t getHead() const {return packetHead;}

    /**
     * @brief getSequenceNumber is getter for parameter sequence number
     * @return sequence number of current packet.
     */
    uint32_t getSequenceNumber() const {return sequenceNumber;}

    /**
     * @brief getSourceIdentifier is getter for parameter sourceID.
     * @return source identifier.
     */
    uint32_t getSourceIdentifier() const {return sourceId;}

    /**
     * @brief setSequenceNumber is setter for parameter sequence number.
     * @param _sequenceNumber is new sequence number to set.
     */
    void setSequenceNumber(uint16_t _sequenceNumber){sequenceNumber = _sequenceNumber;}

    /**
     * @brief setSourceID is setter for parameter sourceID.
     * @param _sourceID new source ID.
     */
    void setSourceID(uint32_t _sourceID) {sourceId = _sourceID;}

    /**
     * @brief setMessageLength setter for message length.
     * @param _length new length to set.
     */
    void setMessageLength(uint16_t _length) {messageHeader->setMessageLength(_length);}

    /**
     * @brief setMessageType setter for message type.
     * @param _messageType new message type to set.
     */
    void setMessageType(uint8_t * _messageType) {messageHeader->setMessageType(_messageType);}

    /**
     * @brief getMessageType getter for message type.
     * @return type of message.
     */
    uint8_t* getMessageType() {return messageHeader->getMessageType();}

    /**
     * @brief getMessageLength getter for message length.
     * @return length of message in  in WORDS.
     */
    uint16_t getMessageLength() {return messageHeader->getMessageLength() * WORD_LENGTH ;}

    /**
     * @brief getWholePacketLength return length of whole packet.
     * @return packet length.
     */
    uint16_t getWholePacketLength(){return (getMessageLength() + PACKET_WITHOUT_MESSAGE_LENGTH);}

    /**
     * @brief initializeMessageHead initialize message head.
     * @param _data data to be initialized.
     */
    void initializeMessageHead(uint8_t* _data) {messageHeader->initializeMessageHead(_data);}

    /**
     * @brief initializeHead copy head to message`s data.
     * @param _data
     */
    void initializePacketData(uint8_t* _data);

    /**
     * @brief initializeMessageData function copy all message values to byte array.
     */
    virtual void initializeMessageData() = 0;
};

#endif // ZRTPPACKET_H
