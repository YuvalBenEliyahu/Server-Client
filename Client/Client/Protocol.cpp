#include "Protocol.h"


// Requests protocols
RequestHeader::RequestHeader(const std::string& clientID, uint16_t code, uint32_t payloadSize)
	: code(code), payloadSize(payloadSize)
{
	memcpy(this->clientID, boost::algorithm::unhex(clientID).c_str(), CLIENT_ID_SIZE);
}
RequestRegistration::RequestRegistration(const std::string& name) {
	strcpy_s(reinterpret_cast<char*>(this->name), NAME_SIZE, name.c_str());
}

RequestPublicKey::RequestPublicKey(const std::string& name, const std::string& publicKey) {
	strcpy_s(reinterpret_cast<char*>(this->name), NAME_SIZE, name.c_str());
	std::memcpy(this->publicKey, publicKey.c_str(), PUBLIC_KEY_SIZE);
}

RequestReConnect::RequestReConnect(const std::string& name) {
	strcpy_s(reinterpret_cast<char*>(this->name), NAME_SIZE, name.c_str());
}

RequestFile::RequestFile(uint32_t contentSize, uint32_t origFileSize, uint16_t packetNumber, uint16_t totalPackets,
	const std::string& fileName, const std::string& messageContent)
	: contentSize(contentSize), origFileSize(origFileSize), packetNumber(packetNumber), totalPackets(totalPackets) {
	strcpy_s(reinterpret_cast<char*>(this->fileName), FILE_SIZE, fileName.c_str());
	std::memcpy(this->messageContent, messageContent.c_str(), 64);
}

RequestCRC::RequestCRC(const std::string& fileName) {
	strcpy_s(reinterpret_cast<char*>(this->fileName), FILE_SIZE, fileName.c_str());
}

// Response protocols
ResponseHeader::ResponseHeader(const std::vector<uint8_t>& data) {
	std::memcpy(&version, &data[0], sizeof(version));
	std::memcpy(&code, &data[sizeof(version)], sizeof(code));
	std::memcpy(&payloadSize, &data[sizeof(version) + sizeof(code)], sizeof(payloadSize));
}

RegistrationSuccess::RegistrationSuccess(const std::vector<uint8_t>& data) {
	std::memcpy(&clientID, &data[0], CLIENT_ID_SIZE);
}

ReceivedPublicKey::ReceivedPublicKey(const std::vector<uint8_t>& data) {
	std::memcpy(&clientID, &data[0], CLIENT_ID_SIZE);
	aesKey.assign(data.begin() + CLIENT_ID_SIZE, data.begin() + CLIENT_ID_SIZE + AES_KEY_SIZE);
}

FileReceivedWithValidCRC::FileReceivedWithValidCRC(const std::vector<uint8_t>& data) {
	std::memcpy(&clientID, &data[0], CLIENT_ID_SIZE);
}

ReConnectAccepted::ReConnectAccepted(const std::vector<uint8_t>& data) {
	std::memcpy(&clientID, &data[0], CLIENT_ID_SIZE);
	aesKey.assign(data.begin() + CLIENT_ID_SIZE, data.begin() + CLIENT_ID_SIZE + AES_KEY_SIZE);
}

ReConnectDenied::ReConnectDenied(const std::vector<uint8_t>& data) {
	std::memcpy(&clientID, &data[0], CLIENT_ID_SIZE);
}


