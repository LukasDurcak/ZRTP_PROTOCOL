#include "processtimeoutthread.h"

ProcessTimeoutThread::ProcessTimeoutThread(NetworkHandler* _networkHandler){

    networkHandler = _networkHandler;
}

void ProcessTimeoutThread::run(){

    networkHandler->processTimeoutInZrtp();
}
