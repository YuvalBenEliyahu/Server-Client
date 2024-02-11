#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "client.h"
#include "SocketHandler.h"
#include "Protocol.h"
#include <vector>



class ClientHandler {
private:
    Client client;
    SocketHandler socketHandler;
    const int VERSION = 3;

public:
    ClientHandler();
    bool initialize();
    bool firstTimeRegistration();
    bool reConnect();
    bool sendRSA();
    bool receiveAES();
};

#endif // CLIENT_HANDLER_H

