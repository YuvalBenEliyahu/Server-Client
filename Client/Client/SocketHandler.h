#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <iostream>
#include <boost/asio.hpp>

const size_t PACKET_SIZE = 1024;

class SocketHandler {
private:
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver::results_type endpoint;

    bool isValidAddress(const std::string& address);
    bool isValidPort(const std::string& portStr);

public:
    bool connected;
    SocketHandler();

    bool performRequestResponse(const uint8_t* requestData, std::size_t requestSize, std::vector<uint8_t>& responseData);
    bool performRequest(const uint8_t* requestData, std::size_t requestSize);
    bool setSocketInfo(const std::string& address, const std::string& port);
    bool connect();
    bool send(const uint8_t* data, std::size_t dataSize);
    bool receive(std::vector<uint8_t>& data);
    void closeConnection();
    ~SocketHandler();
};

#endif // SOCKETHANDLER_H
