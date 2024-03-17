#include "client.h"

Client::Client() {}

bool Client::readFromMeInfo() {
	std::ifstream meInfoFile("me.info");

	if (!meInfoFile.is_open()) {
		std::cerr << "me.info file not found.\n";
		return false;
	}

	if (!std::getline(meInfoFile, name)) {
		std::cerr << "Error reading name from me.info.\n";
		return false;
	}

	if (!std::getline(meInfoFile, clientID)) {
		std::cerr << "Error reading ASCII identifier from me.info.\n";
		return false;
	}

	std::string line;
	while (std::getline(meInfoFile, line)) {
		privateKey += line;
	}

	meInfoFile.close();
	return true;
}

bool Client::readFromTransferInfo() {
	std::ifstream transferInfoFile("transfer.info");

	if (!transferInfoFile.is_open()) {
		std::cerr << "Error: transfer.info file is missing." << std::endl;
		return false;
	}

	std::string ipPort;
	if (!std::getline(transferInfoFile, ipPort)) {
		std::cerr << "Error reading IP and port from transfer.info." << std::endl;
		return false;
	}

	if (!parseIpPort(ipPort)) {
		return false;
	}

	if (!std::getline(transferInfoFile, name)) {
		std::cerr << "Error reading name from me.info." << std::endl;
		return false;
	}

	if (!std::getline(transferInfoFile, filePath)) {
		std::cerr << "Error reading filePath from transfer.info." << std::endl;
		return false;
	}

	transferInfoFile.close();
	return true;
}

bool Client::writeToMeInfo(const uint8_t clientID[16]) {
	std::ofstream meInfoFile("me.info");

	if (!meInfoFile.is_open()) {
		std::cerr << "Error: Unable to open me.info for writing.\n";
		return false;
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
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error writing to me.info: " << e.what() << '\n';
		return false;
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


bool Client::parseIpPort(const std::string& ipPort) {
	try {
		size_t pos = ipPort.find(":");
		if (pos == std::string::npos) {
			std::cerr << "Error: Colon not found in IP and port string: " << ipPort << std::endl;
			return false;
		}

		ipAddress = ipPort.substr(0, pos);
		port = ipPort.substr(pos + 1);

		if (ipAddress.empty() || port.empty()) {
			std::cerr << "Error: IP address or port is empty in transfer.info: " << ipPort << std::endl;
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing IP address and port from transfer.info: " << e.what() << std::endl;
		return false;
	}

	return true;
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