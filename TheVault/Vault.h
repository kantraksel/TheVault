#pragma once
#include <vector>
#include <string>
#include "SecureArray.h"

class Vault
{
private:
	SecureArray mData;

	SecureArray mKeySalt;
	SecureArray mLockNonce;
	SecureArray mFirstKey;

	std::vector<SecureArray> mLockSteps;
	SecureArray mEBlock;

public:
	Vault();
	~Vault();

	bool Initialize();
	void Reset();
	void ResetCache();
	bool Open(const std::wstring_view& file);
	bool Place(const std::wstring_view& file);

	size_t GetLockSteps() { return mLockSteps.size(); }
	SecureArray& GetBlock() { return mEBlock; }
	SecureArray& GetFirstKey() { return mFirstKey; }

	SecureArray CreateKey(const std::string_view& password);
	SecureArray CreateMasterKey(const std::vector<SecureArray>& keys, const SecureArray& lastKey);
	bool UnlockStep(const SecureArray& key, int i, SecureArray& plain);
	bool UnlockBlock(const SecureArray& key);

	void GenerateNew();
	void ResetSteps();
	bool AddStep(const SecureArray& plain, const SecureArray& key);
	bool LockBlock(const SecureArray& key, const std::string_view& content);
};
