#pragma once
#include <vector>
#include <string>
#include "Utils/SecureArray.h"

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
	SecureArray CreateKey(const std::string_view& password, SecureArray& salt);
	bool DecryptStep(const SecureArray& key);
	bool ReadStep();

	void ResetSteps();
	bool AddStep(const SecureArray& name, const SecureArray& key, const SecureArray& salt);
	bool Place(const std::wstring_view& file);

	SecureArray& GetBlock() { return mBlock; }
	SecureArray& GetStepSalt() { return mLockSteps.back().salt; }
	SecureArray& GetStepName() { return mLockSteps.back().name; }
};
