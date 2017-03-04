
#include "processmessagethread.h"

ProcessMessageThread::ProcessMessageThread(NetworkHandler* _networkHandler, unsigned char *_data, unsigned int _length){

    networkHandler = _networkHandler;
    messageData = _data;
    length = _length;
}

void ProcessMessageThread::run(){
    networkHandler->processMessage(messageData, length);
}

ProcessMessageThread::~ProcessMessageThread(){
    delete[] messageData;
}
