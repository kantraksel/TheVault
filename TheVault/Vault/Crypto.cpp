#include <sodium.h>
#include "Crypto.h"

const size_t Crypto::PwMinSize = crypto_pwhash_BYTES_MIN;
const size_t Crypto::PwMaxSize = crypto_pwhash_BYTES_MAX;
const size_t Crypto::PwSaltSize = crypto_pwhash_SALTBYTES;
const size_t Crypto::ChestKeySize = crypto_secretbox_KEYBYTES;
const size_t Crypto::ChestNonceSize = crypto_secretbox_NONCEBYTES;
const size_t Crypto::ChestExtraSize = crypto_secretbox_MACBYTES;

bool Crypto::Init()
{
	return sodium_init() >= 0;
}

void Crypto::ZeroMemory(SecureArray& memory)
{
	sodium_memzero(memory.data(), memory.size());
}

void Crypto::FillRandomBytes(SecureArray& memory)
{
	randombytes_buf(memory.data(), memory.size());
}

SecureArray Crypto::HashPassword(const std::string_view& password, const SecureArray& salt)
{
	if (password.size() < crypto_pwhash_PASSWD_MIN || password.size() > crypto_pwhash_PASSWD_MAX || salt.size() != crypto_pwhash_SALTBYTES)
		return nullptr;

	auto hash = SecureArray(crypto_secretbox_KEYBYTES);

#if _DEBUG
	constexpr uint64_t OpsLimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	constexpr size_t MemLimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
#else
	constexpr uint64_t OpsLimit = crypto_pwhash_OPSLIMIT_SENSITIVE + 15;
	constexpr size_t MemLimit = crypto_pwhash_MEMLIMIT_SENSITIVE * 2;
#endif

	int result = crypto_pwhash(hash.data(), hash.size(), password.data(), password.size(), salt.data(), OpsLimit, MemLimit, crypto_pwhash_ALG_DEFAULT);
	if (result < 0)
		return nullptr;

	return hash;
}

SecureArray Crypto::CreateChest(const SecureArray& content, const SecureArray& key, const SecureArray& nonce)
{
	if (content.empty() || content.size() > static_cast<size_t>(INT64_MAX) || key.size() != crypto_secretbox_KEYBYTES || nonce.size() != crypto_secretbox_NONCEBYTES)
		return nullptr;

	auto chest = SecureArray(content.size() + crypto_secretbox_MACBYTES);
	int result = crypto_secretbox_easy(chest.data(), content.data(), content.size(), nonce.data(), key.data());
	if (result < 0)
		return nullptr;

	return chest;
}

bool Crypto::OpenChestInPlace(SecureArray& chest, const SecureArray& key, const SecureArray& nonce)
{
	if (key.size() != crypto_secretbox_KEYBYTES || nonce.size() != crypto_secretbox_NONCEBYTES || chest.size() <= crypto_secretbox_MACBYTES)
		return false;

	int result = crypto_secretbox_open_easy(chest.data(), chest.data(), chest.size(), nonce.data(), key.data());
	if (result < 0)
		return false;

	return true;
}

SecureArray Crypto::Base64ToBuffer(const std::string_view& text)
{
	if (text.empty() || text.size() % 4 != 0 || text.size() > INT32_MAX)
		return nullptr;

	size_t size;
	auto buffer = SecureArray(text.size() / 4 * 3);
	if (sodium_base642bin(buffer.data(), buffer.size(), text.data(), text.size(), nullptr, &size, nullptr, sodium_base64_VARIANT_URLSAFE) == 0)
		return SecureArray::Copy(buffer.span(0, size));
	return nullptr;
}

std::string_view Crypto::BufferToBase64(const SecureArray& buffer, SecureArray& text)
{
	text = SecureArray(sodium_base64_encoded_len(buffer.size(), sodium_base64_VARIANT_URLSAFE));
	if (sodium_bin2base64(reinterpret_cast<char*>(text.data()), text.size(), buffer.data(), buffer.size(), sodium_base64_VARIANT_URLSAFE))
	{
		return { reinterpret_cast<char*>(text.data()), text.size() - 1 };
	}
	text = nullptr;
	return {};
}
