#include "clientHandler.h"


ClientHandler::ClientHandler() {}

bool ClientHandler::initialize() {
    try {
        // Initialize the client

        client.readFromTransferInfo();
        // Display client information
        std::cout << "Client Name: " << client.getName() << std::endl;
        std::cout << "Server IP Address: " << client.getIpAddress() << std::endl;
        std::cout << "Server Port: " << client.getPort() << std::endl;

        // Set socket information using client's server details
        if (!socketHandler.setSocketInfo(client.getIpAddress(), client.getPort())) {
            std::cerr << "Error setting socket information." << std::endl;;
            return false;
        }

        // reading me.info, if me.info found re-connecting
        if (!client.readFromMeInfo() || !reConnect())
            // me.info not found. intializing first time registration and RSA change
            if (!firstTimeRegistration() || !sendRSA())
                return false;

        sendFile();

        return true;

    }catch (const std::exception& e) {
        std::cerr << "Error sending file: " << e.what() << std::endl;
        return false;
    }
}

bool ClientHandler::firstTimeRegistration() {
    try {
        RequestRegistration requestRegistration(client.getName());
        RequestHeader requestHeader("", RequestPayloadCode::REQUEST_REGISTRATION, sizeof(requestRegistration));
        Request<RequestRegistration> request(requestHeader, requestRegistration);
        std::vector<uint8_t> receivedData;
        if (!socketHandler.performRequestResponse(reinterpret_cast<const uint8_t*>(&request), sizeof(request), receivedData)) {
            return false;
        }
        Response<RegistrationSuccess> response(receivedData);
        isResponseError(response);
        std::cerr << "Registration successfully" << std::endl;
        client.createPrivateAndPublicKeys();
        client.writeToMeInfo(response.payload.clientID);
        client.readFromMeInfo();
        return true;
    }
    catch (const ServerError& e) {
        firstTimeRegistration();
    }
}

bool ClientHandler::reConnect() {
    try {
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
    catch (const ServerError& e) {
        reConnect();
    }
}


bool ClientHandler::sendRSA() {
    try {
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
    catch (const ServerError& e) {
        sendRSA();
    }
}

bool ClientHandler::receiveAES(std::vector<uint8_t> receivedData) {
    Response<ReceivedPublicKey> response(receivedData);
    isResponseError(response);
    std::cerr << "AES received: " << std::endl;
    client.decryptAES(response.payload.aesKey);
    return true;
}


void ClientHandler::sendFile() {
    try {
        int retryCount = 0;
        std::string filePath = client.getFilePath();
        std::uint32_t fileSize = std::filesystem::file_size(filePath);
        std::string fileName = std::filesystem::path(filePath).stem().string();

        while (retryCount < MAX_CRC_RETRIES) {
            std::vector<uint8_t> receivedData;
            // Open the file for reading
            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Error opening file: " + filePath);
            }

            // Read the entire file into memory
            std::vector<char> fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            // Encrypt the entire file content
            std::string encryptedFileContent = client.aesEncryptor.encrypt(std::string(fileContent.begin(), fileContent.end()));
            uint32_t encryptedFileContentSize = static_cast<uint32_t>(encryptedFileContent.size());
            uint16_t totalPackets = (encryptedFileContent.size() / MAX_CHUNK_SIZE) + 1;


            // Send the file in chunks
            for (uint16_t packetNumber = 0; packetNumber < totalPackets; ++packetNumber) {
                // Calculate chunk size
                uint32_t currentChunkSize = std::min(MAX_CHUNK_SIZE, encryptedFileContentSize - (packetNumber * MAX_CHUNK_SIZE));

                // Create chunk from encrypted file content
                std::string chunk(encryptedFileContent.begin() + packetNumber * MAX_CHUNK_SIZE,
                    encryptedFileContent.begin() + packetNumber * MAX_CHUNK_SIZE + currentChunkSize);

                // Create the RequestFile structure for the current chunk
                RequestFile chunkRequestFile{
                    currentChunkSize,
                    fileSize,
                    packetNumber,
                    totalPackets,
                    fileName,
                    chunk
                };

                // Create and send request
                RequestHeader requestHeader(client.getClientID(), RequestPayloadCode::REQUEST_SEND_FILE, sizeof(chunkRequestFile));
                Request<RequestFile> request(requestHeader, chunkRequestFile);
                socketHandler.performRequestResponse(reinterpret_cast<const uint8_t*>(&request), sizeof(request), receivedData);
            }

            // Close the file
            file.close();
            std::cerr << "File sent successfully" << std::endl;

            if (receiveCRC(receivedData)) {
                // CRC check succeeded, break the loop and return success
                std::cerr << "CRC valid" << std::endl;
                requestCRC(fileName, RequestPayloadCode::REQUEST_CRC_VALID);
                return;
            }
            else {
                // CRC check failed, send CRC failure response and retry
                std::cerr << "CRC invalid, retry number: " << retryCount << std::endl;
                requestCRC(fileName, RequestPayloadCode::REQUEST_CRC_NOT_VALID_RE_SEND);
                retryCount++;
            }
        }
        std::cerr << "CRC invalid, stopped" << std::endl;
        requestCRC(fileName, RequestPayloadCode::REQUEST_CRC_NOT_VALID_STOP);
    }
    catch (const ServerError& e) {
        sendFile();
    }
}


bool ClientHandler::receiveCRC(std::vector<uint8_t> receivedData) {
    Response<FileReceivedWithValidCRC> response(receivedData);
    isResponseError(response);
    std::string filePath = client.getFilePath();
    std::string cksum = readfile(filePath); // return cksum
    std::string recivedCkSum = std::to_string(response.payload.ckSum);
    if (cksum == recivedCkSum) {
        return true;
    }
    return false;
}


void ClientHandler::requestCRC(std::string fileName, RequestPayloadCode payloadCode) {
    RequestCRC requestCRC(fileName);
    RequestHeader requestHeader(client.getClientID(), payloadCode, sizeof(requestCRC));
    Request<RequestCRC> request(requestHeader, requestCRC);
    socketHandler.performRequest(reinterpret_cast<const uint8_t*>(&request), sizeof(request));
}

template <typename Payload>
void ClientHandler::isResponseError(Response<Payload> response) {
    if ((response.responseHeader.code == ResponsePayloadCode::RESPONSE_ERROR) || (response.responseHeader.code == ResponsePayloadCode::RESPONSE_REGISTER_FAIL) || (response.responseHeader.code == ResponsePayloadCode::RESPONSE_RE_LOG_IN_DENIED)) {
        std::cerr << "Server responded with an error retry: " << ++retries << std::endl;
        if (retries == MAX_ERROR_RETRIES)
            throw std::runtime_error("Server responded with error max retries attempted.");
        throw ServerError("Server error occurred");
    }
}