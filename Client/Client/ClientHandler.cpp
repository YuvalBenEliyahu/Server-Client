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

    // reading me.info
    if (!client.readFromMeInfo() || !reConnect())
        // me.info not found. intializing first time registration and RSA change
        if (!firstTimeRegistration() || !sendRSA())
            return false;
    sendFile();
    return true;
}

bool ClientHandler::firstTimeRegistration() {

    RequestRegistration requestRegistration(client.getName());
    RequestHeader requestHeader("", RequestPayloadCode::REQUEST_REGISTRATION, sizeof(requestRegistration));
    Request<RequestRegistration> request(requestHeader, requestRegistration);
    std::vector<uint8_t> receivedData;
    if (!socketHandler.performRequestResponse(reinterpret_cast<const uint8_t*>(&request), sizeof(request), receivedData)) {
        return false;
    }
    Response<RegistrationSuccess> response(receivedData);
    std::cerr << "Registration successfully" << std::endl;
    client.createPrivateAndPublicKeys();
    client.writeToMeInfo(response.payload.clientID);
    client.readFromMeInfo();

    return true;
}

bool ClientHandler::reConnect() {
    std::cerr << "Reconnecting" << std::endl;
    RequestReConnect requestReConnect(client.getName());
    RequestHeader requestHeader(client.getClientID(), RequestPayloadCode::REQUEST_RE_CONNECT, sizeof(requestReConnect));
    Request<RequestReConnect> request(requestHeader, requestReConnect);
    std::vector<uint8_t> receivedData;
    if (!socketHandler.performRequestResponse(reinterpret_cast<const uint8_t*>(&request), sizeof(request), receivedData)) {
        return false;
    }
    if (!receiveAES(receivedData))
        return false;
    std::cerr << "Reconnected successfully" << std::endl;
    return true;
}


bool ClientHandler::sendRSA() {
    std::cerr << "Creating RSA" << std::endl;
    RequestPublicKey requestPublicKey(client.getName(), client.getPublickey());
    RequestHeader requestHeader(client.getClientID(), RequestPayloadCode::REQUEST_PUBLIC_KEY, sizeof(requestPublicKey));
    Request<RequestPublicKey> request(requestHeader, requestPublicKey);
    std::vector<uint8_t> receivedData;
    std::cerr << "Sending RSA" << std::endl;
    if (!socketHandler.performRequestResponse(reinterpret_cast<const uint8_t*>(&request), sizeof(request), receivedData)) {
        return false;
    }
    if (!receiveAES(receivedData))
        return false;
    std::cerr << "Connected for first time successfully" << std::endl;
    return true;
}

bool ClientHandler::receiveAES(std::vector<uint8_t> receivedData) {
    Response<ReceivedPublicKey> response(receivedData);
    std::cerr << "AES received: " << std::endl;
    client.decryptAES(response.payload.aesKey);
    return true;
}


bool ClientHandler::sendFile() {
    try {

        std::string filePath = client.getFilePath();
        // Open the file for reading
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return false;
        }
        std::uintmax_t fileSize = std::filesystem::file_size(filePath);
        std::string fileName = std::filesystem::path(filePath).stem().string();
        const uint32_t maxChunkSize = 1024;
        const uint16_t totalPackets = (fileSize / maxChunkSize) + 1;

        // Send the file in chunks
        for (uint16_t packetNumber = 0;packetNumber <= totalPackets - 1; ++packetNumber) {
            // Check if we reached the end of the file
            if (file.tellg() == -1) {
                break;
            }
            std::streampos currentPos = file.tellg();
            file.seekg(0, std::ios::end);
            std::streampos endPos = file.tellg();
            file.seekg(currentPos);
            std::uintmax_t remainingSize = endPos - currentPos;

            // Determine the chunk size
            uint32_t currentChunkSize = std::min(maxChunkSize, static_cast<uint32_t>(remainingSize));

            // Read the chunk from the file
            std::vector<char> chunk(currentChunkSize);
            file.read(chunk.data(), currentChunkSize);
            // Encrypt the message content with aesEncryptor
            std::string encryptedMessageContent = client.aesEncryptor.encrypt(std::string(chunk.begin(), chunk.end()));

            // Create the RequestFile structure
            RequestFile requestFile{
                static_cast<uint32_t>(encryptedMessageContent.size()),
                static_cast<uint32_t>(chunk.size()),
                packetNumber,
                totalPackets,
                fileName,
                encryptedMessageContent
            };
            RequestHeader requestHeader(client.getClientID(), RequestPayloadCode::REQUEST_SEND_FILE, sizeof(requestFile));
            Request<RequestFile> request(requestHeader, requestFile);
            if(packetNumber != totalPackets -1){
                socketHandler.performRequest(reinterpret_cast<const uint8_t*>(&request), sizeof(request));
            }
            else {
                std::vector<uint8_t> receivedData;
                if (!socketHandler.performRequestResponse(reinterpret_cast<const uint8_t*>(&request), sizeof(request), receivedData)) {
                    return false;
                }
            }

        }
        // Close the file
        file.close();
        std::cerr << "File sent successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error sending file: " << e.what() << std::endl;
        return false;
    }
}


