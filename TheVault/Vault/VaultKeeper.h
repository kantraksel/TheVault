#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <future>
#include "Utils/SecureArray.h"

class VaultKeeper
{
public:
	typedef std::future<uint64_t> Future;

private:
	struct Layer
	{
		SecureArray key;
		SecureArray salt;
		std::string hint;
	};

	std::vector<Layer> mChain;
	std::mutex chainMutex;
	std::wstring file; //used only in the thread

	void ResetState();
	uint64_t RaiseError(const std::string_view& msg, bool critical = false);

public:
	VaultKeeper();
	~VaultKeeper();

	Future OpenVault(const std::wstring& file);
	Future CreateVault(const std::wstring& file);
	Future CloseVault();
	Future SaveVault();

	void GetLastHint(std::string& str);
	Future SubmitPassword(const std::string_view& password);

	struct DirectApi
	{
		VaultKeeper& keeper;
		std::unique_lock<std::mutex> lock;

		int GetHintCount();
		std::string_view GetHint(int i);
		bool IsKeyAssigned(int i);
	};
	DirectApi GetDirectApi();
	
	int AddHint(const std::string_view& hint);
	void RemoveHint(int i);
	Future SetHintKey(int i, const std::string_view& password);
	void ChangeHint(int i, const std::string_view& hint);

	enum ReturnValue
	{
		Failed,
		Success,
		UnlockRequired,
	};
};
