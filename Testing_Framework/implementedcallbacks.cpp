#include "implementedcallbacks.h"

ImplementedCallbacks::ImplementedCallbacks(NetworkHandler *_networkHandler){
    cbNetHandler = _networkHandler;
}

ImplementedCallbacks::~ImplementedCallbacks(){
    delete cbNetHandler;
}

bool ImplementedCallbacks::sendData(const unsigned char *message, unsigned int length){

    QByteArray dataToSend = QByteArray((char*) message, length);

    std::cout << "Sent message: ";
    for(int i = 16; i < 24; i++){
       std::cout << *(message + i);
    }

    // If the datagram is too large, this function will return -1
    if ( cbNetHandler->sendingSocket->writeDatagram(dataToSend, cbNetHandler->currentAddresses->sendingIPAdress,
                                                    cbNetHandler->currentAddresses->sendingPort) == -1){

        return false;
    }
    return true;
}

bool ImplementedCallbacks::startTimer(int time){
    cbNetHandler->timer->start(time);
    return true;
}

bool ImplementedCallbacks::stopTimer(){
    cbNetHandler->timer->stop();
    return true;
}

void ImplementedCallbacks::enterCriticalSection(){
    cbNetHandler->mutex->lock();
}

void ImplementedCallbacks::leaveCriticalSection(){
    cbNetHandler->mutex->unlock();
}

void ImplementedCallbacks::keyNegotitationEnded(){
    cbNetHandler->endAplication();
}

