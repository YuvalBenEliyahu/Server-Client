#pragma pack(1)
#ifndef REQUEST_PROTOCOL_H
#define REQUEST_PROTOCOL_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <boost/algorithm/hex.hpp>

const size_t CLIENT_ID_SIZE = 16;
const size_t RESPONSE_HEADER_SIZE = 7;
const size_t NAME_SIZE = 255;
const size_t FILE_SIZE = 255;
const size_t PUBLIC_KEY_SIZE = 160;
const size_t AES_KEY_SIZE = 128;
const size_t PAYLOAD_SIZE = 128;
const uint8_t VERSION = 3;


enum RequestPayloadCode{
    REQUEST_REGISTRATION = 1025,
    REQUEST_PUBLIC_KEY = 1026,
    REQUEST_RE_CONNECT = 1027,
    REQUEST_SEND_FILE = 1028,
    REQUEST_CRC_VALID = 1029,
    REQUEST_CRC_NOT_VALID_RE_SEND = 1030,
    REQUEST_CRC_NOT_VALID_STOP = 1031,
};

enum ResponsePayloadCode{
    RESPONSE_REGISTER_SUCCESS = 1600,
    RESPONSE_REGISTER_FAIL = 1601,
    RESPONSE_RECEIVED_PUBLIC_KEY = 1602,
    RESPONSE_FILE_RECEIVED_WITH_VALID_CRC = 1603,
    RESPONSE_MSG_RECEIVED = 1604,
    RESPONSE_RE_LOG_IN_ACCEPTED = 1605,
    RESPONSE_RE_LOG_IN_DENIED = 1606,
    RESPONSE_ERROR = 1607,
};

class RequestHeader {
public:
    uint8_t clientID[CLIENT_ID_SIZE];
    uint8_t version = VERSION;
    uint16_t code;
    uint32_t payloadSize;

    RequestHeader(const std::string& clientID, uint16_t code, uint32_t payloadSize);
};

class RequestRegistration {
public:
    uint8_t name[NAME_SIZE];

    RequestRegistration(const std::string& name);
};

class RequestPublicKey {
public:
    uint8_t name[NAME_SIZE];
    uint8_t publicKey[PUBLIC_KEY_SIZE];

    RequestPublicKey(const std::string& name, const std::string& publicKey);
};

class RequestReConnect {
public:
    uint8_t name[NAME_SIZE];

    RequestReConnect(const std::string& name);
};

class RequestFile {
public:
    uint32_t contentSize;
    uint32_t origFileSize;
    uint16_t packetNumber;
    uint16_t totalPackets;
    uint8_t fileName[FILE_SIZE];
    uint8_t messageContent[64];

    RequestFile(uint32_t contentSize, uint32_t origFileSize, uint16_t packetNumber, uint16_t totalPackets,
        const std::string& fileName, const std::string& messageContent);
};

class RequestCRC {
public:
    uint8_t fileName[FILE_SIZE];

    RequestCRC(const std::string& fileName);
};


class ResponseHeader{
public:
    uint8_t version;
    uint16_t code;
    uint32_t payloadSize;
    ResponseHeader() = default;
    ResponseHeader(const std::vector<uint8_t>& data);
};


class RegistrationSuccess{
public:
    uint8_t clientID[CLIENT_ID_SIZE];
    RegistrationSuccess() = default;
    RegistrationSuccess(const std::vector<uint8_t>& data);

};

class RegistrationFail {
    RegistrationFail() = default;
};

class ReceivedPublicKey{
public:
    uint8_t clientID[CLIENT_ID_SIZE];
    std::string aesKey;
    ReceivedPublicKey() = default;
    ReceivedPublicKey(const std::vector<uint8_t>& data);

};

class FileReceivedWithValidCRC{
public:
    uint8_t clientID[CLIENT_ID_SIZE];
    uint32_t ContentSize;
    uint8_t fileName[NAME_SIZE];
    uint32_t ckSum;
    FileReceivedWithValidCRC() = default;
    FileReceivedWithValidCRC(const std::vector<uint8_t>& data);

};

class ReConnectAccepted{
public:
    uint8_t clientID[CLIENT_ID_SIZE];
    std::string aesKey;
    ReConnectAccepted() = default;
    ReConnectAccepted(const std::vector<uint8_t>& data);
};

class ReConnectDenied{
public:
    uint8_t clientID[CLIENT_ID_SIZE];
    ReConnectDenied() = default;
    ReConnectDenied(const std::vector<uint8_t>& data);
};

class ErrorResponse{
private:


public:
    ErrorResponse() = default;
    ErrorResponse(const std::vector<uint8_t>& data);

};

template <typename Payload>
class Request {
private:
    RequestHeader requestHeader;
    Payload payload;
public:
    Request(const RequestHeader& header, const Payload& payload) : requestHeader(header), payload(payload) {}
};


template <typename Payload>
class Response {
public:
    ResponseHeader responseHeader;
    Payload payload;
    Response(const std::vector<uint8_t>& data) {
        try {
            responseHeader = ResponseHeader(std::vector<uint8_t>(data.begin(), data.begin() + RESPONSE_HEADER_SIZE));
            //if (responseHeader.code == static_cast<uint16_t>(ResponsePayloadCode::RESPONSE_ERROR)) {
            //    payload = ErrorResponse(std::vector<uint8_t>(data.begin() + RESPONSE_HEADER_SIZE, data.end()));
            //}
            //else {
                payload = Payload(std::vector<uint8_t>(data.begin() + RESPONSE_HEADER_SIZE, data.end()));
            //}
        } catch (const std::exception& e) {
            std::cerr << "Error unpacking request: " << e.what() << std::endl;
        }
    }
};


#endif // RESPONSE_PROTOCOL_H


