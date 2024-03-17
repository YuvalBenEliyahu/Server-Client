#include "SocketHandler.h"

SocketHandler::SocketHandler() : socket(ioContext), connected(false) {}

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
    if (connected) {
        std::cerr << "Already connected." << std::endl;
        return false;
    }

    try {
        boost::asio::connect(socket, endpoint);
        socket.non_blocking(false);
        connected = true;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error connecting to server: " << e.what() << std::endl;
        return false;
    }
}

bool SocketHandler::send(const uint8_t* data, std::size_t dataSize) {
    if (!connected && !connect()) {
        return false;
    }

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
    if (!connected && !connect()) {
        return false;
    }

    try {
        boost::asio::streambuf receiveBuffer;
        boost::system::error_code error;

        // Read data into the buffer
        while (boost::asio::read(socket, receiveBuffer.prepare(PACKET_SIZE), error)) {
            receiveBuffer.commit(PACKET_SIZE);
        }

        // Resize the vector to accommodate the received data
        data.resize(boost::asio::buffer_size(receiveBuffer.data()));

        // Copy the data from the buffer to the vector
        boost::asio::buffer_copy(boost::asio::buffer(data), receiveBuffer.data());

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error receiving data: " << e.what() << std::endl;
        return false;
    }
}

void SocketHandler::closeConnection() {
    if (connected) {
        try {
            socket.close();
            connected = false;
        }
        catch (const std::exception& e) {
            std::cerr << "Error closing socket: " << e.what() << std::endl;
        }
    }
}

SocketHandler::~SocketHandler() {
    closeConnection();
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
bool SocketHandler::performRequest(const uint8_t* requestData, std::size_t requestSize) {
    if (!connected && !connect()) {
        std::cerr << "Error connecting." << std::endl;
        return false;
    }

    if (!send(requestData, requestSize)) {
        std::cerr << "Error sending request." << std::endl;
        return false;
    }

    closeConnection();
    return true;
}


bool SocketHandler::performRequestResponse(const uint8_t* requestData, std::size_t requestSize, std::vector<uint8_t>& responseData) {
    if (!connected && !connect()) {
        std::cerr << "Error connecting." << std::endl;
        return false;
    }

    if (!send(requestData, requestSize)) {
        std::cerr << "Error sending request." << std::endl;
        return false;
    }

    if (!receive(responseData)) {
        std::cerr << "Error receiving response." << std::endl;
        return false;
    }

    closeConnection();
    return true;
}
