#include "clientHandler.h"


ClientHandler::ClientHandler() {}

bool ClientHandler::initialize() {
    // Initialize the client

    if (!client.readFromTransferInfo()) {
        return false;
    }
    // Display client information
    std::cout << "Client Name: " << client.getName() << std::endl;
    std::cout << "Server IP Address: " << client.getIpAddress() << std::endl;
    std::cout << "Server Port: " << client.getPort() << std::endl;

    // Set socket information using client's server details
    if (!socketHandler.setSocketInfo(client.getIpAddress(), client.getPort())) {
        std::cerr << "Error setting socket information." << std::endl;;
        return false;
    }

    // Connect to the server
    if (!socketHandler.connect()) {
        std::cerr << "Error connecting to the server." << std::endl;
        return false;
    }
    // reading me.info
    if (!client.readFromMeInfo() || !reConnect())
        // me.info not found. intializing first time registration and RSA change
        if (!firstTimeRegistration() || !sendRSA())
            return false;
    if (!receiveAES())
        return false;
    return true;
}

bool ClientHandler::firstTimeRegistration() {

    RequestRegistration requestRegistration(client.getName());
    RequestHeader requestHeader("", RequestPayloadCode::REQUEST_REGISTRATION, sizeof(requestRegistration));
    Request<RequestRegistration> request(requestHeader, requestRegistration);
    if (!socketHandler.send(reinterpret_cast<const uint8_t*>(&request), sizeof(request))) {
        return false;
    }
    std::cerr << "Client username sent" << std::endl;
    std::vector<uint8_t> receivedData;
    if (!socketHandler.receive(receivedData)) {
        return false;
    }
    Response<RegistrationSuccess> response(receivedData);
    std::cerr << "Registration succsesfuly" << std::endl;
    client.createPrivateAndPublicKeys();
    client.writeToMeInfo(response.payload.ClientID);

    return true;
}

bool ClientHandler::reConnect() {
    std::cerr << "Reconnecting" << std::endl;
    RequestReConnect requestReConnect(client.getName());
    RequestHeader requestHeader(client.getName(), RequestPayloadCode::REQUEST_RE_CONNECT, sizeof(requestReConnect));
    Request<RequestReConnect> request(requestHeader, requestReConnect);
    if (!socketHandler.send(reinterpret_cast<const uint8_t*>(&request), sizeof(request))) {
        return false;
    }
    return true;
}


bool ClientHandler::sendRSA() {
    std::cerr << "Creating RSA" << std::endl;
    RequestPublicKey requestPublicKey(client.getName(), client.getPublickey());
    RequestHeader requestHeader(client.getClientID(), RequestPayloadCode::REQUEST_PUBLIC_KEY, sizeof(requestPublicKey));
    Request<RequestPublicKey> request(requestHeader, requestPublicKey);

    std::cerr << "Sending RSA" << std::endl;
    if (!socketHandler.send(reinterpret_cast<const uint8_t*>(&request), sizeof(request))) {
        return false;
    }

    return true;
}

bool ClientHandler::receiveAES() {
    std::vector<uint8_t> receivedData;
    if (!socketHandler.receive(receivedData)) {
        return false;
    }

    Response<ReceivedPublicKey> response(receivedData);
    std::cerr << "AES received" << std::endl;

    // Process the received AES key as needed

    return true;
}






