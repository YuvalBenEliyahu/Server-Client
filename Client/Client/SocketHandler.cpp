#include "SocketHandler.h"

SocketHandler::SocketHandler() : socket(ioContext) {}

bool SocketHandler::setSocketInfo(const std::string& address, const std::string& port) {
    if (!isValidAddress(address) || !isValidPort(port))
        return false;
    try {
        endpoint = boost::asio::ip::tcp::resolver(ioContext).resolve(address, port);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error resolving address: " << e.what() << std::endl;
        return false;
    }
}

bool SocketHandler::connect() {
    try {
        boost::asio::connect(socket, endpoint);
        socket.non_blocking(false);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error connecting to server: " << e.what() << std::endl;
        return false;
    }
}

bool SocketHandler::send(const uint8_t* data, std::size_t dataSize) {
    try {
        boost::asio::write(socket, boost::asio::buffer(data, dataSize));
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error sending data: " << e.what() << std::endl;
        return false;
    }
}

bool SocketHandler::receive(std::vector<uint8_t>& data) {
    try {
        boost::asio::streambuf receiveBuffer;
        boost::system::error_code error;

        // Read data into the buffer
        size_t bytesRead = boost::asio::read(socket, receiveBuffer, error);

        // Resize the vector to accommodate the received data
        data.resize(bytesRead);

        // Copy the data from the buffer to the vector
        boost::asio::buffer_copy(boost::asio::buffer(data), receiveBuffer.data());

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error receiving data: " << e.what() << std::endl;
        return false;
    }
}

SocketHandler::~SocketHandler() {
    try {
        socket.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error closing socket: " << e.what() << std::endl;
    }
}

bool SocketHandler::isValidAddress(const std::string& address) {
    try {
        boost::asio::ip::address_v4::from_string(address);
    }
    catch (...) {
        std::cout << "Error parsing IPv4 address: " << address << std::endl;
        return false;
    }
    return true;
}

bool SocketHandler::isValidPort(const std::string& portStr) {
    try {
        size_t pos;
        int port = std::stoi(portStr, &pos);

        // Check if the entire string was used for conversion
        if (pos != portStr.size()) {
            return false;
        }

        if (port < 0 || port > 65535) {
            return false;
        }
        return true;
    }
    catch (...) {
        std::cout << "Error parsing port: " << portStr << std::endl;
        return false;
    }
}

