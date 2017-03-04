#ifndef ERRORMESSAGE_H
#define ERRORMESSAGE_H

#include "zrtpPacket.h"

#define ERROR_MESSAGE_LENGTH 32

class ErrorMessage : public ZrtpPacket{

    uint8_t dataToSend[ERROR_MESSAGE_LENGTH];

    zrtpErrorCode currentErrorCode;

public:

    /**
     * @brief ErrorMessage constructor for error message.
     */
    ErrorMessage(zrtpErrorCode _errorCode);

    /**
     * @brief ErrorMessage destructor for error message.
     */
    ~ErrorMessage();

    /**
     * @brief getErrorData getter for errorData.
     * @return error message data.
     */
    uint8_t * getErrorData(){return dataToSend;}

    /**
     * @brief setErrorCode setter for error code.
     * @param _currentErrorCode code to set.
     */
    void setErrorCode (zrtpErrorCode _currentErrorCode);

    /**
     * @brief getErrorCode getter for parameter error code.
     * @return current error code.
     */
    zrtpErrorCode getErrorCode () {return currentErrorCode;}

    virtual void initializeMessageData();
};

#endif // ERRORMESSAGE_H
