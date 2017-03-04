#ifndef MESSAGES_H
#define MESSAGES_H

#include "zrtpPacket.h"
#include "CommitMessage.h"
#include "helloAckMessage.h"
#include "hellomessage.h"
#include "dhpart.h"
#include "confirmmessage.h"
#include "conf2ackmessage.h"
#include "errorackmessage.h"
#include "errormessage.h"

enum MESSAGE_TYPE {
    HELLO_MESSAGE,
    HELLO_ACK_MESSAGE,
    COMMIT_MESSAGE,
    DHPART1_MESSAGE,
    DHPART2_MESSAGE,
    CONFIRM1_MESSAGE,
    CONFIRM2_MESSAGE,
    CONF_ACK_MESSAGE,
    ERROR_MESSAGE,
    ERROR_ACK_MESSAGE,
    UNKNOWN_MESSAGE
};

#endif // MESSAGES_H
