#include "Vault.h"
#include "Crypto.h"
#include "Files/Container.h"
#include "Files/MemoryStream.h"

struct VaultHeader
{
	static constexpr unsigned short Magic = 0x5645;
	static constexpr unsigned short Version = 1;

	unsigned char LockSteps;
	unsigned char KeySalt[16];
	unsigned char LockNonce[24];
	unsigned char FirstKey[32];
};

Vault::Vault()
{
	if (sizeof(VaultHeader::KeySalt) != Crypto::PwSaltSize)
		throw std::exception("KeySalt size mismatch");

	if (sizeof(VaultHeader::LockNonce) != Crypto::ChestNonceSize)
		throw std::exception("LockNonce size mismatch");

	if (sizeof(VaultHeader::FirstKey) != Crypto::ChestKeySize)
		throw std::exception("FirstKey size mismatch");
}

Vault::~Vault()
{
}

bool Vault::Initialize()
{
	if (!mKeySalt)
		mKeySalt = Crypto::AllocMemory(sizeof(VaultHeader::KeySalt));
	if (!mLockNonce)
		mLockNonce = Crypto::AllocMemory(sizeof(VaultHeader::LockNonce));
	if (!mFirstKey)
		mFirstKey = Crypto::AllocMemory(sizeof(VaultHeader::FirstKey));

	if (!mKeySalt || !mLockNonce || !mFirstKey)
		return false;
	return true;
}

void Vault::Reset()
{
	mData.reset();
	mLockSteps.clear();
	mEBlock.reset();

	Crypto::ZeroMemory(mKeySalt);
	Crypto::ZeroMemory(mLockNonce);
	Crypto::ZeroMemory(mFirstKey);
}

void Vault::ResetCache()
{
	mData.reset();
	mLockSteps.clear();
	mEBlock.reset();
}

bool Vault::Open(const std::wstring_view& file)
{
	FileReader stream;
	if (!stream.Open(file))
		return false;
	
	VaultHeader header;
	unsigned int dataSize;

	if (!Container::Open<VaultHeader>(stream, header, dataSize) || dataSize == 0)
		return false;

	if (header.LockSteps == 0)
		return false;

	auto data = Crypto::AllocMemory(dataSize);
	if (!data)
		return false;

	auto ref = FixedArray<unsigned char>::CreateArrayRef(data, data.size());
	if (!stream.Read(ref))
		return false;

	std::vector<SecureArray> lockSteps;
	lockSteps.reserve(header.LockSteps);

	MemoryStream memory(data, data.size());
	for (unsigned char i = 0; i < header.LockSteps; ++i)
	{
		unsigned int size;
		if (!memory.Read(size) || size == 0)
			return false;

		auto mem = SecureArray::Wrap(data + memory.GetPos(), size, nullptr);
		memory.Seek(size);

		lockSteps.push_back(std::move(mem));
	}

	auto eblock = SecureArray::Wrap(data + memory.GetPos(), data.size() - memory.GetPos(), nullptr);
	if ((memory.GetPos() + eblock.size()) > data.size())
		return false;

	mData = std::move(data);
	mLockSteps = std::move(lockSteps);
	mEBlock = std::move(eblock);

	memcpy(mKeySalt, header.KeySalt, mKeySalt.size());
	memcpy(mLockNonce, header.LockNonce, mLockNonce.size());
	memcpy(mFirstKey, header.FirstKey, mFirstKey.size());
	return true;
}

bool Vault::Place(const std::wstring_view& file)
{
	FileWriter stream;
	if (!stream.Open(file))
		return false;

	VaultHeader header{};
	header.LockSteps = (unsigned char)mLockSteps.size();
	memcpy(header.KeySalt, mKeySalt, mKeySalt.size());
	memcpy(header.LockNonce, mLockNonce, mLockNonce.size());
	memcpy(header.FirstKey, mFirstKey, mFirstKey.size());

	if (!Container::BeginWrite<VaultHeader>(stream, header))
		return false;

	for (auto& step : mLockSteps)
	{
		auto size = (unsigned int)step.size();
		if (!stream.Write(size))
			return false;

		if (!stream.Write(step, size))
			return false;
	}

	if (!stream.Write(mEBlock, (uint32_t)mEBlock.size()))
		return false;

	if (!Container::EndWrite<VaultHeader>(stream))
		return false;
	return true;
}

SecureArray Vault::CreateKey(const std::string_view& password)
{
	return Crypto::HashPassword(password, mKeySalt);
}

SecureArray Vault::CreateMasterKey(const std::vector<SecureArray>& keys, const SecureArray& lastKey)
{
	size_t size = lastKey.size();
	for (auto& key : keys)
	{
		size += key.size();
	}
	if (size == 0)
		return nullptr;

	auto master = Crypto::AllocMemory(size);
	if (!master)
		return nullptr;

	MemoryStream stream(master, master.size());
	for (auto& key : keys)
	{
		if (stream.Write(key, key.size()) != key.size())
			return nullptr;
	}
	if (stream.Write(lastKey, lastKey.size()) != lastKey.size())
		return nullptr;

	auto data = std::string_view(master.str(), master.size());
	return Crypto::HashData(data);
}

bool Vault::UnlockStep(const SecureArray& key, int i, SecureArray& plain)
{
	if (i >= mLockSteps.size() || i < 0 || !key)
		return false;

	auto data = Crypto::OpenChest(mLockSteps[i], key, mLockNonce);
	if (!data)
		return false;
	MemoryStream memory(data, data.size());

	unsigned short size;
	if (!memory.Read(size) || size == 0 || (memory.GetPos() + size) > data.size())
		return false;

	plain = Crypto::AllocMemory(size);
	if (!plain)
		return false;

	memcpy_s(plain, plain.size(), data + memory.GetPos(), size);
	return true;
}

bool Vault::UnlockBlock(const SecureArray& key)
{
	if (!key)
		return false;

	return Crypto::OpenChestInPlace(mEBlock, key, mLockNonce);
}

void Vault::GenerateNew()
{
	Crypto::FillRandomBytes(mKeySalt);
	Crypto::FillRandomBytes(mLockNonce);
	Crypto::FillRandomBytes(mFirstKey);

	mData = {};
	mLockSteps.clear();
	mEBlock = {};
}

void Vault::ResetSteps()
{
	mLockSteps.clear();
}

bool Vault::AddStep(const SecureArray& plain, const SecureArray& key)
{
	if (!key || !plain)
		return false;

	auto size = (unsigned short)plain.size();
	auto data = Crypto::AllocMemory(sizeof(size) + plain.size());
	if (!data)
		return false;

	MemoryStream memory(data, data.size());
	if (!memory.Write(size) || memory.Write(plain, size) != size)
		return false;

	auto content = std::string_view(data.str(), data.size());
	auto block = Crypto::CreateChest(content, key, mLockNonce);
	if (!block)
		return false;

	mLockSteps.push_back(std::move(block));
	return true;
}

bool Vault::LockBlock(const SecureArray& key, const std::string_view& content)
{
	if (!key || content.empty())
		return false;

	mEBlock = Crypto::CreateChest(content, key, mLockNonce);
	return mEBlock;
}
