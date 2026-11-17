#include "VaultCommander.hpp"
#include "Modals.hpp"
#include "Engine/GhostFries.h"
#include "App/Game.h"

extern Game game;

VaultCommander::VaultCommander() = default;
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
		Modals::GetErrorModal().Open(message, critical);
		return true;
	});
}

void VaultCommander::CreateVault(const std::wstring& file)
{
	ProcessVaultTask(game.GetKeeper().CreateVault(file),
		[this](uint64_t)
		{
			ProcessVaultTask(game.GetKeeper().SaveVault(),
				[](uint64_t)
				{
					game.GetUnsavedState().NotifyChange();
					game.GetMainWindow().GetLockApplet().Open();
				});
		});
}

void VaultCommander::OpenVault(const std::wstring& file)
{
	ProcessVaultTask(game.GetKeeper().OpenVault(file),
		[](uint64_t result)
		{
			auto& main = game.GetMainWindow();
			switch (result)
			{
				case VaultKeeper::Failed:
				{
					main.GetWelcomeApplet().Open();
					main.DisableInteractions();
					break;
				}

				case VaultKeeper::UnlockRequired:
				{
					main.GetLoginApplet().Open();
					main.DisableInteractions();
					break;
				}

				case VaultKeeper::Success:
					break;

				default:
				{
					auto message = std::format("Unknown result code {} returned by OpenVault", result);
					Modals::GetErrorModal().Open(message, true);
					break;
				}
			}
		});
}

void VaultCommander::SaveVault(bool closeVault, bool exitApp)
{
	{
		auto api = game.GetKeeper().GetDirectApi();
		int count = api.GetHintCount();
		if (count == 0)
		{
			Modals::GetErrorModal().Open("Vault must be encrypted with at least one hint");
			return;
		}

		for (int i = 0; i < count; ++i)
		{
			if (!api.IsKeyAssigned(i))
			{
				Modals::GetErrorModal().Open("All hint keys must be set");
				return;
			}
		}
	}

	ProcessVaultTask(game.GetKeeper().SaveVault(),
		[=, this](uint64_t result)
		{
			switch (result)
			{
				case VaultKeeper::Failed:
				{
					game.GetMainWindow().GetLockApplet().Open();
					break;
				}

				case VaultKeeper::Success:
				{
					if (closeVault || exitApp)
						CloseVault(exitApp);
					break;
				}

				default:
				{
					auto message = std::format("Unknown result code {} returned by SaveVault", result);
					Modals::GetErrorModal().Open(message, true);
					break;
				}
			}
		},
		"Saving vault...");
}

void VaultCommander::TryCloseVault(bool exitApp)
{
	if (game.GetUnsavedState().HasChanged())
		Modals::GetConfirmExitModal().Open(exitApp);
	else
		CloseVault(exitApp);
}

void VaultCommander::CloseVault(bool exitApp)
{
	ProcessVaultTask(game.GetKeeper().CloseVault(),
		[=](uint64_t)
		{
			auto& main = game.GetMainWindow();
			main.GetWelcomeApplet().Open();
			main.DisableInteractions();
			if (exitApp)
				GhostFries::Quit();
		},
		"Closing vault...");
}

void VaultCommander::SubmitPassword(const std::string_view& password, const Function<void(uint64_t)>& resolveCallback)
{
	scheduler.Schedule<uint64_t>(game.GetKeeper().SubmitPassword(password),
		[=](uint64_t value)
		{
			if (resolveCallback)
				resolveCallback(value);
		}, {});
}

void VaultCommander::SetPassword(int i, const std::string_view& password, const Function<void(uint64_t)>& resolveCallback)
{
	scheduler.Schedule<uint64_t>(game.GetKeeper().SetHintKey(i, password),
		[=](uint64_t value)
		{
			if (resolveCallback)
				resolveCallback(value);
		}, {});
}

void VaultCommander::ProcessVaultTask(std::future<uint64_t>&& task, const Function<void(uint64_t)>& resolveCallback, const std::string_view& title)
{
	auto& main = game.GetMainWindow();
	main.GetProcessApplet().Open(title);
	main.DisableInteractions();
	scheduler.Schedule<uint64_t>(std::move(task),
		[=](uint64_t value)
		{
			auto& main = game.GetMainWindow();
			main.GetProcessApplet().Close();
			main.EnableInteractions();
			if (resolveCallback)
				resolveCallback(value);
		},
		[](const std::exception&)
		{
			auto& main = game.GetMainWindow();
			main.GetProcessApplet().Close();
			main.EnableInteractions();
		});
}
