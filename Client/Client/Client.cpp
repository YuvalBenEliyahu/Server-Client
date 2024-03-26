#include "client.h"

Client::Client() {}

bool Client::readFromMeInfo() {
	std::ifstream meInfoFile("me.info");

	if (!meInfoFile.is_open()) {
		return false;
	}

	if (!std::getline(meInfoFile, name)) {
		throw std::runtime_error("Error reading name from me.info.");
	}

	if (!std::getline(meInfoFile, clientID)) {
		throw std::runtime_error("Error reading ASCII identifier from me.info.");
	}

	std::string line;
	while (std::getline(meInfoFile, line)) {
		privateKey += line;
	}

	meInfoFile.close();
	return true;
}

void Client::readFromTransferInfo() {
	std::ifstream transferInfoFile("transfer.info");

	if (!transferInfoFile.is_open()) {
		throw std::runtime_error("Error: transfer.info file is missing.");
	}

	std::string ipPort;
	if (!std::getline(transferInfoFile, ipPort)) {
		throw std::runtime_error("Error reading IP and port from transfer.info.");
	}

	parseIpPort(ipPort);


	std::string name;
	if (!std::getline(transferInfoFile, name) || !isValidName(name)){
		throw std::runtime_error("Error reading name from transfer.info.");
	}

	if (!std::getline(transferInfoFile, filePath) || !isSafeFilePath(filePath)){
		throw std::runtime_error("Error reading filePath from transfer.info.");
	}

	transferInfoFile.close();
}

void Client::writeToMeInfo(const uint8_t clientID[16]) {
	std::ofstream meInfoFile("me.info");

	if (!meInfoFile.is_open()) {
		throw std::runtime_error("Error: Unable to open me.info for writing.");
	}

	try {
		// Write the name
		meInfoFile << name << '\n';

		// Write the hex representation of the clientID
		for (size_t i = 0; i < 16; ++i) {
			meInfoFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(clientID[i]);
		}
		meInfoFile << '\n';

		// Write the private key 
		meInfoFile << privateKey;

		std::cout << "me.info file created successfully.\n";
	}
	catch (const std::exception& e) {
		meInfoFile.close(); // Ensure file is closed before rethrowing the exception
		throw; // Re-throw the caught exception
	}
}

bool Client::createPrivateAndPublicKeys() {
	try {
		// Generate a new RSA key pair
		RSAPrivateWrapper privateKey;
		this->publicKey = privateKey.getPublicKey();

		// Get the keys as strings
		std::string private_key_str = Base64Wrapper::encode(privateKey.getPrivateKey());
		// save to client
		this->privateKey = (private_key_str);
		// Save private key to a file
		std::ofstream private_key_file("priv.key");
		if (!private_key_file.is_open()) {
			std::cerr << "Error: Unable to open priv.key for writing.\n";
			return false;
		}
		private_key_file << private_key_str;
		private_key_file.close();

		std::cout << "Key pair generated and saved successfully.\n";

		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return false;
	}
}
bool Client::decryptAES(const std::string& aesKey) {
	try {
		// Create RSA decryptor using an existing private key
		RSAPrivateWrapper rsaDecryptor(Base64Wrapper::decode(privateKey));

		// Use the private key to decrypt the AES key
		std::string decryptedAESKey = rsaDecryptor.decrypt(aesKey);

		// Generate the AES key using the decrypted key
		aesEncryptor = AESWrapper(decryptedAESKey);

		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error decrypting AES key: " << e.what() << std::endl;
		return false;
	}
}


void Client::parseIpPort(const std::string& ipPort) {
	try {
		size_t pos = ipPort.find(":");
		if (pos == std::string::npos) {
			throw std::invalid_argument("Colon not found in IP and port string: " + ipPort);
		}

		ipAddress = ipPort.substr(0, pos);
		port = ipPort.substr(pos + 1);

		if (ipAddress.empty() || port.empty()) {
			throw std::invalid_argument("IP address or port is empty in transfer.info: " + ipPort);
		}
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Error parsing IP address and port from transfer.info: " + std::string(e.what()));
	}

}
bool  Client::isValidName(const std::string& name) {
	// Check if the name is not empty and does not exceed NAME_SIZE
	if (name.empty() || name.size() > 255) {
		return false;
	}

	// Check if all characters are alphabetic or space and count spaces
	int spaceCount = 0;
	bool lastWasSpace = false;
	for (char c : name) {
		if (std::isspace(c)) {
			if (lastWasSpace) { // No consecutive spaces allowed
				return false;
			}
			lastWasSpace = true;
			++spaceCount;
		}
		else {
			if (!std::isalpha(c)) {
				return false;
			}
			lastWasSpace = false;
		}
	}

	// Check if the space count is less than or equal to 3
	return spaceCount <= 3;
}

bool  Client::isSafeFilePath(const std::string& path) {
	// Check for illegal characters
	const std::string illegalChars = ":?*|<>\"";
	for (char c : path) {
		if (illegalChars.find(c) != std::string::npos) {
			return false;
		}
	}

	// Check for relative paths
	if (path.substr(0, 2) == "..") {
		return false;
	}

	// Check for absolute paths
	if (std::filesystem::path(path).is_absolute()) {
		return false;
	}

	// Check if the path points to a symbolic link
	if (std::filesystem::is_symlink(path)) {
		return false;
	}

	return std::filesystem::exists(path);
}





// Getter functions
std::string Client::getName() const {
	return name;
}
std::string Client::getClientID() const {
	return clientID;
}
std::string Client::getPrivateKey() const {
	return privateKey;
}
std::string Client::getPublickey() const {
	return publicKey;
}
std::string Client::getFilePath() const {
	return filePath;
}
std::string Client::getIpAddress() const {
	return ipAddress;
}
std::string Client::getPort() const {
	return port;
}