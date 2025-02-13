#include <sodium.h>
#include "Crypto.h"

const size_t Crypto::PwMinSize = crypto_pwhash_BYTES_MIN;
const size_t Crypto::PwMaxSize = crypto_pwhash_BYTES_MAX;
const size_t Crypto::PwSaltSize = crypto_pwhash_SALTBYTES;
const size_t Crypto::ChestKeySize = crypto_secretbox_KEYBYTES;
const size_t Crypto::ChestNonceSize = crypto_secretbox_NONCEBYTES;

bool Crypto::Init()
{
	return sodium_init() >= 0;
}

static void FreeMemory(void* ptr)
{
	sodium_free(ptr);
}

SecureArray Crypto::AllocMemory(size_t size)
{
	char* mem = (char*)sodium_malloc(size);
	if (!mem)
		return nullptr;
	return SecureArray::Wrap(mem, size, FreeMemory);
}

void Crypto::ZeroMemory(SecureArray& memory)
{
	sodium_memzero(memory, memory.size());
}

void Crypto::FillRandomBytes(SecureArray& memory)
{
	randombytes_buf(memory, memory.size());
}

SecureArray Crypto::CopyMemory(const SecureArray& memory)
{
	auto size = memory.size();
	char* mem = (char*)sodium_malloc(size);
	if (!mem)
		return nullptr;
	memcpy(mem, memory, size);
	return SecureArray::Wrap(mem, size, FreeMemory);
}

SecureArray Crypto::HashPassword(const std::string_view& password, const SecureArray& salt)
{
	if (password.size() < crypto_pwhash_PASSWD_MIN || password.size() > crypto_pwhash_PASSWD_MAX || salt.size() != crypto_pwhash_SALTBYTES)
		return nullptr;

	auto hash = AllocMemory(crypto_secretbox_KEYBYTES);
	if (!hash)
		return nullptr;

#if _DEBUG
	constexpr uint64_t OpsLimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	constexpr size_t MemLimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
#else
	constexpr uint64_t OpsLimit = crypto_pwhash_OPSLIMIT_SENSITIVE;
	constexpr size_t MemLimit = crypto_pwhash_MEMLIMIT_SENSITIVE;
#endif

	int result = crypto_pwhash(hash, hash.size(), password.data(), password.size(), salt, OpsLimit, MemLimit, crypto_pwhash_ALG_DEFAULT);
	if (result < 0)
		return nullptr;

	return hash;
}

SecureArray Crypto::HashData(const std::string_view& data)
{
	if (data.empty())
		return nullptr;

	auto hash = AllocMemory(crypto_generichash_BYTES);
	if (!hash)
		return nullptr;

	int result = crypto_generichash(hash, hash.size(), (unsigned char*)data.data(), data.size(), nullptr, 0);
	if (result < 0)
		return nullptr;

	return hash;
}

SecureArray Crypto::CreateChest(const std::string_view& content, const SecureArray& key, const SecureArray& nonce)
{
	if (content.empty() || key.size() != crypto_secretbox_KEYBYTES || nonce.size() != crypto_secretbox_NONCEBYTES)
		return nullptr;

	auto chest = AllocMemory(content.size() + crypto_secretbox_MACBYTES);
	if (!chest)
		return nullptr;

	int result = crypto_secretbox_easy(chest, (unsigned char*)content.data(), content.size(), nonce, key);
	if (result < 0)
		return nullptr;

	return chest;
}

SecureArray Crypto::OpenChest(const SecureArray& chest, const SecureArray& key, const SecureArray& nonce)
{
	if (!chest || key.size() != crypto_secretbox_KEYBYTES || nonce.size() != crypto_secretbox_NONCEBYTES || chest.size() <= crypto_secretbox_MACBYTES)
		return nullptr;

	auto content = AllocMemory(chest.size() - crypto_secretbox_MACBYTES);
	if (!content)
		return nullptr;

	int result = crypto_secretbox_open_easy(content, chest, chest.size(), nonce, key);
	if (result < 0)
		return nullptr;

	return content;
}

bool Crypto::OpenChestInPlace(SecureArray& chest, const SecureArray& key, const SecureArray& nonce)
{
	if (!chest || key.size() != crypto_secretbox_KEYBYTES || nonce.size() != crypto_secretbox_NONCEBYTES || chest.size() <= crypto_secretbox_MACBYTES)
		return false;

	int result = crypto_secretbox_open_easy(chest, chest, chest.size(), nonce, key);
	if (result < 0)
		return false;

	return true;
}

FixedArrayUChar Crypto::Base64ToBuffer(const std::string_view& text)
{
	if (text.size() == 0 || text.size() % 4 != 0 || text.size() > INT32_MAX)
		return nullptr;

	size_t size;
	auto buffer = FixedArrayUChar((unsigned int)text.size() / 4 * 3);
	if (sodium_base642bin(buffer, buffer.size(), text.data(), text.size(), nullptr, &size, nullptr, sodium_base64_VARIANT_URLSAFE) == 0)
		return FixedArrayUChar::Copy(buffer, (unsigned int)size);
	return nullptr;
}

bool Crypto::BufferToBase64(const FixedArrayUChar& buffer, std::string& text)
{
	text.resize(sodium_base64_encoded_len(buffer.size(), sodium_base64_VARIANT_URLSAFE));
	if (sodium_bin2base64(text.data(), text.size(), buffer, buffer.size(), sodium_base64_VARIANT_URLSAFE))
	{
		text.resize(text.size() - 1);
		return true;
	}
	text.clear();
	return false;
}
