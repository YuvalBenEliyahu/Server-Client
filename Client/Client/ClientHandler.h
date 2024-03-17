#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "client.h"
#include "SocketHandler.h"
#include "Protocol.h"
#include <vector>
#include <filesystem>

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
    bool receiveAES(std::vector<uint8_t> receivedData);
    bool sendFile();
};

#endif // CLIENT_HANDLER_H

