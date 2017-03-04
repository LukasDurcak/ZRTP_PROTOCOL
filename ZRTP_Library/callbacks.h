#ifndef CALLBACKS_H
#define CALLBACKS_H

/**
 * @brief The Callbacks class represend callbacks function which user must implement.
 */
class Callbacks{

public:

    /**
     * @brief Callbacks constructor for class callbacks.
     */
    Callbacks();

    /**
     * @brief Callbacks destructor for class callbacks.
     */
    virtual ~Callbacks();

    /**
     * @brief sendData function that send data to another endpoint.
     * @param message to send.
     * @param length of sended message.
     * @return true if succesfull false otherwise.
     */
    virtual bool sendData(const unsigned char* message, unsigned int length) = 0;

    /**
     * @brief startTimer start implemented timer.
     * @param time in milisecond.
     * @return true if succesfull false otherwise.
     */
    virtual bool startTimer(int time) = 0;

    /**
     * @brief stopTimer stop implemented timer.
     * @return true if succesfull false otherwise.
     */
    virtual bool stopTimer() = 0;

    /**
     * @brief keyNegotitationEnded this function is called when key negotiation is finished.
     */
    virtual void keyNegotitationEnded() = 0;


    // Critical Section handling
    /**
     * @brief enterCriticalSection this function should lock the mutex in your implementation.
     */
    virtual void enterCriticalSection() = 0;

    /**
     * @brief enterCriticalSection this function should unlock the mutex in your implementation.
     */
    virtual void leaveCriticalSection() = 0;
};

#endif // CALLBACKS_H
