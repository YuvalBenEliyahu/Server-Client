#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "Base64Wrapper.h"
#include <boost/algorithm/hex.hpp>

class Client {
private:
    std::string name;
    std::string clientID;
    std::string privateKey;
    std::string publicKey;
    std::string filePath;
    std::string ipAddress;
    std::string port;
    std::string cyperText;

    bool parseIpPort(const std::string& ipPort);

public:

    AESWrapper aesEncryptor;
    Client();

    bool readFromMeInfo();
    bool writeToMeInfo(const uint8_t clientID[16]);
    bool readFromTransferInfo();
    bool createPrivateAndPublicKeys();
    bool decryptAES(const std::string& aesKey);

    // Getter and setters functions
    std::string getName() const;
    std::string getClientID() const;
    std::string getPrivateKey() const;
    std::string getPublickey() const;
    std::string getFilePath() const;
    std::string getIpAddress() const;
    std::string getPort() const;
};

#endif // CLIENT_H
