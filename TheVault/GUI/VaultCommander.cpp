#include "VaultCommander.hpp"
#include "Applets/LoginApplet.h"
#include "Applets/ProcessApplet.h"
#include "Engine/GhostFries.h"
#include "Modals/ConfirmExitModal.hpp"
#include "Modals/ErrorModal.hpp"
#include "Utils/WinApi.h"
#include "Vault/VaultKeeper.h"

VaultCommander::VaultCommander(UnsavedState& unsavedState) : unsavedState{unsavedState}
{

}
VaultCommander::~VaultCommander() = default;

void VaultCommander::Update()
{
	scheduler.Run();
}

void VaultCommander::ShowError(const std::string_view& text, bool critical)
{
	std::string message(text);
	scheduler.Schedule([=]()
	{
		ErrorModal::Get().Open(message, critical);
		return true;
	});
}

bool VaultCommander::CreateVault()
{
	std::wstring file;
	if (!WinApi::SaveFileDialog(L"Create new vault", L"vault.bin", file))
		return false;

	ProcessVaultTask(VaultKeeper::Get().CreateVault(file),
		[this](uint64_t)
		{
			ProcessVaultTask(VaultKeeper::Get().SaveVault(VaultKeeper::Get().GetVaultPath()),
				[this](uint64_t)
				{
					unsavedState.NotifyChange();
				});
		});
	return true;
}

bool VaultCommander::OpenVault()
{
	std::wstring file;
	if (!WinApi::OpenFileDialog(L"Open existing vault", L"vault.bin", file))
		return false;

	ProcessVaultTask(VaultKeeper::Get().OpenVault(file),
		[](uint64_t result)
		{
			switch (result)
			{
				case VaultKeeper::Failed:
					break;

				case VaultKeeper::UnlockRequired:
				{
					LoginApplet::Get().Open();
					break;
				}

				case VaultKeeper::Success:
					break;

				default:
				{
					auto message = std::format("Unknown result code {} returned by OpenVault", result);
					ErrorModal::Get().Open(message, true);
					break;
				}
			}
		});
	return true;
}

void VaultCommander::SaveVault(bool closeVault, bool exitApp)
{
	auto& keeper = VaultKeeper::Get();
	{
		auto api = keeper.GetDirectApi();
		int count = api.GetHintCount();
		if (count == 0)
		{
			ErrorModal::Get().Open("Vault must be encrypted with at least one hint");
			return;
		}

		for (int i = 0; i < count; ++i)
		{
			if (!api.IsKeyAssigned(i))
			{
				ErrorModal::Get().Open("All hint keys must be set");
				return;
			}
		}
	}

	auto file = keeper.GetVaultPath();
	if (file.empty() && !WinApi::SaveFileDialog(L"Save vault", L"vault.bin", file))
		return;

	ProcessVaultTask(keeper.SaveVault(file),
		[=, this](uint64_t result)
		{
			switch (result)
			{
				case VaultKeeper::Failed:
					break;

				case VaultKeeper::Success:
				{
					if (closeVault || exitApp)
						CloseVault(exitApp);
					break;
				}

				default:
				{
					auto message = std::format("Unknown result code {} returned by SaveVault", result);
					ErrorModal::Get().Open(message, true);
					break;
				}
			}
		},
		"Saving vault...");
}

void VaultCommander::TryCloseVault(bool exitApp)
{
	if (unsavedState.HasChanged())
		ConfirmExitModal::Get().Open(exitApp);
	else
		CloseVault(exitApp);
}

void VaultCommander::CloseVault(bool exitApp)
{
	ProcessVaultTask(VaultKeeper::Get().CloseVault(),
		[=](uint64_t)
		{
			if (exitApp)
				GhostFries::Quit();
		},
		"Closing vault...");
}

void VaultCommander::SubmitPassword(const std::string_view& password, const Function<void(uint64_t)>& resolveCallback)
{
	scheduler.Schedule<uint64_t>(VaultKeeper::Get().SubmitPassword(password),
		[=](uint64_t value)
		{
			if (resolveCallback)
				resolveCallback(value);
		}, {});
}

void VaultCommander::SetPassword(int i, const std::string_view& password, const Function<void(uint64_t)>& resolveCallback)
{
	scheduler.Schedule<uint64_t>(VaultKeeper::Get().SetHintKey(i, password),
		[=](uint64_t value)
		{
			if (resolveCallback)
				resolveCallback(value);
		}, {});
}

void VaultCommander::ProcessVaultTask(std::future<uint64_t>&& task, const Function<void(uint64_t)>& resolveCallback, const std::string_view& title)
{
	ProcessApplet::Get().Open(title);
	scheduler.Schedule<uint64_t>(std::move(task),
		[=](uint64_t value)
		{
			ProcessApplet::Get().Close();
			if (resolveCallback)
				resolveCallback(value);
		},
		[](const std::exception&)
		{
			ProcessApplet::Get().Close();
		});
}
