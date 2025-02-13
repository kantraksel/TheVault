#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <future>
#include <list>
#include <functional>
#include "SecureArray.h"

class VaultKeeper
{
public:
	typedef std::future<uint64_t> Future;

private:
	std::vector<std::string> mHints; //protected by hintMutex
	std::vector<SecureArray> mKeyChain; //protected by hintMutex
	std::mutex hintMutex;
	std::wstring file; //used only in the thread

	std::jthread thread;
	std::condition_variable threadCvar;
	std::mutex taskMutex;
	std::list<struct Task> tasks; //protected by taskMutex

	void Run(std::stop_token token);
	Future SendCmd(const std::function<uint64_t()>& f);
	Future SendCmd(const std::function<uint64_t(const SecureArray&)>& f, const SecureArray& arg);
	Future SendCmd(const std::function<uint64_t(const std::wstring&)>& f, const std::wstring_view& arg);
	Future SendCmd(const std::function<uint64_t(const std::string&)>& f, const std::string_view& arg);

	uint64_t OpenVaultDeferred(const std::wstring& file);
	uint64_t CreateVaultDeferred(const std::wstring& file);
	uint64_t CloseVaultDeferred();
	uint64_t SubmitPasswordDeferred(const SecureArray& password);
	uint64_t SetHintKeyDeferred(int i, const SecureArray& password);
	uint64_t SaveVaultDeferred(bool close);

public:
	VaultKeeper();
	~VaultKeeper();

	void Init();
	void Shutdown();

	Future OpenVault(const std::wstring_view& file);
	Future CreateVault(const std::wstring_view& file);
	Future CloseVault();
	Future SaveVault();
	Future SaveCloseVault();

	void GetLastHint(std::string& str);
	Future SubmitPassword(const SecureArray& password);
	void ResetSalts();

	// direct api for LockSetup
	void LockDirectApi();
	void UnlockDirectApi();
	int GetHintCount();
	std::string_view GetHint(int i);
	bool IsKeyAssigned(int i);
	void AddHint(const std::string_view& hint);
	void RemoveHint(int i);
	Future SetHintKey(int i, const SecureArray& password);
	void ChangeHint(int i, const std::string_view& hint);
	
	uint64_t RaiseError(const std::string_view& msg, bool critical = false);
};
