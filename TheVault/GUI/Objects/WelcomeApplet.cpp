#include "ImGuiUtils.h"
#include "WelcomeApplet.h"
#include "../../Game.h"
#include "../../WinApi.h"

extern Game game;

WelcomeApplet::WelcomeApplet()
{
}

WelcomeApplet::~WelcomeApplet()
{
}

void WelcomeApplet::Render()
{
	BeginVisibleChildWindow("##Welcome", ImVec2(285, 0));

	Text("Welcome to TheVault");
	ImGui::Separator();

	static std::wstring name;
	if (ImGui::Button("Open existing vault") && WinApi::OpenFileDialog(L"Open existing vault", L"vault.bin", name))
	{
		game.GetMainWindow().ProcessVaultTask(game.GetKeeper().OpenVault(name));
	}
	ImGui::SameLine();
	if (ImGui::Button("Create new vault") && WinApi::SaveFileDialog(L"Create new vault", L"vault.bin", name))
	{
		game.GetMainWindow().ProcessVaultTask(game.GetKeeper().CreateVault(name));
	}

	ImGui::EndChild();
}
