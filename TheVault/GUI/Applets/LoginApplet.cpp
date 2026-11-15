#include "Utils/ImGuiUtils.h"
#include "LoginApplet.h"
#include "App/Game.h"

extern Game game;

LoginApplet::LoginApplet() : isOpen{false}, firstFrameDraw{false}, isProcessing{false}
{
	passwordInput.resize(256);
}

LoginApplet::~LoginApplet()
{
}

void LoginApplet::Open()
{
	isOpen = true;
	firstFrameDraw = true;
	isProcessing = false;
	RefreshHintName();
}

void LoginApplet::Close()
{
	isOpen = false;
	hint.clear();
	memset(passwordInput.data(), 0, passwordInput.size());
}

void LoginApplet::Render()
{
	if (!isOpen)
		return;
	bool isWindowOpen = isOpen;

	ImGui::SetNextWindowSize(ImVec2(350, 0));
	CenterNextWindow();
	if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopup))
		ImGui::SetNextWindowFocus();
	ImGui::Begin("Enter password matching the hint:##Login", &isWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

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
				game.GetVaultCommander().SubmitPassword(password, [this](uint64_t result)
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
								game.GetMainWindow().GetMainApplet().EnableInteractions();
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
								Modals::GetErrorModal().Open(message, true);
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
		game.GetVaultCommander().CloseVault();
	}
}

void LoginApplet::RefreshHintName()
{
	game.GetKeeper().GetLastHint(hint);
}
