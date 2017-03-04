#include "zrtpPacket/messages.h"

MESSAGE_TYPE decryptMessage(const char *data){
    char messageType[8];

    memcpy(messageType, data + 16, 8);

    std::string messageTypeString (messageType);

    //std::cout << messageTypeString << std::endl;

    if (messageTypeString.compare("Hello   ") == 0){
        //std::cout << "HELLO SPRAVA PRIJATA !!!! " << std::endl;
        return HELLO_MESSAGE;
    }

    if (messageTypeString.compare("HelloACK") == 0){
        //std::cout << "HELLO ACK SPRAVA PRIJATA !!!! " << std::endl;
        return HELLO_ACK_MESSAGE;
    }

    if (messageTypeString.compare("Commit  ") == 0){
        //std::cout << "COMMIT SPRAVA PRIJATA !!!! " << std::endl;
        return COMMIT_MESSAGE;
    }

    if (messageTypeString.compare("DHPart1 ") == 0){
        //std::cout << "DHPART1 SPRAVA PRIJATA !!!! " << std::endl;
        return DHPART1_MESSAGE;
    }

    if (messageTypeString.compare("DHPart2 ") == 0){
        //std::cout << "DHPART2 SPRAVA PRIJATA !!!! " << std::endl;
        return DHPART2_MESSAGE;
    }   else {
            //std::cout << "NEZNAMA SPRAVA PRIJATA !!!! " << std::endl;
            return UNKNOWN_MESSAGE;
        }
}
