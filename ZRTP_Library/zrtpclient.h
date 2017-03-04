#ifndef ZRTPCLIENT_H
#define ZRTPCLIENT_H

#include "zrtpPacket/messages.h"

class ZRTPclient
{
private:
    typedef struct secrets{
        char rs1 = 6;
        char rs2 = 13;
        char auxSecret = 29;
        char pbxSecret = 4;
    }sharedSecrets;

public:
    ZRTPclient();
};

#endif // ZRTPCLIENT_H
