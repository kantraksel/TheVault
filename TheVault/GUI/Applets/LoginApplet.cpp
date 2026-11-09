#include "Utils/ImGuiUtils.h"
#include "LoginApplet.h"
#include "App/Game.h"
#include "Vault/Crypto.h"

extern Game game;

LoginApplet::LoginApplet() : firstFrameDraw{}
{
	hint.reserve(256);
}

LoginApplet::~LoginApplet()
{
}

void LoginApplet::Initialize()
{
	passwordInput = SecureArray(256);
	Crypto::ZeroMemory(passwordInput);
}

void LoginApplet::OnEnter()
{
	firstFrameDraw = true;
}

void LoginApplet::OnLeave()
{
	hint.clear();
	Crypto::ZeroMemory(passwordInput);
}

void LoginApplet::Render()
{
	BeginVisibleChildWindow("##Login", ImVec2(350, 0));

	Text("Enter password matching the hint:");
	ImGui::Separator();
	Text(hint);

	if (!vaultTask.valid())
	{
		ImGui::BeginGroup();

		if (firstFrameDraw)
			ImGui::SetKeyboardFocusHere();

		SetNextRightButtonAlign("Unlock");
		constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		bool submit = ImGui::InputText("##Password", reinterpret_cast<char*>(passwordInput.data()), passwordInput.size(), flags);
		ImGui::SameLine();
		if (ImGui::Button("Unlock") || submit)
		{
			vaultTask = game.GetKeeper().SubmitPassword(passwordInput);
			Crypto::ZeroMemory(passwordInput);
		}

		ImGui::EndGroup();
	}
	else
	{
		ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(-FLT_MIN, 0), "Decrypting...");
		game.GetMainWindow().ProcessVaultResponse(vaultTask);
	}

	ImGui::EndChild();
	firstFrameDraw = false;
}

void LoginApplet::RefreshHintName()
{
	game.GetKeeper().GetLastHint(hint);
}
