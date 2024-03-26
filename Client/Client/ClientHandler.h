#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "client.h"
#include "SocketHandler.h"
#include "Protocol.h"
#include "cksum.h"
#include <vector>
#include <filesystem>

const size_t MAX_ERROR_RETRIES = 3;
const size_t MAX_CRC_RETRIES = 4;
const size_t MAX_CHUNK_SIZE = 1024;

class ClientHandler {
private:
    Client client;
    SocketHandler socketHandler;
    const int VERSION = 3;
    int retries = 0;

public:
    ClientHandler();
    bool initialize();
    bool firstTimeRegistration();
    bool reConnect();
    bool sendRSA();
    bool receiveAES(std::vector<uint8_t> receivedData);
    void sendFile();
    bool receiveCRC(std::vector<uint8_t> receivedData);
    void requestCRC(std::string fileName, RequestPayloadCode payloadCode);

    template <typename Payload>
    void isResponseError(Response<Payload> response);
};

class ServerError : public std::exception {
public:
    ServerError(const std::string& message) : message(message) {}

    // Override what() method to provide error message
    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};
#endif // CLIENT_HANDLER_H

