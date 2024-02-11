#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include "RSAWrapper.h"
#include "Base64Wrapper.h"


class Client {
private:
    std::string name;
    std::string ClientID;
    std::string privateKey;
    std::string publickey;
    std::string filePath;
    std::string ipAddress;
    std::string port;

    bool parseIpPort(const std::string& ipPort);

public:

    Client();

    // Function to read from me.info file
    bool readFromMeInfo();
    bool writeToMeInfo(const uint8_t clientID[16]);
    // Function to read from transfer.info file
    bool readFromTransferInfo();
    bool createPrivateAndPublicKeys();

    // Getter functions
    std::string getName() const;
    std::string getClientID() const;
    std::string getPrivateKey() const;
    std::string getPublickey() const;
    std::string getIpAddress() const;
    std::string getPort() const;
};

#endif // CLIENT_H
