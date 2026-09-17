#include "Vault.h"
#include "Crypto.h"
#include "Files/Container.h"
#include "Files/MemoryStream.h"

struct VaultHeader
{
	static constexpr unsigned short Magic = 0x5645;
	static constexpr unsigned short Version = 2;
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

	auto data = Crypto::AllocMemory(dataSize);
	if (!data)
		return false;

	auto ref = FixedArray<unsigned char>::CreateArrayRef(data, data.size());
	if (!stream.Read(ref))
		return false;
	mBuffer = std::move(data);
	mData = SecureArray::Wrap(mBuffer.str(), mBuffer.size(), nullptr);

	if (!ReadLockStep() || !UnlockStep(mLockSteps.front().name))
		return false;
	return true;
}

bool Vault::ReadLockStep()
{
	MemoryStream memory(mData, mData.size());

	BlockType type;
	if (!memory.Read(type))
		return false;

	if (memory.GetReadableSize() < Crypto::PwSaltSize + Crypto::ChestNonceSize)
		return false;
	auto salt = SecureArray::Wrap(memory.GetCurrentData(), Crypto::PwSaltSize, nullptr);
	memory.Seek(salt.size());
	auto nonce = SecureArray::Wrap(memory.GetCurrentData(), Crypto::ChestNonceSize, nullptr);
	memory.Seek(nonce.size());
	
	unsigned int size;
	if (!memory.Read(size) || memory.GetReadableSize() < size)
		return false;
	auto data = SecureArray::Wrap(memory.GetCurrentData(), size, nullptr);
	memory.Seek(size);

	if (!memory.Read(size) || memory.GetReadableSize() < size)
		return false;
	auto name = SecureArray::Wrap(memory.GetCurrentData(), size, nullptr);

	if (type == BlockType::Data)
		mData = SecureArray::Wrap(data, data.size(), nullptr);
	mLockSteps.push_back(LockStep
		{
			.salt = std::move(salt),
			.nonce = std::move(nonce),
			.data = std::move(data),
			.name = std::move(name),
			.type = type,
		});
	return true;
}

bool Vault::Place(const std::wstring_view& file)
{
	FileWriter stream;
	if (!stream.Open(file))
		return false;

	VaultHeader header{};
	if (!Container::BeginWrite<VaultHeader>(stream, header))
		return false;

	auto block = SecureArray::Wrap(mBlock, mBlock.size(), nullptr);
	auto blockType = BlockType::Data;
	for (auto i = mLockSteps.rbegin(); i != mLockSteps.rend(); ++i)
	{
		auto& step = *i;

		if (step.nonce.size() != Crypto::ChestNonceSize)
			step.nonce = Crypto::AllocMemory(Crypto::ChestNonceSize);
		Crypto::FillRandomBytes(step.nonce);

		auto content = std::string_view(block.str(), block.size());
		step.data = Crypto::CreateChest(content, step.key, step.nonce);
		if (!step.data)
			return false;

		size_t dataSize = sizeof(BlockType) + sizeof(unsigned int) + step.data.size() + sizeof(unsigned int) + step.name.size();
		dataSize += Crypto::PwSaltSize + Crypto::ChestNonceSize;
		auto data = Crypto::AllocMemory(dataSize);
		MemoryStream memory(data, data.size());

		step.type = blockType;
		if (!memory.Write(step.type))
			return false;

		if (step.salt.size() != Crypto::PwSaltSize || memory.Write(step.salt, step.salt.size()) != step.salt.size())
			return false;

		if (step.nonce.size() != Crypto::ChestNonceSize || memory.Write(step.nonce, step.nonce.size()) != step.nonce.size())
			return false;
		
		auto size = static_cast<unsigned int>(step.data.size());
		if (static_cast<size_t>(size) != step.data.size() || !memory.Write(size))
			return false;

		if (memory.Write(step.data, step.data.size()) != step.data.size())
			return false;

		size = static_cast<unsigned int>(step.name.size());
		if (static_cast<size_t>(size) != step.name.size() || !memory.Write(size))
			return false;

		if (memory.Write(step.name, step.name.size()) != step.name.size())
			return false;

		block = std::move(data);
		blockType = BlockType::Challenge;
	}

	if (!stream.Write(block, static_cast<uint32_t>(block.size())))
		return false;

	if (!Container::EndWrite<VaultHeader>(stream))
		return false;
	return true;
}

SecureArray Vault::CreateKey(const std::string_view& password, SecureArray& salt)
{
	if (!salt)
	{
		salt = Crypto::AllocMemory(Crypto::PwSaltSize);
		Crypto::FillRandomBytes(salt);
	}
	return Crypto::HashPassword(password, salt);
}

bool Vault::UnlockStep(const SecureArray& key)
{
	if (mLockSteps.size() < 1 || !key)
		return false;

	auto& step = mLockSteps.back();
	if (!Crypto::OpenChestInPlace(step.data, key, step.nonce))
		return false;
	step.key = Crypto::CopyMemory(key);

	mData = SecureArray::Wrap(step.data, step.data.size() - Crypto::ChestExtraSize, nullptr);
	if (step.type == BlockType::Challenge && !ReadLockStep())
		return false;
	else if (step.type == BlockType::Data)
		mBlock = SecureArray::Wrap(mData, mData.size(), nullptr);
	return true;
}

void Vault::ResetSteps()
{
	mLockSteps.clear();

	auto key = Crypto::AllocMemory(Crypto::ChestKeySize);
	Crypto::FillRandomBytes(key);
	auto keyRef = SecureArray::Wrap(key, key.size(), nullptr);
	auto salt = Crypto::AllocMemory(Crypto::PwSaltSize);
	Crypto::FillRandomBytes(salt);

	mLockSteps.push_back(LockStep
		{
			.key = std::move(keyRef),
			.salt = std::move(salt),
			.name = std::move(key),
			.type = BlockType::Challenge,
		});
}

bool Vault::AddStep(const SecureArray& name, const SecureArray& key, const SecureArray& salt)
{
	if (!key || !name || !salt)
		return false;

	mLockSteps.push_back(LockStep
		{
			.key = Crypto::CopyMemory(key),
			.salt = Crypto::CopyMemory(salt),
			.name = Crypto::CopyMemory(name),
			.type = BlockType::Challenge,
		});
	return true;
}
