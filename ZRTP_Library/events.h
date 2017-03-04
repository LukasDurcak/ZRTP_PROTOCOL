#ifndef EVENTS_H
#define EVENTS_H

// Struct represent Event types
typedef enum _ZrtpEventType{
    START,
    STOP,
    MESSAGE,
    TIME
} ZrtpEventType;

typedef struct{
    ZrtpEventType eventType;
    unsigned char* messageData;
    int messageDataLength;
} ZrtpEvent;

#endif // EVENTS_H
