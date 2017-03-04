#ifndef PROCESSTIMEOUTTHREAD_H
#define PROCESSTIMEOUTTHREAD_H
#include <QtCore>
#include "networkhandler.h"

class NetworkHandler;

class ProcessTimeoutThread : public QThread {

public:
    ProcessTimeoutThread(NetworkHandler* _networkHandler);

    void run();

private:
    NetworkHandler * networkHandler;
};

#endif // PROCESSTIMEOUTTHREAD_H
