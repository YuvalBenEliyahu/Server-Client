#include "Protocol.h"


// Requests protocols
RequestHeader::RequestHeader(const std::string& clientID, uint16_t code, uint32_t payloadSize)
    : code(code), payloadSize(payloadSize)
{
    strcpy_s(reinterpret_cast<char*>(this->clientID), CLIENT_ID_SIZE, clientID.c_str());
}
// RequestRegistration constructor
RequestRegistration::RequestRegistration(const std::string& data) {
    strcpy_s(reinterpret_cast<char*>(this->name), NAME_SIZE, data.c_str());
}

// RequestPublicKey constructor
RequestPublicKey::RequestPublicKey(const std::string& name, const std::string& publicKey) {
    strcpy_s(reinterpret_cast<char*>(this->name), NAME_SIZE, name.c_str());
    strcpy_s(reinterpret_cast<char*>(this->publicKey), PUBLIC_KEY_SIZE, publicKey.c_str());
}

// RequestReConnect constructor
RequestReConnect::RequestReConnect(const std::string& name) {
    strcpy_s(reinterpret_cast<char*>(this->name), NAME_SIZE, name.c_str());
}

// RequestFile constructor
RequestFile::RequestFile(uint32_t contentSize, uint32_t origFileSize, uint32_t packetNumber, uint32_t totalPackets,
    const std::string& fileName, const std::string& messageContent)
    : contentSize(contentSize), origFileSize(origFileSize), packetNumber(packetNumber), totalPackets(totalPackets) {
    strcpy_s(reinterpret_cast<char*>(this->fileName), FILE_SIZE, fileName.c_str());
    strcpy_s(reinterpret_cast<char*>(this->messageContent), PAYLOAD_SIZE, messageContent.c_str());
}

// RequestCRC constructor
RequestCRC::RequestCRC(const std::string& fileName) {
    strcpy_s(reinterpret_cast<char*>(this->fileName), FILE_SIZE, fileName.c_str());
}

// Response protocols
ResponseHeader::ResponseHeader(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Error unpacking request: empty header");
    }
    if (data.size() != RESPONSE_HEADER_SIZE) {
        throw std::runtime_error("Error unpacking request: incomplete header data");
    }
    std::memcpy(&version, &data[0], sizeof(version));
    std::memcpy(&code, &data[sizeof(version)], sizeof(code));
    std::memcpy(&payloadSize, &data[sizeof(version) + sizeof(code)], sizeof(payloadSize));
}

RegistrationSuccess::RegistrationSuccess(const std::vector<uint8_t>& data) {
        std::memcpy(&ClientID, &data[0], CLIENT_ID_SIZE);
}

// ReceivedPublicKey implementation...
ReceivedPublicKey::ReceivedPublicKey(const std::vector<uint8_t>& data) {
    if (data.size() >= NAME_SIZE + PUBLIC_KEY_SIZE) {
        std::memcpy(name, data.data(), NAME_SIZE);
        std::memcpy(publicKey, data.data() + NAME_SIZE, PUBLIC_KEY_SIZE);
    }
    else {
        throw std::runtime_error("Insufficient data for ReceivedPublicKey");
    }
}

// FileReceivedWithValidCRC implementation...
FileReceivedWithValidCRC::FileReceivedWithValidCRC(const std::vector<uint8_t>& data) {
    if (data.size() >= NAME_SIZE) {
        std::memcpy(name, data.data(), NAME_SIZE);
    }
    else {
        throw std::runtime_error("Insufficient data for FileReceivedWithValidCRC");
    }
}

// MessageReceived implementation...
MessageReceived::MessageReceived(const std::vector<uint8_t>& data) {
    if (data.size() >= 4 * sizeof(uint32_t) + FILE_SIZE) {
        contentSize = *reinterpret_cast<const uint32_t*>(data.data());
        OrigFileSize = *reinterpret_cast<const uint32_t*>(data.data() + sizeof(uint32_t));
        PacketNumber = *reinterpret_cast<const uint32_t*>(data.data() + 2 * sizeof(uint32_t));
        totalPackets = *reinterpret_cast<const uint32_t*>(data.data() + 3 * sizeof(uint32_t));

        std::memcpy(fileName, data.data() + 4 * sizeof(uint32_t), FILE_SIZE);

        messageContent = new uint8_t[contentSize + 1];
        std::memcpy(messageContent, data.data() + 4 * sizeof(uint32_t) + FILE_SIZE, contentSize + 1);
    }
    else {
        throw std::runtime_error("Insufficient data for MessageReceived");
    }
}

MessageReceived::~MessageReceived() {
    delete[] messageContent;
}

// ReConnectAccepted implementation...
ReConnectAccepted::ReConnectAccepted(const std::vector<uint8_t>& data) {
    if (data.size() >= NAME_SIZE) {
        std::memcpy(fileName, data.data(), NAME_SIZE);
    }
    else {
        throw std::runtime_error("Insufficient data for ReConnectAccepted");
    }
}

// ReConnectDenied implementation...
ReConnectDenied::ReConnectDenied(const std::vector<uint8_t>& data) {
    if (data.size() >= NAME_SIZE) {
        std::memcpy(fileName, data.data(), NAME_SIZE);
    }
    else {
        throw std::runtime_error("Insufficient data for ReConnectDenied");
    }
}

// ErrorResponse implementation...
ErrorResponse::ErrorResponse(const std::vector<uint8_t>& data) {
    if (data.size() >= NAME_SIZE) {
        std::memcpy(fileName, data.data(), NAME_SIZE);
    }
    else {
        throw std::runtime_error("Insufficient data for ErrorResponse");
    }
}

