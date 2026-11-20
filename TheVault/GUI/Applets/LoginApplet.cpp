#include "LoginApplet.h"
#include "Utils/ImGuiUtils.h"
#include "GUI/VaultCommander.hpp"
#include "GUI/Modals/ErrorModal.hpp"
#include "Vault/VaultKeeper.h"

LoginApplet::LoginApplet(MainWindow& mainWindow) : BlockingApplet(mainWindow), firstFrameDraw{false}, isProcessing{false}
{
	passwordInput.resize(256);
}

LoginApplet::~LoginApplet()
{
}

void LoginApplet::Open()
{
	BlockingApplet::Open();
	firstFrameDraw = true;
	isProcessing = false;
	RefreshHintName();
}

void LoginApplet::Close()
{
	BlockingApplet::Close();
	hint.clear();
	memset(passwordInput.data(), 0, passwordInput.size());
}

void LoginApplet::OnRender()
{
	bool isWindowOpen = isOpen;

	ImGui::SetNextWindowSize(ImVec2(350, 0));
	CenterNextWindow();
	if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopup))
		ImGui::SetNextWindowFocus();
	bool* pIsWindowOpen = !isProcessing ? &isWindowOpen : nullptr;
	ImGui::Begin("Enter password matching the hint:##Login", pIsWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	Text(hint);

	if (!isProcessing)
	{
		ImGui::BeginGroup();

		if (firstFrameDraw)
			ImGui::SetKeyboardFocusHere();

		SetNextRightButtonAlign("Unlock");
		constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		bool submit = ImGui::InputText("##Password", passwordInput.data(), passwordInput.size() - 1, flags);
		ImGui::SameLine();
		if (ImGui::Button("Unlock") || submit)
		{
			auto password = std::string_view(passwordInput.data());
			if (!password.empty())
			{
				isProcessing = true;
				VaultCommander::Get().SubmitPassword(password, [this](uint64_t result)
					{
						switch (result)
						{
							case VaultKeeper::Failed:
							{
								isProcessing = false;
								break;
							}

							case VaultKeeper::Success:
							{
								Close();
								break;
							}

							case VaultKeeper::UnlockRequired:
							{
								Close();
								Open();
								break;
							}

							default:
							{
								auto message = std::format("Unknown result code {} returned by SaveVault", result);
								ErrorModal::Get().Open(message, true);
								break;
							}
						}
					});
				memset(passwordInput.data(), 0, passwordInput.size());
			}
		}

		ImGui::EndGroup();
	}
	else
	{
		ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(-FLT_MIN, 0), "Decrypting...");
	}

	ImGui::End();

	if (firstFrameDraw)
		firstFrameDraw = false;

	if (!isWindowOpen)
	{
		Close();
		VaultCommander::Get().CloseVault();
	}
}

void LoginApplet::RefreshHintName()
{
	VaultKeeper::Get().GetLastHint(hint);
}
