#pragma once
#include <vector>
#include <string>
#include "SecureArray.h"

class Vault
{
private:
	SecureArray mBuffer;
	SecureArray mData;

	enum class BlockType
	{
		Challenge,
		Data,
	};

	struct LockStep
	{
		SecureArray key;
		SecureArray salt;
		SecureArray nonce;
		SecureArray data;
		SecureArray name;
		BlockType type;
	};
	std::vector<LockStep> mLockSteps;
	SecureArray mBlock;

	bool ReadLockStep();

public:
	Vault();
	~Vault();

	void Reset();
	bool Open(const std::wstring_view& file);
	bool Place(const std::wstring_view& file);

	size_t GetLockSteps() { return mLockSteps.size(); }
	SecureArray& GetBlock() { return mBlock; }
	SecureArray& GetStepSalt() { return mLockSteps.back().salt; }
	SecureArray& GetStepName() { return mLockSteps.back().name; }

	SecureArray CreateKey(const std::string_view& password, SecureArray& salt);
	bool UnlockStep(const SecureArray& key);

	void ResetSteps();
	bool AddStep(const SecureArray& name, const SecureArray& key, const SecureArray& salt);
};
