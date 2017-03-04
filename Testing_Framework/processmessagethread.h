#ifndef PROCESSMESSAGETHREAD_H
#define PROCESSMESSAGETHREAD_H

#include <QtCore>
#include <QMutex>
#include <networkhandler.h>

class NetworkHandler;

class ProcessMessageThread : public QThread {

public:
    ProcessMessageThread(NetworkHandler* _networkHandler, unsigned char* _data, unsigned int _length);

    ~ProcessMessageThread();

    void run();

private:
    NetworkHandler* networkHandler;
    unsigned char* messageData;
    unsigned int length;
};

#endif // PROCESSMESSAGETHREAD_H

