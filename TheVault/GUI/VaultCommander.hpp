#pragma once
#include "ReactiveScheduler.hpp"

class VaultCommander
{
private:
	ReactiveScheduler scheduler;

	void ProcessVaultTask(std::future<uint64_t>&& task, const Function<void(uint64_t)>& resolveCallback, const std::string_view& title = {});

public:
	VaultCommander();
	~VaultCommander();
	void Update();

	void CreateVault(const std::wstring& file);
	void OpenVault(const std::wstring& file);
	void SaveVault(bool closeVault = false, bool exitApp = false);
	void CloseVault(bool exitApp = false);
	void TryCloseVault(bool exitApp = false);

	void SubmitPassword(const std::string_view& password, const Function<void(uint64_t)>& resolveCallback);
	void SetPassword(int i, const std::string_view& password, const Function<void(uint64_t)>& resolveCallback);

	void ShowError(const std::string_view& text, bool critical);
};
