#include "ImGuiUtils.h"
#include "LoginApplet.h"
#include "../../Game.h"
#include "../../Crypto.h"

extern Game game;

LoginApplet::LoginApplet()
{
	hint.reserve(256);
}

LoginApplet::~LoginApplet()
{
}

void LoginApplet::Initialize()
{
	passwordInput = Crypto::AllocMemory(256);
	if (!passwordInput)
	{
		game.GetKeeper().RaiseError("Failed to allocate memory", true);
		return;
	}
	Crypto::ZeroMemory(passwordInput);
}

void LoginApplet::OnLeave()
{
	hint.clear();
}

void LoginApplet::Render()
{
	BeginVisibleChildWindow("##Login", ImVec2(350, 0));

	Text("Complete challenge series to open the vault");
	ImGui::Separator();
	Text(hint);

	if (!vaultTask.valid())
	{
		ImGui::BeginGroup();

		SetNextRightButtonAlign("Submit");
		constexpr auto flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		bool submit = ImGui::InputText("##Password", passwordInput.str(), passwordInput.size(), flags);
		ImGui::SameLine();
		bool submit2 = ImGui::Button("Submit");
		if (submit || submit2)
		{
			vaultTask = game.GetKeeper().SubmitPassword(passwordInput);
			Crypto::ZeroMemory(passwordInput);
		}

		ImGui::EndGroup();
	}
	else
	{
		ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(-FLT_MIN, 0), "Decrypting...");
		game.GetMainWindow().ProcessVaultResponse(vaultTask);
	}

	ImGui::EndChild();
}

void LoginApplet::RefreshHintName()
{
	game.GetKeeper().GetLastHint(hint);
}
