#pragma once
#include <string>
#include "Utils/SecureArray.h"

namespace Crypto
{
	extern const size_t PwMinSize;
	extern const size_t PwMaxSize;
	extern const size_t PwSaltSize;
	extern const size_t ChestKeySize;
	extern const size_t ChestNonceSize;
	extern const size_t ChestExtraSize;

	bool Init();
	void ZeroMemory(SecureArray& memory);
	void FillRandomBytes(SecureArray& memory);

	SecureArray HashPassword(const std::string_view& password, const SecureArray& salt);

	SecureArray CreateChest(const SecureArray& content, const SecureArray& key, const SecureArray& nonce);
	bool OpenChestInPlace(SecureArray& chest, const SecureArray& key, const SecureArray& nonce);

	SecureArray Base64ToBuffer(const std::string_view& text);
	bool BufferToBase64(const SecureArray& buffer, std::string& text);
};
