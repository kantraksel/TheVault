#include <filesystem>
#include "Vault.h"
#include "Crypto.h"
#include "Files/Container.h"
#include "Files/MemoryStream.h"

struct VaultHeader
{
	static constexpr unsigned short Magic = 0x5645;
	static constexpr unsigned short Version = 3;
};

Vault::Vault()
{
}

Vault::~Vault()
{
}

void Vault::Reset()
{
	mData.reset();
	mBuffer.reset();
	mLockSteps.clear();
	mBlock.reset();
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

	auto data = SecureArray(dataSize);
	if (stream.Read(data.data(), static_cast<uint32_t>(data.size())) != data.size())
		return false;

	mBuffer = std::move(data);
	mData = mBuffer.getRef();

	if (!ReadLockStep() || !DecryptStep(mLockSteps.front().name) || !ReadStep())
		return false;
	return true;
}

bool Vault::ReadLockStep()
{
	MemoryStream memory(mData.data(), mData.size());

	if (memory.GetReadableSize() < Crypto::PwSaltSize + Crypto::ChestNonceSize)
		return false;
	auto salt = SecureArray::CreateRef(memory.GetCurrentData(), Crypto::PwSaltSize);
	memory.Seek(salt.size());
	auto nonce = SecureArray::CreateRef(memory.GetCurrentData(), Crypto::ChestNonceSize);
	memory.Seek(nonce.size());
	
	unsigned int size;
	if (!memory.Read(size) || memory.GetReadableSize() < size)
		return false;
	auto data = SecureArray::CreateRef(memory.GetCurrentData(), size);
	memory.Seek(size);

	if (!memory.Read(size) || memory.GetReadableSize() < size)
		return false;
	auto name = SecureArray::CreateRef(memory.GetCurrentData(), size);

	mLockSteps.push_back(LockStep
		{
			.salt = std::move(salt),
			.nonce = std::move(nonce),
			.data = std::move(data),
			.name = std::move(name),
			.type = BlockType::Challenge,
		});
	return true;
}

bool Vault::Place(const std::wstring_view& file)
{
	std::error_code ec;
	auto bakFile = std::format(L"{}.bak", file);
	std::filesystem::remove(bakFile, ec);
	if (std::filesystem::exists(file, ec))
	{
		std::filesystem::rename(file, bakFile, ec);
		if (ec)
			return false;
	}

	FileWriter stream;
	if (!stream.Open(file))
		return false;

	VaultHeader header{};
	if (!Container::BeginWrite<VaultHeader>(stream, header))
		return false;

	auto block = mBlock.getRef();
	auto blockType = BlockType::Data;
	for (auto i = mLockSteps.rbegin(); i != mLockSteps.rend(); ++i)
	{
		auto& step = *i;

		if (step.nonce.size() != Crypto::ChestNonceSize)
			step.nonce = SecureArray(Crypto::ChestNonceSize);
		Crypto::FillRandomBytes(step.nonce);

		auto data = SecureArray(sizeof(BlockType) + block.size());
		MemoryStream stream(data.data(), data.size());
		if (!stream.Write(blockType) || !stream.Write(block.data(), block.size()))
			return false;

		step.data = Crypto::CreateChest(data, step.key, step.nonce);
		if (!step.data)
			return false;

		size_t dataSize = sizeof(unsigned int) + step.data.size() + sizeof(unsigned int) + step.name.size();
		dataSize += Crypto::PwSaltSize + Crypto::ChestNonceSize;
		data = SecureArray(dataSize);
		MemoryStream memory(data.data(), data.size());

		if (step.salt.size() != Crypto::PwSaltSize || memory.Write(step.salt.data(), step.salt.size()) != step.salt.size())
			return false;

		if (step.nonce.size() != Crypto::ChestNonceSize || memory.Write(step.nonce.data(), step.nonce.size()) != step.nonce.size())
			return false;
		
		auto size = static_cast<unsigned int>(step.data.size());
		if (static_cast<size_t>(size) != step.data.size() || !memory.Write(size))
			return false;

		if (memory.Write(step.data.data(), step.data.size()) != step.data.size())
			return false;

		size = static_cast<unsigned int>(step.name.size());
		if (static_cast<size_t>(size) != step.name.size() || !memory.Write(size))
			return false;

		if (memory.Write(step.name.data(), step.name.size()) != step.name.size())
			return false;

		block = std::move(data);
		blockType = BlockType::Challenge;
	}

	auto size = static_cast<uint32_t>(block.size());
	if (static_cast<uint64_t>(size) != block.size() || !stream.Write(block.data(), size))
		return false;

	if (!Container::EndWrite<VaultHeader>(stream))
		return false;
	return true;
}

SecureArray Vault::CreateKey(const std::string_view& password, SecureArray& salt)
{
	if (!salt)
	{
		salt = SecureArray(Crypto::PwSaltSize);
		Crypto::FillRandomBytes(salt);
	}
	return Crypto::HashPassword(password, salt);
}

bool Vault::DecryptStep(const SecureArray& key)
{
	if (mLockSteps.empty())
		return false;

	auto& step = mLockSteps.back();
	if (!Crypto::OpenChestInPlace(step.data, key, step.nonce))
		return false;
	step.key = SecureArray::Copy(key);
	return true;
}

bool Vault::ReadStep()
{
	if (mLockSteps.empty())
		return false;
	auto& step = mLockSteps.back();

	auto block = step.data.span(0, step.data.size() - Crypto::ChestExtraSize);
	MemoryStream memory(block.data(), block.size());
	if (!memory.Read(step.type))
		return false;
	block = block.span(sizeof(step.type));

	if (step.type == BlockType::Challenge)
	{
		auto oldBlock = mData;
		mData = block;
		if (!ReadLockStep())
		{
			mData = oldBlock;
			return false;
		}
	}
	else if (step.type == BlockType::Data)
		mBlock = block;
	return true;
}

void Vault::ResetSteps()
{
	mLockSteps.clear();

	auto key = SecureArray(Crypto::ChestKeySize);
	Crypto::FillRandomBytes(key);
	auto keyRef = key.getRef();
	auto salt = SecureArray(Crypto::PwSaltSize);
	Crypto::FillRandomBytes(salt);

	mLockSteps.push_back(LockStep
		{
			.key = std::move(key),
			.salt = std::move(salt),
			.name = std::move(keyRef),
			.type = BlockType::Challenge,
		});
}

bool Vault::AddStep(const SecureArray& name, const SecureArray& key, const SecureArray& salt)
{
	if (!key || !name || !salt)
		return false;

	mLockSteps.push_back(LockStep
		{
			.key = SecureArray::Copy(key),
			.salt = SecureArray::Copy(salt),
			.name = SecureArray::Copy(name),
			.type = BlockType::Challenge,
		});
	return true;
}
