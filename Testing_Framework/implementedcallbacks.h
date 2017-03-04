#ifndef IMPLEMENTEDCALLBACKS_H
#define IMPLEMENTEDCALLBACKS_H

#include <QObject>
#include "callbacks.h"
#include "networkhandler.h"

class NetworkHandler;

class ImplementedCallbacks : public Callbacks{

private:
    NetworkHandler* cbNetHandler;

public:
    ImplementedCallbacks(NetworkHandler* _networkHandler);

    ~ImplementedCallbacks();

    virtual bool sendData(const unsigned char* message, unsigned int length);

    virtual bool startTimer(int time);

    virtual bool stopTimer();

    virtual void keyNegotitationEnded();

    // Critical section handling

    virtual void enterCriticalSection();

    virtual void leaveCriticalSection();

};

#endif // IMPLEMENTEDCALLBACKS_H
