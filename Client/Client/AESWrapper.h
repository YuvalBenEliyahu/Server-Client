#pragma once

#include <string>


class AESWrapper
{
public:
	static const unsigned int DEFAULT_KEYLENGTH = 32;
private:
	unsigned char _key[DEFAULT_KEYLENGTH];
	AESWrapper(const AESWrapper& aes);
public:
	AESWrapper();
	AESWrapper(const std::string& key);
	~AESWrapper();

	const unsigned char* getKey() const;

	std::string encrypt(const std::string& plain);
	std::string decrypt(const char* cipher, unsigned int length);
};