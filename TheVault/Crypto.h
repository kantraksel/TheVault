#pragma once
#include <string>
#include "SecureArray.h"
#include "Utility/FixedArray.h"

namespace Crypto
{
	extern const size_t PwMinSize;
	extern const size_t PwMaxSize;
	extern const size_t PwSaltSize;
	extern const size_t ChestKeySize;
	extern const size_t ChestNonceSize;

	bool Init();
	SecureArray AllocMemory(size_t size);
	void ZeroMemory(SecureArray& memory);
	void FillRandomBytes(SecureArray& memory);
	SecureArray CopyMemory(const SecureArray& memory);

	SecureArray HashPassword(const std::string_view& password, const SecureArray& salt);
	SecureArray HashData(const std::string_view& data);

	SecureArray CreateChest(const std::string_view& content, const SecureArray& key, const SecureArray& nonce);
	SecureArray OpenChest(const SecureArray& chest, const SecureArray& key, const SecureArray& nonce);
	bool OpenChestInPlace(SecureArray& chest, const SecureArray& key, const SecureArray& nonce);

	FixedArrayUChar Base64ToBuffer(const std::string_view& text);
	bool BufferToBase64(const FixedArrayUChar& buffer, std::string& text);
};
