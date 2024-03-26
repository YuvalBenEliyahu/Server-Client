#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
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

    void parseIpPort(const std::string& ipPort);

public:

    AESWrapper aesEncryptor;
    Client();

    bool readFromMeInfo();
    void writeToMeInfo(const uint8_t clientID[16]);
    void readFromTransferInfo();
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
    bool isSafeFilePath(const std::string& path);
    bool isValidName(const std::string& name);

};

#endif // CLIENT_H
