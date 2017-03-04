#include "networkhandler.h"

QSystemSemaphore *semaphore1;
QSystemSemaphore *semaphore2;

NetworkHandler::NetworkHandler(char *argv[], QObject *parent) :
    QObject(parent) {

    parseInputArguments(argv);

    semaphore1 = new QSystemSemaphore("semaphore1", 0, QSystemSemaphore::Open);
    semaphore2 = new QSystemSemaphore("semaphore2", 0, QSystemSemaphore::Open);

    sendingSocket = new QUdpSocket(this);
    receivingSocket = new QUdpSocket(this);
    mutex = new QMutex();
    currentAddresses = new Addresses;    
    myThreads.reserve(20);
    zrtpPointVector.reserve(applicationRunCounter);

    // Initiator send to 1234 and receive on 1244
    // Responder send to 1244 and receive on 1234
    (currentRole == INITIATOR) ? setAddresses(currentAddresses, 1234, QHostAddress::LocalHost, 1244, QHostAddress::LocalHost):
                                 setAddresses(currentAddresses, 1244, QHostAddress::LocalHost, 1234, QHostAddress::LocalHost);


    timer = new QTimer();
    timer->setSingleShot(true);

    receivingSocket->bind(currentAddresses->receivingIPAdress, currentAddresses->receivingPort);

    connect(receivingSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    connect(timer,SIGNAL(timeout()), this, SLOT(processTimeOut()));

    (currentRole == INITIATOR) ? semaphore1->release() : semaphore2->release();
    (currentRole == INITIATOR) ? semaphore2->acquire() : semaphore1->acquire();

    elapsedTimer.start();
}

NetworkHandler::~NetworkHandler(){

    delete (receivingSocket);
    delete (sendingSocket);
    delete (zrtpPoint);
    delete (callbacks);
    delete (currentAddresses);

    delete(timer);
    delete(mutex);

    delete(semaphore1);
    delete(semaphore2);

    eraseThreads();

    ZrtpPoint* pointToDelete;
    for(std::vector<ZrtpPoint*>::iterator it = zrtpPointVector.begin(); it != zrtpPointVector.end(); ++it){
        pointToDelete = *it;
        delete  pointToDelete;
    }
}

void NetworkHandler::setAddresses(Addresses *_addresses, quint16 _sendingPort,
                                  QHostAddress _sendingIP, quint16 _receivingPort,
                                  QHostAddress _receivingIP){

    _addresses->sendingPort = _sendingPort;
    _addresses->sendingIPAdress = _sendingIP;
    _addresses->receivingPort = _receivingPort;
    _addresses->receivingIPAdress = _receivingIP;
}

void NetworkHandler::readPendingDatagrams(){
    QByteArray Buffer;
    Buffer.resize(receivingSocket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    receivingSocket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);

    if (writeOut){
         qDebug() << "Message from: " << sender.toString();
         qDebug() << "Message port: " << senderPort;
         qDebug() << "Message size: " << Buffer.size();
         qDebug() << "Message ";
         for(int i = 0; i < Buffer.size(); i++){
             cout << Buffer[i];
         }
    }   else {
        std::cout << "Received message :";
        for(int i = 16; i < 24; i++){
           std::cout << Buffer[i];
        }
        std::cout<<std::endl;
     }

    uint8_t *message = new uint8_t[Buffer.size()];
    memcpy(message,Buffer.data(),Buffer.size());
    ProcessMessageThread * processMessageThread = new ProcessMessageThread(this, (uint8_t*) Buffer.data(), (quint16) Buffer.size());
    myThreads.push_back(processMessageThread);
    processMessageThread->run();
    delete[] message;
}

void NetworkHandler::parseInputArguments(char *argv[]){

    if (strcmp(argv[1], "initiator") == 0){
        currentRole = INITIATOR;
    }   else {
            currentRole = RESPONDER;
        }

    (currentRole == INITIATOR) ? (qDebug() << "           Role: INITIATOR") : (qDebug() << "           Role: RESPONDER");
    callbacks = new ImplementedCallbacks(this);
    zrtpPoint = new ZrtpPoint(currentRole, callbacks);


    if (strcmp(argv[2],"version") == 0){
        applicationRunCounter = 0;
        if (currentRole == INITIATOR){
            zrtpPoint->addSupported((const char*) "2.00", 1);
        }   else {
            zrtpPoint->addSupported((const char*) "1.40", 1);
        }
    }

    if (strcmp(argv[2],"test") == 0){
        applicationRunCounter = atoi(argv[3]);
    }

    if (strcmp(argv[2],"algorithm") == 0){
        applicationRunCounter = 0;
        if (currentRole == INITIATOR){
            zrtpPoint->addSupported((const char*) "DH2k", 5);
            zrtpPoint->addSupported((const char*) "DH3k", 5);
            zrtpPoint->addSupported((const char*) "EC25", 5);
        }   else {
                zrtpPoint->addSupported((const char*) "EC38", 5);
                zrtpPoint->addSupported((const char*) "EC25", 5);
                zrtpPoint->addSupported((const char*) "DH3k", 5);
            }
    }

    if (*argv[4] == '1'){
        writeOut = true;
    }
}

void NetworkHandler::processMessage(uint8_t *data, quint16 size){
    zrtpPoint->processMessage((uint8_t*) data,  (quint16) size);
}

void NetworkHandler::processTimeOut(){

    ProcessTimeoutThread * processTimeoutThread = new ProcessTimeoutThread(this);
   myThreads.push_back(processTimeoutThread);
   processTimeoutThread->run();
}

void NetworkHandler::processTimeoutInZrtp(){
   zrtpPoint->processTimeout();
}

void NetworkHandler::startZrtpNegotiation(){

    elapsedTimer.start();
    zrtpPoint->startEngine();
}

void NetworkHandler::eraseThreads(){

    for(std::vector<QThread*>::iterator it = myThreads.end(); it != myThreads.begin(); --it){
       QThread * threadToDelete = *it;
       threadToDelete->wait();
       threadToDelete->quit();
       delete *it;
       myThreads.erase(it);
    }
}

void NetworkHandler::endAplication() {

    cout << endl;
    cout << "Aplication time : ";
    cout << elapsedTimer.elapsed() << endl;

    if (applicationRunCounter > 0) {
        applicationRunCounter --;

        qDebug() ;
        qDebug() << "**************************************************" ;
        qDebug() << "               Running again";
        qDebug() << "**************************************************" ;
        qDebug();

        zrtpPointVector.push_back(zrtpPoint);
        zrtpPoint = new ZrtpPoint(currentRole, callbacks);

        (currentRole == INITIATOR) ? semaphore1->release() : semaphore2->release();
        (currentRole == INITIATOR) ? semaphore2->acquire() : semaphore1->acquire();
         zrtpPoint->startEngine();
    }   else {
            //outputFile.close();
            cout << "Application has ended you can close it " << endl;
        }
}

