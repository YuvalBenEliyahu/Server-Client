#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <iostream>
#include <boost/asio.hpp>

class SocketHandler {
private:
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver::results_type endpoint;

    bool isValidAddress(const std::string& address);
    bool isValidPort(const std::string& portStr);

public:
    SocketHandler();

    bool setSocketInfo(const std::string& address, const std::string& port);
    bool connect();
    bool send(const uint8_t* data, std::size_t dataSize);
    bool receive(std::vector<uint8_t>& data);

    ~SocketHandler();
};

#endif // SOCKETHANDLER_H
