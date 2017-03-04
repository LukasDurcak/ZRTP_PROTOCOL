#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QCoreApplication>
#include <QSystemSemaphore>
#include <QMutex>
#include <exception>
#include "zrtppoint.h"
#include "implementedcallbacks.h"
#include "processmessagethread.h"
#include "processtimeoutthread.h"

class ImplementedCallbacks;

typedef struct {
    quint16 sendingPort;
    quint16 receivingPort;
    QHostAddress sendingIPAdress;
    QHostAddress receivingIPAdress;
}Addresses;

class NetworkHandler : public QObject
{
    Q_OBJECT

    friend class ImplementedCallbacks;

public:

    // Parameter role 0 for INITIATOR / 1 for RESPONDER
    NetworkHandler(char * argv[], QObject *parent = 0);

    ~NetworkHandler();

signals:
    
public slots:

    void readPendingDatagrams();

    void parseInputArguments(char * argv[]);

    void processTimeOut();

    void processMessage(uint8_t* data, quint16 size);

    void processTimeoutInZrtp();

    void startZrtpNegotiation();

    // run negotiation again if user set it
    void endAplication();

    void eraseThreads();

    void setAddresses(Addresses * _addresses, quint16 _sendingPort, QHostAddress _sendingIP,
                      quint16 _receivingPort, QHostAddress _receivingIP);

private:

    int applicationRunCounter = 0;
    bool writeToFile = false;
    fstream outputFile;

    QUdpSocket *receivingSocket;
    QUdpSocket *sendingSocket;
    ZrtpPoint* zrtpPoint;
    Callbacks* callbacks;
    Addresses* currentAddresses;

    QTimer* timer;
    QMutex* mutex;

    QElapsedTimer elapsedTimer;

    role currentRole;

    std::vector<QThread*> myThreads;
    std::vector<ZrtpPoint*> zrtpPointVector;
};

#endif // NETWORKHANDLER_H
